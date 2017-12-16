#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define ESP32
#include <SocketIOClient.h>
#include <WiFi.h>
#include <DHT.h>
#include<ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT22

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);
DHT dht(DHTPIN, DHTTYPE);

WiFiClient webClient;
SocketIOClient client;
const char* ssid = "FUINNO.LAB";
const char* password = "12345678";

const char* server = "192.168.0.102";
char host[] = "192.168.0.102";
int port = 3000;
extern String RID;
extern String Rname;
extern String Rcontent;
extern String Rfull;

unsigned long previousMillis = 0;
long interval = 4000;
byte ledR = 12;
byte ledG = 14; // internally pulled up
byte ledB = 13;
int LED = 27;
bool ledState = LOW;
byte ledArray[3] = {1, 2, 3};
byte R, G, B;
byte brightness = 255;
const boolean invert = false; // set true if common anode, false if common cathode

uint8_t color = 0;          // a value from 0 to 255 representing the hue

void setup() {
  Serial.begin(115200);
  delay(10);

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

  // Connect to the websocket server
  if (client.connect(host, port)) {
    Serial.printf("[SocIO] Connected to url: %s\n",  host);
    client.send("connection", "message", "2st ESP32 connected!" + WiFi.localIP().toString());
  } else {
    Serial.println("Connection failed.");
  }

  ledcAttachPin(ledR, 1);
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);

  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

  for (byte i = 0; i < 3; i++) {
    ledcWrite(ledArray[i], 255);  // test high output of all leds in sequence
    delay(1000);
    ledcWrite(ledArray[i], 0);
  }
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledState);

  /* initialize OLED with I2C address 0x3C */
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    client.heartbeat(0);

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      //Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Connect to the server
    if (webClient.connect(server, port)) {
      Serial.println("Connected");
      String req_uri = "/update?temp=" + String(t, 1) + "&humd=" + String(h, 1);
      webClient.print("GET " + req_uri + " HTTP/1.1\n"
                      + "Host: " + server + "\n"
                      + "Connection: close\n"
                      + "Content-Length: 0\n" + "\n\n");
      Serial.printf("Temperature %s - Humidity %s\r\n", String(t, 1).c_str(), String(h, 1).c_str());

    }
  }
  if (client.monitor())
  {
    if (RID == "hex") {
      String hexStr = Rcontent;
      long number = (long) strtol( &hexStr[0], NULL, 16);
      int r = number >> 16;
      int g = number >> 8 & 0xFF;
      int b = number & 0xFF;

      ledcWrite(1, r);
      ledcWrite(2, g);
      ledcWrite(3, b);
      Serial.println(Rcontent);
      RID = "";
    }

    if (RID == "weather-indication") {
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(Rfull);

      String weather = root["weather"][0];
      String city = root["city"][0];
      Serial.println(city);
      RID = "";
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(city);
      display.setTextColor(BLACK, WHITE);
      display.setTextSize(2);
      display.println(weather);
      display.display();
    }
  }
}

