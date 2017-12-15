#define ESP32
#include <SocketIOClient.h>

//FOR SERVO
#define COUNT_LOW 0
#define COUNT_HIGH 8888
#define TIMER_WIDTH 16

#include "esp32-hal-ledc.h"

//FOR OLED DISPLAY
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
SSD1306  display(0x3c, 21, 22);


int doorSensor = 19;
int state;

SocketIOClient client;
const char * ssid = "UNKNOWNWIFI";
const char * password = "brunei2017" ;

unsigned long previousMillis = 0;
long interval = 4000;
char host[] = "192.168.1.15";
int port = 3000 ;
extern String RID;
extern String Rcontent;
extern String Rfull;


//for buzzer
int freq = 2000;

//for oled display
int counter = 1;

void setup()
{
  pinMode(doorSensor, INPUT_PULLUP);

  //for servo
  ledcSetup(1, 50, TIMER_WIDTH); // channel 1, 50 Hz, 16-bit width
  ledcSetup(2, 50, TIMER_WIDTH); // channel 2, 50 Hz, 16-bit width
  ledcAttachPin(4, 1);   // GPIO 4 assigned to channel 1
  ledcAttachPin(5, 2);   // GPIO 4 assigned to channel 2

  Serial.begin(115200);
  delay(10);

  //for buzzer
  //  ledcSetup(0, 2000, 8); //channel 0, 2000 Hz, 8-bit width
  //  ledcAttachPin(26, 0); //GPIO 26 assigned to channel 0

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  //Waiting until its connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  if (client.connect(host, port)) {
    Serial.printf("Connected to url: %s\n",  host);
    client.send("connection", "message", "1st ESP32 connected!" + WiFi.localIP().toString());
  } else {
    Serial.println("Connection failed.");
  }
}



void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    client.heartbeat(0);
  }

  // clear the display
  display.clear();
  
  
  doorCheck();

  //When catching any event, we have 2 parameters:
  //  +RID: Event's name
  //  +RFull: List of parameters which are being put in JSON string!
  if (client.monitor()) {

    //FOR DOOR LOCK 1---------------------------------------------------
    if (RID == "servo1") {
      //Print out to Serial monitor
      //      Serial.print(RID);
      //      Serial.print(' ');
      //      Serial.println(Rfull);
      if (Rfull == "1") {
        for (int i = COUNT_LOW; i < COUNT_HIGH; i = i + 100) {
          ledcWrite(1, i);
          delay(5);
          drawKitchenLock();
          // write the buffer to the display
          display.display();
        }
      }
      if (Rfull == "0") {
        for (int i = COUNT_HIGH; i > COUNT_LOW; i = i - 100) {
          ledcWrite(1, i);
          delay(5);
          drawKitchenUnlock();
          // write the buffer to the display
          display.display();
        }
      }
      RID = "";
    }


    //FOR DOOR LOCK 2----------------------------------------------
    if (RID == "servo2") {
      //Print out to Serial monitor
      //Serial.print(RID);
      //Serial.print(' ');
      //Serial.println(Rfull);
      if (Rfull == "1") {
        for (int i = COUNT_LOW; i < COUNT_HIGH; i = i + 100) {
          ledcWrite(2, i);
          delay(5);
          drawLrLock();
          // write the buffer to the display
          display.display();
        }
      }
      if (Rfull == "0") {
        for (int i = COUNT_HIGH; i > COUNT_LOW; i = i - 100) {
          ledcWrite(2, i);
          delay(5);
          drawLrUnlock();
          // write the buffer to the display
          display.display();
        }
      }
      RID = "";
    }


  //FOR DOOR LOCK3----------------------------------------------
    if (RID == "servo3") {
      //Print out to Serial monitor
      //Serial.print(RID);
      //Serial.print(' ');
      //Serial.println(Rfull);
      if (Rfull == "1") {
        for (int i = COUNT_LOW; i < COUNT_HIGH; i = i + 100) {
          ledcWrite(2, i);
          delay(5);
          drawBrLock();
          // write the buffer to the display
          display.display();
        }
      }
      if (Rfull == "0") {
        for (int i = COUNT_HIGH; i > COUNT_LOW; i = i - 100) {
          ledcWrite(2, i);
          delay(5);
          drawBrUnlock();
          // write the buffer to the display
          display.display();
        }
      }
      RID = "";
    }



  
  }
  //Reconnecting
  if (!client.connected()) {
    client.reconnect(host, port);
  }
}


void doorCheck() {
  state = digitalRead(doorSensor);
  if (state == 1) {
    client.send("OPEN", "message", "OPENED!");
  } else {
    client.send("OPEN", "message", "CLOSED!");
  }
  delay(1000);
}



void drawKitchenLock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "KITCHEN:");
    display.drawString(0, 28, "LOCKED!");
}
void drawKitchenUnlock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "KITCHEN:");
    display.drawString(0, 28, "UNLOCKED!");
}

void drawLrLock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "LIVING ROOM:");
    display.drawString(0, 28, "LOCKED!");
}

void drawLrUnlock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "LIVING ROOM:");
    display.drawString(0, 28, "UNLOCKED!");
}

void drawBrLock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "BEDROOM:");
    display.drawString(0, 28, "LOCKED!");
}

void drawBrUnlock() {
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "BEDROOM:");
    display.drawString(0, 28, "UNLOCKED!");
}

void drawWelcomeText(){
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "WELCOME");
    display.drawString(0, 10, "TO");
    display.drawString(0, 26, "ROBOTICS LAB");
}

void drawProgressBar() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

