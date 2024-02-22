
const int BAUD = 19200;
const int CONSOLE_BAUD = 9600;
const String ID = "2";
const int WRITERATE = 1; //How often this writes to radio per second

long lastSend = millis();
String recievedMessage;
String sendMessage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(CONSOLE_BAUD);
  Serial1.begin(BAUD);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial1.available() > 0) {
    char out = Serial1.read();
    if (out == '\n') {
      Serial.println(recievedMessage);
      recievedMessage = "";
    } else {
      recievedMessage += out;
    }
  }

  if(Serial.available() > 0) {
    char input = Serial.read();
    if(input == '\n') {
      Serial1.println(sendMessage);
      sendMessage = "";
    } else {
      sendMessage += input;
    }
  }
}

