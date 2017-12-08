#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h> // Add to the sketch of the Serial Command library   

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

const byte RX = 3 ; // Pin 3 is used as pin RX
const byte TX = 2 ; // Pin 2 is used for TX pins

SoftwareSerial mySerial = SoftwareSerial ( RX , TX );

SerialCommand sCmd ( mySerial ); // Declare variables using the Serial Command library

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

int red = 4;
int blue = 5;
int relaylightbulb1 = 8;
int relaylightbulb2 = 9;
int relaylightbulb3 = 10;
int relaylightbulb4 = 11;

const unsigned long timeinterval = 500UL ; // Every 2000ms, the cycle is repeated

void setup () {
  Serial.begin(57600);
  mySerial.begin(57600);

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(relaylightbulb1, OUTPUT);
  pinMode(relaylightbulb2, OUTPUT);
  pinMode(relaylightbulb3, OUTPUT);
  pinMode(relaylightbulb4, OUTPUT);

  digitalWrite(relaylightbulb1, LOW);
  digitalWrite(relaylightbulb2, LOW);
  digitalWrite(relaylightbulb3, LOW);

  sCmd.addCommand("led", led); // When the LED command is executed, the leds function will be executed
  sCmd.addCommand("switch1", switch1);
  sCmd.addCommand("switch2", switch2);
  sCmd.addCommand("switch3", switch3);
  sCmd.addCommand("switch4", switch4);
  sCmd.addCommand("servo", doorSwitch1);
}


//unsigned long interval1 = 0 ;
void loop () {
  sCmd.readSerial();
}


void switch1() {
  char * json = sCmd.next();
  int relayLightStatus1 = (int)json[0] - 48;
  Serial.println(relayLightStatus1);
  if (relayLightStatus1 == 1) {
    digitalWrite(relaylightbulb1, LOW);
  } else {
    digitalWrite(relaylightbulb1, HIGH);
  }
}

void switch2() {
  char * json = sCmd.next();
  
  int relayLightStatus2 = (int)json[0] - 48;
  Serial.println(relayLightStatus2);
  if (relayLightStatus2 == 1) {
    digitalWrite(relaylightbulb2, LOW);
  } else {
    digitalWrite(relaylightbulb2, HIGH);
  }
}

void switch3() {
  char * json = sCmd.next();
  
  int relayLightStatus3 = (int)json[0] - 48;
  Serial.println(relayLightStatus3);
  if (relayLightStatus3 == 1) {
    digitalWrite(relaylightbulb3, LOW);
  } else {
    digitalWrite(relaylightbulb3, HIGH);
  }
}

void switch4() {
  char * json = sCmd.next();
  //  Serial.println((int)json[0] - 48);
  //StaticJsonBuffer <200> jsonBuffer ; // create a json buffer that can hold up to 200 characters
  //JsonObject & root = jsonBuffer.parseObject(json); // set a json root variable

  int relayLightStatus4 = (int)json[0] - 48;
  Serial.println(relayLightStatus4);
  if (relayLightStatus4 == 1) {
    digitalWrite(relaylightbulb4, LOW);
  } else {
    digitalWrite(relaylightbulb4, HIGH);
  }
}

void doorSwitch1() {
  char * json = sCmd.next();
  Serial.println(json);
  int relayLightStatus1 = (int)json[0] - 48;
  Serial.println(relayLightStatus1);
  if (relayLightStatus1 == 1) {
    digitalWrite(relaylightbulb1, LOW);
  } else {
    digitalWrite(relaylightbulb1, HIGH);
  }
}

// The led_red function will be executed when sending the LED_RED function
void led () {
  char * json = sCmd.next(); // Just one line to read the received parameter
  Serial.println(json);
  StaticJsonBuffer < 200 > jsonBuffer ; // create a json buffer that can hold up to 200 characters
  JsonObject & root = jsonBuffer.parseObject(json); // set a json root variable

  int redStatus = root [ "led" ] [ 0 ]; // json -> root parameter -> element # 0. Do not worry if you do not have this element, no errors!
  int blueStatus = root [ "led" ] [ 1 ]; // json -> root parameter -> element # 0. Do not worry if you do not have this element, no errors!
  // int ledStatus = root ["led"] [0];

  // test the value
  Serial.print(F("redStatus"));
  Serial.println(redStatus);
  Serial.print(F("blueStatus"));
  Serial.println(blueStatus);
  //  Serial.print(F("ledStatus"));
  // Serial.println(ledStatus);


  StaticJsonBuffer < 200 > jsonBuffer2 ;
  JsonObject & root2 = jsonBuffer2.createObject();
  root2 [ "redStatus" ] = redStatus ;
  root2 [ "blueStatus" ] = blueStatus ;
  // root2 [ "ledStatus" ] = ledStatus ;

  // Create an array in JSON
  JsonArray & data = root2.createNestedArray("data");
  data.add(redStatus);
  data.add(blueStatus);
  //data.add(ledStatus);

  // print the software serial port for the ESP8266 receiver
  mySerial.print("LED_STATUS"); // send the command name
  mySerial.print('\r'); // send \ r
  root2.printTo(mySerial); // send the JSON string
  mySerial.print('\r'); // send \ r

  // Print out Serial to debug
  //root2.printTo(Serial); //Down the line

  // output to screen
  digitalWrite(red, redStatus );
  digitalWrite(blue, blueStatus );
}

