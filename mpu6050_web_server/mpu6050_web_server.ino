// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h> // IMPORTANT!!! Set MPU6050_DEVICE_ID in Adafruit_MPU6050.h to 0x72 if code below does not work.
#include <Adafruit_Sensor.h>
#include <Wire.h>
// #include <ArduinoJson.h>

// #include <esp32cam.h>
// #include <WebServer.h>
#include <WiFi.h>
#include "AsyncUDP.h"
// #include <esp_camera.h>

const char* WIFI_SSID = "esp32";
const char* WIFI_PASS = "givelife";

WiFiServer server(63000);

Adafruit_MPU6050 mpu;

// bool mpu_valid = false;

// void handleIMU()
// {
  // auto frame = esp32cam::capture();
  // if (frame == nullptr) {
  //   Serial.println("IMU CAPTURE FAIL");
  //   server.send(503, "", "");
  //   return;
  // }
  // Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                // static_cast<int>(frame->size()));

  // server.setContentLength(frame->size());
  // server.send(200, "application/plain");
  // WiFiClient client = server.client();
  // frame->writeTo(client);
// }

void serveImuJson()
{
  // print("request received")
  sensors_event_t a, g, temp;
  if(mpu.getEvent(&a, &g, &temp))
  {
   
    // Serial.print("Acceleration X: ");
    // Serial.print(a.acceleration.x);
    // StaticJsonDocument<512> data;
    // data["acceleration"];
    // data["acceleration"]["x"] = a.acceleration.x;
    // data["acceleration"]["y"] = a.acceleration.y;
    // data["acceleration"]["z"] = a.acceleration.z;

    // data["rotation"];
    // data["rotation"]["x"] = g.gyro.x;
    // data["rotation"]["y"] = g.gyro.y;
    // data["rotation"]["z"] = g.gyro.z;
    


    // char serializedData[512];
    // String serializedData = "acceleration: x: " + String(a.acceleration.x) +  "; y: " + String(a.acceleration.y) + "; z: " + String(a.acceleration.z) + "; rotation: x "+ String(g.gyro.x) +  "; y: " + String(g.gyro.y) + "; z: " + String(g.gyro.z) + ";";
    // char
    //  server.setContentLength(serializedData.length());
    // serializeJson(data, serializedData);
    // serializeJsonPretty(data, Serial);
    // WiFiClient client = server.client();
    // server.send(200, "text/plain");
    // client.write(serializedData.c_str(), serializedData.length());
    // server.send(200, "text/plain", "test");
  }
  else
  {
    // server.send(400, "text/plain", "error");    
  }
  
  
  // WiFiClient client = server.client();
  // frame->writeTo(client);

}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  Wire.setPins(13, 16); // sda, scl
  while (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    // while (1) {
    delay(10);
    // }
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
  // till here

  // below code is for connecting to WIFI
  // WiFi.persistent(false);
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASS);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  // }
  // till here

  Serial.print("http://");

  // below code is for setting up a WIFI AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(myIP);

  // Serial.println("  /cam.bmp");
  // Serial.println("  /cam-lo.jpg");
  Serial.println("/imu.txt");
  // Serial.println("  /cam.mjpeg");

  // server.on("/cam.bmp", handleBmp);
  // server.on("/cam-lo.jpg", handleJpgLo);
  // server.on("/imu.txt", serveImuJson);
  // server.on("/cam.jpg", handleJpg);
  // server.on("/cam.mjpeg", handleMjpeg);
  server.setNoDelay(true);

  server.begin();
}

void loop() {
  /* Get new sensor events with the readings */
  // sensors_event_t a, g, temp;
  // mpu.getEvent(&a, &g, &temp);

  // // /* Print out the values */
  // Serial.print("Acceleration X: ");
  // Serial.print(a.acceleration.x);
  // Serial.print(", Y: ");
  // Serial.print(a.acceleration.y);
  // Serial.print(", Z: ");
  // Serial.print(a.acceleration.z);
  // Serial.println(" m/s^2");

  // Serial.print("Rotation X: ");
  // Serial.print(g.gyro.x);
  // Serial.print(", Y: ");
  // Serial.print(g.gyro.y);
  // Serial.print(", Z: ");
  // Serial.print(g.gyro.z);
  // Serial.println(" rad/s");

  // Serial.print("Temperature: ");
  // Serial.print(temp.temperature);
  // Serial.println(" degC");

  // Serial.println("");
  // delay(500);
  // serveIMUTxt();
  // delay(500);
  // server.handleClient();
  WiFiClient client = server.available();
  while(client)
  {
    // uint8_t buffer[1024];
    sensors_event_t a, g, temp;
    if(mpu.getEvent(&a, &g, &temp))
    {
   
      // Serial.print("Acceleration X: ");
      // Serial.print(a.acceleration.x);
      // StaticJsonDocument<512> data;
      // data["acceleration"];
      // data["acceleration"]["x"] = a.acceleration.x;
      // data["acceleration"]["y"] = a.acceleration.y;
      // data["acceleration"]["z"] = a.acceleration.z;

      // data["rotation"];
      // data["rotation"]["x"] = g.gyro.x;
      // data["rotation"]["y"] = g.gyro.y;
      // data["rotation"]["z"] = g.gyro.z;



      // char serializedData[512];
      String serializedData = "acceleration: x: " + String(a.acceleration.x) +  "; y: " + String(a.acceleration.y) + "; z: " + String(a.acceleration.z) + "; rotation: x "+ String(g.gyro.x) +  "; y: " + String(g.gyro.y) + "; z: " + String(g.gyro.z) + ";\n";
      // char
      //  server.setContentLength(serializedData.length());
      // serializeJson(data, serializedData);
      // serializeJsonPretty(data, Serial);
      // WiFiClient client = server.client();
      // server.send(200, "text/plain");
      char buffer[128];
      memset(buffer, 0, 128);
      strcpy(buffer, serializedData.c_str());
      client.write(buffer, 128);
      // Serial.print(serializedData.length());
      client.flush();
      delay(100);
      // server.send(200, "text/plain", "test");
   }
    // client.write(buffer, 1024);

  }
}
