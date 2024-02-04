/////////////////////////////////////////////////////////////////
/*

  Broadcasting Your Voice with ESP32-S3 & INMP441
  For More Information: https://youtu.be/qq2FRv0lCPw
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

/*
- Device
ESP32-S3 DevKit-C
https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html

- Required Library
Arduino ESP32: 2.0.9

Arduino Websockets: 0.5.3
https://github.com/gilmaimon/ArduinoWebsockets
*/


#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <I2S.h>


#define I2S_SD 10
#define I2S_WS 11
#define I2S_SCK 12
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define SAMPLE_BITS 16

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];
int msgLen = 0;

char* ssid = "WhiteSky-KnightsCircle";
char* password = "3rgxhbzu";

const char* websocket_server_host = "ec2-18-191-145-116.us-east-2.compute.amazonaws.com";
const uint16_t websocket_server_port = 8888;  // <WEBSOCKET_SERVER_PORT>

using namespace websockets;
WebsocketsClient client;
bool isWebSocketConnected;

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    isWebSocketConnected = true;
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
    isWebSocketConnected = false;
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up...");
  connectWiFi();
  connectWSServer();

  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop() {
  client.poll();
}

void connectWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void connectWSServer() {
  client.onEvent(onEventsCallback);
  client.onMessage([](WebsocketsMessage msg){
    String message = msg.data();
    msgLen += message.length();
    if (msgLen >= 25) {
      Serial.println("");
      msgLen = 0;
    }
    if (message != "") {
      Serial.print(message);
    }
  });

  while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
    Serial.print(",");
  }

  Serial.println("Websocket Connected!");
}


void micTask(void* parameter) {

  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, SAMPLE_BITS)) {
    Serial.println("Failed to initialize I2S!");
    while (1) ;
  }

  size_t bytesIn = 0;
  while (1) {
    esp_err_t result = esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
    if (result == ESP_OK && isWebSocketConnected) {
      client.sendBinary((const char*)sBuffer, bytesIn);
    } 
    else {
      Serial.println("uh oh");
      delay(100000);
    }
  }
}
