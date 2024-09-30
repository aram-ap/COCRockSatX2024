#include <Arduino_JSON.h>
#include <assert.h>
#include <DGS.h>
 
// Adafruit_MPL3115A2 mpl;
 
const int MODEM_BAUD = 57600;
const int CONSOLE_BAUD = 57600;
const String ID = "SO2-A";
const double UPDATE_RATE = 1.0;

// Adafruit_MPL3115A2 baro;
DGS so2(&Serial8);
JSONVar doc;

void setup() {
  Serial.begin(CONSOLE_BAUD); //Serial to computer
  Serial5.begin(MODEM_BAUD); //Modem Serial
  Serial8.begin(9600); //SO2 Serial
  Serial8.flush();

  so2.DEBUG = false;

  pinMode(21, INPUT_PULLUP);

  
  // baro.begin(TwoWire *twoWire = &Wire); 
  Serial.println("Starting SO2 Capsule A");
  // baro.setMode(MPL3115A2_BAROMETER);
  // baro.setSeaPressure(1013.26);
}
 
void loop() {

  // baro.startOneShot();
  // float pressure = baro.getPressure();
  // float altitude = baro.getAltitude();
  // float temperature = baro.getTemperature();

  // Allocate the JSON Document

  // Add values in the document
  // doc["pressure"] = pressure;
  // doc["altitude"] = altitude;
  // doc["temperature"] = temperature;
  doc["concentration"] = so2.getConc();
  doc["tempF"] = so2.getTemp('F');
  doc["humidity"] = so2.getRh();
  doc["time"] = millis();

  doc["test"] = "hello world";
  String jsonString = JSON.stringify(doc);
  Serial5.println(jsonString);
  Serial.println(jsonString);

  


  // serializeJson(doc, Serial5);
  // serializeJsonPretty(doc, Serial);
 
  delay(1000/UPDATE_RATE);
}