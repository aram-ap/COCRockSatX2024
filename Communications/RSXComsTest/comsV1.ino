#define TX1 1;
#define RX1 0;

const int BAUD = 115200;
const int CONSOLE_BAUD = 19200;
const int ID = 1;
const int WRITERATE = 2; //How often this writes to radio per second

long lastSend = millis();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(CONSOLE_BAUD);
  Serial1.begin(BAUD);
}

void loop() {
  // put your main code here, to run repeatedly:
  int input = 0;
  if(Serial1.available() > 0) {
    char out = Serial.read();
    Serial.write(out);
  }

  if(millis() - lastSend > 1000/WRITERATE) {
    Serial1.write(ID + "\n");
    lastSend = millis();
  }
}

