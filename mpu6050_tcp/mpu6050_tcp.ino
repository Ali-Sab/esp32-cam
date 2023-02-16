// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h> // IMPORTANT!!! Set MPU6050_DEVICE_ID in Adafruit_MPU6050.h to 0x72 if code below does not work.
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>

const char* WIFI_SSID = "esp32";
const char* WIFI_PASS = "givelife";

WiFiServer server(63000);
Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  Wire.setPins(15, 13); // sda, scl
  while (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");

    delay(10);
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  // below code is for setting up WIFI AP
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);

  Serial.print("http://");

  // below code is for setting up a WIFI AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);

  server.setNoDelay(true);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client)
  {
    sensors_event_t a, g, temp;

    while (client.connected())
    {
      if (mpu.getEvent(&a, &g, &temp))
      {
        String serializedData = String(a.acceleration.x) + ";" + String(a.acceleration.y) + ";" + String(a.acceleration.z) + ";" + String(g.gyro.x) +  ";" + String(g.gyro.y) + ";" + String(g.gyro.z);

        // Serial.flush();
        // Serial.println(serializedData);
        if (client.available()) {
          client.write(serializedData.c_str());
          client.flush();
        }
        
        delayMicroseconds(1000);
      }      
    }
  }
}