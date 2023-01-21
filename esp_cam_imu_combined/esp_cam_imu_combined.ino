#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_camera.h>
#include <WebSocketsServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Constants
const char* WIFI_SSID = "esp32_alsab";
const char* WIFI_PASS = "givelife";
// const char* WIFI_SSID = "Tello-Alison";
// const char* WIFI_PASS = "";

// Globals
WebServer server(81);
WebSocketsServer webSocket = WebSocketsServer(80);

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);

bool connected = false;
Adafruit_MPU6050 mpu;

TaskHandle_t WebServerTask;

void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

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

void webserverLauncher(void* pvParameters) {
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  for (;;) {
    server.handleClient();
  }
}

void setup()
{
  // Start Serial port
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

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(3);
    cfg.setJpeg(85);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  sensor_t* sensor = esp_camera_sensor_get();
  sensor->set_xclk(sensor, 0, 10000000); // 10Mhz seems to work well

  WiFi.softAP(WIFI_SSID, WIFI_PASS);

  Serial.println("Starting WiFi AP");
  Serial.print("http://");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(myIP);
  Serial.println("  /cam-hi.jpg");

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  server.on("/cam-hi.jpg", handleJpgHi);
  server.begin();

  xTaskCreate(webserverLauncher, "webserver_task", 10000, (void*)1, 1, &WebServerTask);
  Serial.println("Created task and moving to loop now");
}

void loop() {
  // Look for and handle WebSocket data
  webSocket.loop();
}