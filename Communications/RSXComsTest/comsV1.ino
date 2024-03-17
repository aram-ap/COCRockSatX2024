const int BAUD = 57600;
const int CONSOLE_BAUD = 57600;
const String ID = "1";

long lastSend = millis();
String recievedMessage;
String sendMessage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(CONSOLE_BAUD);
  Serial1.begin(BAUD);

  Serial.println("Starting");

  Serial1.println("1 Transmitted");
  // Serial1.
}

void loop() {
  // put your main code here, to run repeatedly:

  // if(millis() - lastSend > 1000/WRITERATE) {
  //   Serial.println("SENT: " + ID);
  //   Serial1.println(ID.c_str());
  //   // Serial1.println(ID);
  //   lastSend = millis();

  // }


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
      Serial1.print(ID.c_str());
      Serial1.println(": " + sendMessage);

      Serial.print(ID.c_str());
      Serial.println(": " + sendMessage);

      sendMessage = "";
    } else {
      sendMessage += input;
    }
  }

  // int lastRead = 0;
  // while(Serial1.available() > 0) {
  //   lastRead = Serial1.read();
  //   char out = (char)lastRead;
  //   Serial.print("Recieved: " + Serial1.readString());
  // }

}

