// #include <Adafruit_MPL3115A2.h>
// #include <SPI.h>
#include <string>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
using namespace std;

const int UPDATE_RATE_MS = 1000;
const int MODEM_BAUD = 57000;
int num = 0;

// i2c
LSM9DS1 imu;


// Adafruit_MPL3115A2 baro;

long lastSensorReading = millis();

void setup() {
  // put your setup code here, to run once:

  // Serial3.begin(MODEM_BAUD);
  Serial.begin(19200);

  Serial.println("Magnetometer Capsule initialized");
  // Serial3.println("Magnetometer Capsule initialized");

  Wire.begin();


  if (imu.begin() == false) // with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                   "work for an out of the box LSM9DS1 " \
                   "Breakout, but may need to be modified " \
                   "if the board jumpers are.");
    while (1);
  }
}

void loop() {
  if(millis() - lastSensorReading > UPDATE_RATE_MS) {
    lastSensorReading = millis();

    int altitude = baro.getAltitude() * 100;
    int temperature = baro.getTemperature() * 1000;
    int pressure = baro.getPressure() * 1000;

    JsonDocument reading;
    reading["ID"] = "MAGN";
    reading["SEC"] = millis()/1000;

    // // m/s^2
    reading["ACX"] = a.acceleration.x;
    reading["ACY"] = a.acceleration.y;
    reading["ACZ"] = a.acceleration.z;

    // // uT
    reading["MGX"] = m.magnetic.x;
    reading["MGY"] = m.magnetic.y;
    reading["MGZ"] = m.magnetic.z;

    // // rad/s
    reading["GYX"] = g.gyro.x;
    reading["GYY"] = g.gyro.y;
    reading["GYZ"] = g.gyro.z;

    reading["ALT"] = (float)altitude/100.0;     // m
    reading["TEM"] = (float)temperature/1000.0;        // C
    reading["PRE"] = (float)pressure/1000.0;    // hPa

    reading["CNT"] = num++;

    serializeJson(reading, Serial);
    // serializeJson(reading, Serial3);
    Serial.println();
    // Serial3.println();
  }
}

void setupSensor() {

}
