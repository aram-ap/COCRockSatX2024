#include <SD.h>
#include <SPI.h>

/*
Cam:
Step  29
Dir   28

Step 2:
Step  11
Dir   10

Step 3:
Step  2
Dir   1

Step 4:
Step  0
Dir   4
*/

#define STEP1 29
#define DIR1 28

#define STEP2 11
#define DIR2 10

#define STEP3 2
#define DIR3 1

#define STEP4 0
#define DIR4 4

#define SLP_RST 24

#define TE2_SENSE 36

#define CAM_PIN 19

// SERIAL ---------------------------------------------
const int MODEM_BAUD = 57000;
const int CONSOLE_BAUD = 19200;

// SD -------------------------------------------------
const char* FILENAME = "data.txt";
// ChipSelect is assuming Teensy 4.1
const int chipSelect = BUILTIN_SDCARD;


// EJECTION -------------------------------------------
const double MIN_STEP_PULSE = 1.9; // Microseconds
const int STEPS_PER_REV = 200; // 1.8 degrees

// TE1 +3 s
// TE2 +186 s

const int TE1 = 3; //seconds
// const int TE2 = 186; //seconds
const int TE2 = 186 - TE1;

const double CAM_SPEED = 2; //Rotations per second - before 1:10 ratio gearbox
const double LOCK_SPEED = .8; //Rotations per second
const double CAM_DELTA_ROT = 360 * 51 * 2; //Degrees moved
const double LOCK_DELTA_ROT = 120 * 19; //Degrees moved
const double SECONDS_FROM_LOCK_TO_CAM = .5;

double _timeSinceStartup = TE1 * 1000; //Ms
int lastTimeOutTick = 0;

bool ejectionStarted = false;

// Messages
String recievedMessage;
String sendMessage;

// Camera

const int VidClipSeconds = 15;
const int CamAutoSaveBeginAfterTE2 = 15; // Seconds after TE2 to begin auto video saving;
const int CamToggleDelay = 1000;
bool Te2_began = false;
long LastCameraToggle = millis();

void setup() {
  // put your setup code here, to run once:

  Serial.begin(CONSOLE_BAUD);
  Serial4.begin(MODEM_BAUD);

  Serial.println("Starting up");

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(SLP_RST, OUTPUT);
  
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);

  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  pinMode(STEP3, OUTPUT);
  pinMode(DIR3, OUTPUT);

  pinMode(STEP4, OUTPUT);
  pinMode(DIR4, OUTPUT);

  pinMode(TE2_SENSE, INPUT);

  pinMode(CAM_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(CAM_PIN, LOW);
  // Setup SD card file writing
  checkSD();


  // This guy can be hooked up to the function you have recieving data from the capsules
  writeCard("Flight computer initializing");

  delay(3000);

  toggleVideo();
}

void loop() {
  _timeSinceStartup = millis() + TE1 * 1000;
  while(Serial4.available() > 0) {
    char out = Serial4.read();
    if (out == '\n') {
      Serial.println(recievedMessage);
      writeModemData(recievedMessage);
      recievedMessage = "";
    } else {
      recievedMessage += out;
    }
  }

  // if TE2 activation has already began and time has past the mark of auto saving, then autosave
  if(Te2_began && millis()/1000 - LastCameraToggle/1000 > VidClipSeconds) {
    toggleVideo();
  }

  // if(millis()/1000 >= TE2 && millis()/1000 - LastCameraToggle > CamAutoSaveBeginAfterTE2) {
  //   toggleVideo();
  // }

  if(digitalRead(TE2_SENSE) && !ejectionStarted) {
    initializeEjectionSequence();
  }
}

void toggleVideo() {
  LastCameraToggle = millis();
  digitalWrite(CAM_PIN, LOW);
  delay(CamToggleDelay/2);
  digitalWrite(CAM_PIN, HIGH);
  delay(CamToggleDelay/2);
  digitalWrite(CAM_PIN, LOW);
  writeCard("New video clip beginning");
}


// Just checks to make sure the card is hooked up okay
void checkSD() {
  if(!SD.begin(chipSelect)) {
    Serial.println("SD card fail");
    return;
  }
}

// Used to append to the file stored on the SD card
void writeCard(String msg) {
  digitalWrite(LED_BUILTIN, LOW);

  File dataFile = SD.open(FILENAME, FILE_WRITE);
  long time = millis();

  String dataStr = "|";
  dataStr += (millis()/1000);
  dataStr += "\t| ";

  dataStr += msg;
  if (dataFile) {
    dataFile.println(dataStr);
    dataFile.close();
  } else {
    Serial.println("Error writing to card");
  }

  digitalWrite(LED_BUILTIN, HIGH);
}

void writeModemData(String msg) {
  digitalWrite(LED_BUILTIN, LOW);

  File dataFile = SD.open(FILENAME, FILE_WRITE);
  if(dataFile) {
    dataFile.println(msg);
    dataFile.close();
  } else {
    Serial.println("Error writing to card");
  }
  digitalWrite(LED_BUILTIN, HIGH);
} 

void initializeEjectionSequence() {
  Serial.println("Initializing Ejection Sequence");
  writeCard("Initializing Ejection Sequence");

  digitalWrite(SLP_RST, HIGH);
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);

  delay(1000);
  
  ejectionStarted = true;
  //Unlatch
  int numStepsLock = (int)((double)STEPS_PER_REV * (double)LOCK_DELTA_ROT/360.0);
  double lockPulseWaitTime = (1000 / LOCK_SPEED);
  // Serial.print("numStepsLock:");
  // Serial.println(numStepsLock);
  // Serial.print("lockPulseWaitTime:");
  // Serial.println(lockPulseWaitTime);
  
  Serial.println("Unlatching Capsules");
  writeCard("Unlatching Capsules");

  for(int steps = 0; steps < numStepsLock; steps++) {
    digitalWrite(STEP2, HIGH);
    digitalWrite(STEP3, HIGH);
    digitalWrite(STEP4, HIGH);
    delayMicroseconds(lockPulseWaitTime);
    digitalWrite(STEP2, LOW);
    digitalWrite(STEP3, LOW);
    digitalWrite(STEP4, LOW);  
    delayMicroseconds(lockPulseWaitTime);
  }

  Serial.println("Unlatching completed");
  writeCard("Unlatching completed");

  delay(1000 * SECONDS_FROM_LOCK_TO_CAM);

  int numStepsCam = (int)((double)STEPS_PER_REV * (double)CAM_DELTA_ROT/360.0);
  double camPulseWaitTime = (1000 / CAM_SPEED);
  
  Serial.println("Running Cam");
  writeCard("Running Cam");
  for(int steps = 0; steps < numStepsCam; steps++) {
    digitalWrite(LED_BUILTIN, LOW);

    digitalWrite(STEP1, HIGH);
    delayMicroseconds(400);
    digitalWrite(STEP1, LOW); 
    delayMicroseconds(400);

    digitalWrite(LED_BUILTIN, HIGH);
  }
  Serial.println("Finished Running Cam");

  digitalWrite(SLP_RST, LOW);
  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);
  digitalWrite(STEP3, LOW);
  digitalWrite(STEP4, LOW);
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  Te2_began = true;
  LastCameraToggle = millis();
}


