#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>

// include the library to test free RAM on esp8266
extern "C" {
#include "user_interface.h"
}


const byte RX = D1 ;
const byte TX = D2 ;

SoftwareSerial mySerial = SoftwareSerial ( RX , TX , false , 256 );
SerialCommand sCmd ( mySerial ); // Declare variables using the Serial Command library

SocketIOClient client ;
const char * ssid = "mocmeo" ;
const char * password = "12345678" ;

char host [] = "192.168.0.111" ;
int port = 3000 ;
unsigned long previousMillis = 0;
long interval = 4000;

//extern keyword: used to #include global variables from other libraries. The SocketIOClient library has 2 global variables
// that we need to worry about
// RID: function name (event's name)
// Rfull: List of variables (being packaged as a JSON string)
extern String RID;
extern String Rfull;


void setup()
{
  Serial.begin(57600);
  mySerial.begin(57600);
  delay(10);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  //Waiting until its connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.println();
  Serial.println(F("Wifi connected"));
  Serial.println(F("ESP8266 Wifi Address (Socket Client ESP8266): "));
  Serial.println(WiFi.localIP());

  if (client.connect(host, port)) {
    Serial.printf("[SocIO] Connected to url: %s\n",  host);
    client.send("connection", "message", "1st ESP8266 connected!" + WiFi.localIP().toString());
  } else {
    Serial.println("Connection failed.");
  }
  sCmd.addDefaultHandler(defaultCommand);
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    client.heartbeat(0);
  }
  //When catching any event, we have 2 parameters:
  //  +RID: Event's name
  //  +RFull: List of parameters which are being put in JSON string!
  if (client.monitor()) {

    //print debug for Arduino
    mySerial.print(RID);
    mySerial.print('\r');
    mySerial.print(Rfull);
    mySerial.print('\r');

    //Print out to Serial monitor
    Serial.print(RID);
    Serial.print(' ');
    Serial.println(Rfull);

    //Check how much RAM you still have left, for debugging
    //uint32_t free = system_get_free_heap_size();
    //Serial.println(free);
  }
  //Serial.println(client.connected());

  //Reconnecting
  if (!client.connected()) {
    client.reconnect(host, port);
  }
  sCmd.readSerial();
}

void defaultCommand(String command) {
  char * json = sCmd.next();
  client.send(command, (String)json); // send data to the Socket Server

  // Print the serial monitor to debug
  Serial.print(command);
  Serial.print(' ');
  Serial.println( json );
  Serial.println("In order to do this, see the console for the Socket server" );
}
