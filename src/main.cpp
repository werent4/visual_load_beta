#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <cstdio>
#include <WiFi.h> // Wifi core lib
#include <PubSubClient.h> // mqtt core lib
#include <ArduinoJson.h> // JSON lib

#include "headers_imgs/CPU_data.h"
#include "headers_imgs/GPU_data.h"
#include "headers_imgs/RAM_data.h"

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
//##========= WiFi INI  ========= ##
const char* ssid = "TPLink_repeat";
const char* password = "VcnS4ybmz";

WiFiClient espClient;

//##========= WiFi FUNCTIONS  ========= ##
void setup_wifi();

//##========= MQTT INI  ========= ##
const int PORT = 1883;
const char* mqtt_server = "10.0.59.200";
const char* USERNAME = "werent4_test";
const char* PASSWORD = "werent4test";
const char* TOPIC = "show_load";

//##========= MQTT FUNCTIONS  ========= ##
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

PubSubClient client(mqtt_server, PORT, callback, espClient);

//##========= SCREEN INI ========= ##

const int X_INI = 46;
const int Y_INI = 18;
const int FONT_SIZE = 2;

String RAM = "";
String CPU = "";
String GPU = "";

//##========= SCREEN FUNCTIONS  ========= ##
void drawtext(const char *text, uint16_t color, int w, int h, int s); 

void setup() {
  Serial.begin(115200);  
  //##========= SCREEN_INIT ========= ##
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  
  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240);  
  tft.fillScreen(ST77XX_WHITE);  
  tft.setRotation(3);
  //##========= ========= ========= ##

  //##========= SCREEN INITIAL STATE! ========= ##
  // images
  tft.drawRGBBitmap(0, 6, RAM_data, 40, 40);
  tft.drawRGBBitmap(0, 52, CPU_data, 40, 40);
  tft.drawRGBBitmap(0, 92, GPU_data, 40, 40);

  // RAM part
  drawtext("", ST77XX_BLACK, X_INI, Y_INI , FONT_SIZE); 
  drawtext("RAM:" , ST77XX_BLACK, X_INI, Y_INI, FONT_SIZE); 
  //drawtext("300/300 GB" , ST77XX_BLACK, X_INI + 61, Y_INI, FONT_SIZE); 

  // CPU part
  drawtext("", ST77XX_BLACK, X_INI, Y_INI + 42, FONT_SIZE); 
  drawtext("CPU:" , ST77XX_BLACK, X_INI, Y_INI + 42, FONT_SIZE); 
  //drawtext("300" , ST77XX_BLACK, X_INI + 61, Y_INI + 42, FONT_SIZE); 

  // GPU part
  drawtext("", ST77XX_BLACK, X_INI + 5, Y_INI + 85, FONT_SIZE); 
  drawtext("GPU:" , ST77XX_BLACK, X_INI + 5, Y_INI + 85, FONT_SIZE); 
  //drawtext("300/300 GB" , ST77XX_BLACK, X_INI + 61, Y_INI + 85, FONT_SIZE); 
  //##========= ========= ========= ##

  //##========= WiFi INITIAL STATE! ========= ##
  setup_wifi();
  //##========= ========= ========= ##

  //##========= MQTT INITIAL STATE!  ========= ## 
  client.setServer(mqtt_server, PORT);
  client.setCallback(callback);
  client.setKeepAlive(60);
  //##========= ========= ========= ##
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // RAM
  tft.fillRect(X_INI + 61, Y_INI, 134, 28, ST77XX_WHITE);// Clean
  drawtext(RAM.c_str(), ST77XX_BLACK, X_INI + 61, Y_INI, FONT_SIZE); // Draw new
  // CPU
  tft.fillRect(X_INI + 61, Y_INI + 42, 134, 28, ST77XX_WHITE);// Clean
  drawtext(CPU.c_str(), ST77XX_BLACK,  X_INI + 61, Y_INI + 42, FONT_SIZE); // Draw new
  // GPU
  tft.fillRect(X_INI + 61, Y_INI + 85, 134, 28, ST77XX_WHITE);// Clean
  drawtext(GPU.c_str(), ST77XX_BLACK, X_INI + 61, Y_INI + 85, FONT_SIZE); // Draw new

  delay(500);  
}

//##========= SCREEN FUNCTIONS  ========= ##

//draw text( text, color , x , y)
void drawtext(const char *text, uint16_t color, int w, int h, int s) {
  tft.setCursor(w, h);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
  tft.setTextSize(s);
}

//##========= WiFi FUNCTIONS  ========= ##
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//##========= MQTT FUNCTIONS  ========= ##
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(". Message: ");
  String message_mqtt;


  if (String(topic) == TOPIC){
    char json[length + 1];
    strncpy(json, (char*)message, length);
    json[length] = '\0';

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    } 

    const char* newRAM = doc["RAM"];
    const char* newCPU = doc["CPU"];
    const char* newGPU = doc["GPU"];

    // Update RAM
    if (String(newRAM) != RAM) {
        RAM = String(newRAM);
        Serial.println("Updated RAM: " + RAM);
    }

    // Update CPU
    if (String(newCPU) != CPU) {
        CPU = String(newCPU);
        Serial.println("Updated CPU: " + CPU);
    }

    // Update GPU
    if (String(newGPU) != GPU) {
        GPU = String(newGPU);
        Serial.println("Updated GPU: " + GPU);
    }
  }
  else{
    for (int i = 0; i < length; i++) {
      message_mqtt += (char)message[i];
    }
    Serial.print(message_mqtt);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32S3Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), USERNAME, PASSWORD))  {
      Serial.println("connected");
      // resubscribe
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}