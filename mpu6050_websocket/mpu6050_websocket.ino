#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_MPU6050.h> // IMPORTANT!!! Set MPU6050_DEVICE_ID in Adafruit_MPU6050.h to 0x72 if code below does not work.
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Constants
const char* WIFI_SSID = "esp32_alsab";
const char* WIFI_PASS = "givelife";

// Globals
WebSocketsServer webSocket = WebSocketsServer(80);
bool connected = false;
Adafruit_MPU6050 mpu;

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      connected = false;
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        connected = true;
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
        while (connected) {
          sensors_event_t a, g, temp;
          mpu.getEvent(&a, &g, &temp);

          String payload = "Ax: " + String(a.acceleration.x) + ", Ay: " + a.acceleration.y + ", Az: " + a.acceleration.z + " m/s^2\nRx: " + g.gyro.x + ", Ry: " + g.gyro.y + ", Rz: " + g.gyro.z + " rad/s";

          webSocket.sendTXT(num, payload);
        }
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  Wire.setPins(15, 13); // sda, scl
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

  // Start access point
  Serial.println("Starting WiFi AP");
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  Serial.print("http://");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(myIP);

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {

  // Look for and handle WebSocket data
  webSocket.loop();
}