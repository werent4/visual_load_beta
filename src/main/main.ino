#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <cstdio>
#include <WiFi.h> // Wifi core lib
#include <PubSubClient.h> // mqtt core lib

#include <ArduinoJson.h> // JSON lib
#include <LittleFS.h> // file-system lib
#include <WebServer.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#include "headers_imgs/main_screen_data.h"
#include "headers_imgs/no_wifi_data.h"
#include "headers_imgs/wifi_data.h"
#include "html_content.h"


#define FORMAT_LITTLEFS_IF_FAILED true // formating file system if failed to ini
#define CUSTOM_GREEN ((0 << 11) | (62 << 5) | 20)


bool runServer = false; // start server flag
bool serverStarted = false; // monitor server state
bool server_postStarted = false; // monitor post handler server state

//##========= WebServer INI  ========= ##
WebServer server(80);
WebServer server_post(80);

//##========= WebServer FUNCTIONS  ========= ##
void startWebServer();
void startWevserver_post();
void handleRoot();
void handleSave();

//##========= WiFi INI  ========= ##
char* globalSSID = nullptr;  // SSID
char* globalPassword = nullptr; // PASSWORD

WiFiClient espClient;

//##========= WiFi FUNCTIONS  ========= ##
void setup_wifi();

//##========= Memory managment FUNCTIONS  ========= ## 
void updateGlobalCredentials(const char* ssid, const char* password);

//##========= SCREEN INI ========= ##

const int X_INI = 0;
const int Y_INI = 0;
const int FONT_SIZE = 2;

String RAM = "";
String CPU = "";
String GPU = "";

//##========= SCREEN FUNCTIONS  ========= ##
void drawtext(const char *text, uint16_t color, int w, int h, int s);

void setup() {
    Serial.begin(115200);
    delay(1000);
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
    tft.fillScreen(ST77XX_BLACK);  
    tft.setRotation(3);

    //##========= SCREEN INITIAL STATE! ========= ##
    // images
    tft.drawRGBBitmap(0, 0, main_screen_data, 240, 135);
    delay(1000);

    //##========= FILE SYSTEM INITIAL STATE! ========= ##
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
      Serial.println("Failed to initialize");
      return;
    }
    Serial.println("FIle System Initialized!");
    
    //##========= CHECK FOR CONFIG FILE ========= ##
    if (LittleFS.exists("/cfg.json")) {
        File configFile = LittleFS.open("/cfg.json", "r");
        if (configFile) {
          // Размер буфера для содержимого файла и JSON объекта
          size_t size = configFile.size();
          std::unique_ptr<char[]> buf(new char[size]);

          // Чтение данных в буфер
          configFile.readBytes(buf.get(), size);         
          
          // Создание JSON объекта
          DynamicJsonDocument doc(1024);
          auto deserializeError = deserializeJson(doc, buf.get());
          configFile.close();
            if (!deserializeError) {
                const char* ssid = doc["ssid"];       // Get SSID
                const char* password = doc["password"]; // Get password

                updateGlobalCredentials(ssid, password);

                Serial.println("SSID: " + String(globalSSID));
                Serial.println("Password: " + String(globalPassword));

                setup_wifi();
            } else {
                Serial.println("Failed to parse config file");
            }           
        }
    } else {
        Serial.println("Config file not found. Please visit 192.168.1.1 to configure.");
        runServer = true;
        startWebServer();
    }

}

void loop() {
  if (serverStarted) {
      server.handleClient();
  }
  if(server_postStarted){
    server_post.handleClient();
  }

  if (WiFi.status() != WL_CONNECTED){
    server_post.stop();
    server_postStarted = false;
    startWebServer();
  }
  else{
    tft.fillRect(X_INI + 55, Y_INI + 18, 165, 92, ST77XX_BLACK);// Clean

    drawtext(RAM.c_str(), CUSTOM_GREEN, X_INI + 80, Y_INI + 20, FONT_SIZE); // RAM
    drawtext(CPU.c_str(), CUSTOM_GREEN, X_INI + 80, Y_INI + 60, FONT_SIZE); // CPU
    drawtext(GPU.c_str(), CUSTOM_GREEN, X_INI + 57, Y_INI + 93, FONT_SIZE); // GPU

    delay(500);
  }
}
//##========= SCREEN FUNCTIONS  ========= ##

//draw text( text, color , x , y)
void drawtext(const char *text, uint16_t color, int w, int h, int s) {
  tft.setCursor(w, h);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(s);
  tft.print(text);
}

//##========= WebServer FUNCTIONS  ========= ##
void startWebServer(){
  if (!serverStarted){
    WiFi.mode(WIFI_OFF);
    tft.fillRect(X_INI + 55, Y_INI + 18, 165, 92, ST77XX_BLACK);// Clean
    drawtext("NO WiFi,", CUSTOM_GREEN, X_INI + 80, Y_INI + 50, FONT_SIZE);
    delay(15);
    drawtext("CONFIGURE", CUSTOM_GREEN, X_INI + 80, Y_INI + 65, FONT_SIZE);
    tft.drawRGBBitmap(204, 116, no_wifi_data, 15, 15);
    // Set your desired static IP configuration
    IPAddress localIP(192, 168, 1, 1);  // Change this to your preferred static IP
    IPAddress gateway(192, 168, 1, 1);  // Change this to your preferred gateway
    IPAddress subnet(255, 255, 255, 0); // Change this to your preferred subnet mask
    WiFi.softAPConfig(localIP, gateway, subnet);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP_load_checker");

    // Path handlers
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.begin();
    Serial.println("HTTP server started");
    serverStarted = true;
  }  
}

void startWevserver_post(){
  if (!server_postStarted){
    server_post.on("/data", HTTP_POST, handlePost);
    server_post.begin();
    Serial.println("HTTP server for POST requests started");
    server_postStarted = true;
  }

}

void handleRoot() {
  String html = HTML_CONTENT;

  server.send(200, "text/html", html);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  File configFile = LittleFS.open("/cfg.json", "w");
  if (!configFile) {
    server.send(500, "text/plain", "Failed to create file");
    return;
  }

  configFile.println("{\"ssid\":\"" + ssid + "\", \"password\":\"" + password + "\"}");
  configFile.close();

  server.send(200, "text/plain", "Saved SSID and Password");
}

void handlePost() {
    if (server_post.hasArg("plain") == false) { // Проверяем, есть ли данные в запросе
        server_post.send(500, "text/plain", "Server cannot parse POST data");
        return;
    }

    // Получаем содержимое тела запроса
    String postBody = server_post.arg("plain");

    // Объект для парсинга JSON
    DynamicJsonDocument doc(1024);
    auto error = deserializeJson(doc, postBody);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        server_post.send(500, "text/plain", "JSON parsing failed: " + String(error.c_str()));
        return;
    }

    // Извлекаем данные из JSON
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

    // Логика обработки данных
    Serial.printf("Received value: RAM=%s, CPU=%s, GPU=%s\n", newRAM, newCPU, newGPU);

    server_post.send(200, "text/plain", "Data successfully received and processed");
}


//##========= WiFi FUNCTIONS  ========= ##
void setup_wifi() {
  int maxAttempts = 10;
  int attemptCount = 0;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(globalSSID);

  WiFi.begin(globalSSID, globalPassword);

  while (WiFi.status() != WL_CONNECTED && attemptCount < maxAttempts) {
    delay(500);
    Serial.print(".");
    attemptCount++;

    if (attemptCount == maxAttempts) {
      Serial.println("Failed to connect to WiFi. Please configure network.");
      drawtext("NO WiFi", ST77XX_WHITE, X_INI + 70, Y_INI + 60, FONT_SIZE); 
      tft.drawRGBBitmap(204, 116, no_wifi_data, 15, 15);
      runServer = true;
      startWebServer();
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    server.stop();
    Serial.println("HTTP server stoped");
    runServer = false;
    serverStarted = false;
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    tft.drawRGBBitmap(204, 116, wifi_data, 15, 15);

    startWevserver_post();
    
  }
}


//##========= Memory managment FUNCTIONS  ========= ## 
void updateGlobalCredentials(const char* ssid, const char* password) {
    if (globalSSID) {
        free(globalSSID);
    }
    if (globalPassword) {
        free(globalPassword);
    }

    // Выделение памяти и копирование новых значений
    globalSSID = strdup(ssid);
    globalPassword = strdup(password);
}