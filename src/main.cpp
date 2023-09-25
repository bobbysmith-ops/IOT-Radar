#include <Arduino.h>

#include <Wire.h>
#include <DNSServer.h>
#include <WiFiClient.h>

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer webServer(80);//create a WebServer object, running at port 80

#include <WebSocketsServer.h>
#include <Hash.h>
//#include "Hash.h.h"

//#include "index_html.h.html"
//#include <sketch_js.h.js>
//#include "sketch_js.h.js"


#include "index.html"
#include "sketch.js"


//---------SERVO CODE---------


// Includes the Servo library
//#include <Servo.h>
#include <ESP32Servo.h>

// Defines Trig and Echo pins of the Ultrasonic Sensor
const int trigPin = 19;
const int echoPin = 18;
// Variables for the duration and the distance
long duration;
int distance;
Servo myServo; // Creates a servo object for controlling the servo motor



//--------------VARIABLES----------------

const int ledPin = 5;

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;
const long interval = 50;   

// Logins for WiFi
//const char* ssid = "WIFI_SSID"; // name of WiFi network
//const char* password = "WIFI_PWD"; // the password
const char* ssid = "Sunny";
const char* password = "6047339749604";

// create a WebSocket on port 81
//ie. create a WebSocketsServer object running on port 81
//this is a websocket SERVER, the websocket CLIENT will be in the web application javascript code
WebSocketsServer webSocket = WebSocketsServer(81);


String iTest;
String distanceTest;
String value;

String iTest2;
String distanceTest2;
String value2;

//---------------FUNCTIONS-----------------


//make my callback that runs everytime there's a websocket event
//Handles all the webSocket activities
//depending on what the type passed in is, a number of cases handled
//this stuff all gets passed to the function automatically when its triggered by webSocket.onEvent
//payload is the url
void webSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", client_num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(client_num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", client_num, ip[0], ip[1], ip[2], ip[3], payload);
        
                // send message to client
                webSocket.sendTXT(client_num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] %s\n", client_num, payload);

            // send message to client
            // webSocket.sendTXT(client_num, "Received");

            // using strncmp to compare payload (an byte array) to a string
            if (!strncmp((char *)payload, "on", length)) {
              digitalWrite(ledPin, HIGH);
            } else {
              digitalWrite(ledPin, LOW);
            }
            
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            webSocket.broadcastTXT("message here");
            
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n", client_num, length);
            // hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(client_num, payload, length);
            break;
        case WStype_ERROR:{}
        case WStype_FRAGMENT_TEXT_START:{}
        case WStype_FRAGMENT_BIN_START:{}
        case WStype_FRAGMENT:{}
        case WStype_FRAGMENT_FIN:{}
        case WStype_PING:{}
        case WStype_PONG:{}
          break;
    }

}



//create my route handler callback functions, we will connect them to the routes down below in webServer_setup()
//arg1: http code 200 means the request was succesful
//arg2: text/html is the content type
//arg3: INDEX_HTML is the response content, note this is type String& aka a reference to a string, aka it points to an address in memory, recall the entire index html file was saved in program memory as a string called INDEX_HTML
void handleRoot() {
  webServer.send_P(200, "text/html", INDEX_HTML); // serves the index.html file to the client (ie. it sends a RESPONSE to the client that has sent a request)
}

void handleSketch() {
  webServer.send_P(200, "text/javascript", SKETCH_JS); // serve the sketch_js.h file - send_P send the second and third values as PGM_P types (see WebServer.h at line 123: https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h)
}
//send_P is a send method used for sending PGM_P stuff
//PGM_P is a datatype that is a pointer to a string in program space.



void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i=0; i<webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}


// sets up webserver and websocket server, call this in the setup function
//WebServer.on stuff is connecting routes to route handlers that ive made
void webServer_setup() {
  webServer.on("/", handleRoot);//register handleRoot (could be any name) as the callback function that will be invoked when client request "/" aka root folder.
  webServer.on("/sketch.js", handleSketch);

  webServer.onNotFound(handleNotFound);

  webServer.begin();//start the HTTP webserver
  Serial.println("HTTP server started");

  webSocket.begin();//start the websocket server
  webSocket.onEvent(webSocketEvent);//connect the event handler callback i made to the onEvent signal of webSocket server
  Serial.println("WebSocket server started");
}

//-----------ULTRASONIC STUFF-------

int calculateDistance(){ 
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
}
//-------------------------------



void setup() {

  //Serial.begin(115200);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  //SERVO/ULTRASONIC STUFF---------

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
  myServo.attach(13); // Defines on which pin is the servo motor attached

  //--------------

  // Connect to wifi
  WiFi.begin(ssid, password);
  
  Serial.println("");
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webServer_setup(); 
}



void loop() {

if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  //printf("hiii");

  webSocket.loop();//starts the websocket loop going
  webServer.handleClient();//handles the client, it calls the functions set with webServer.on, ie. it calls the route handlers



//--------------SERVO STUFF----------

//rotates the servo motor from 15 to 165 degrees
  for(int i=15;i<=165;i++){  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
  
  //Serial.print(" degreees: ");//remove
  //Serial.print(i); // Sends the current degree into the Serial Port
  //Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  //Serial.print(" distanceee: ");
  //Serial.print(distance); // Sends the distance value into the Serial Port
  //Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing

  // String iTest = String(i);
  // String distanceTest = String(distance);
  // String value = (iTest + "," + distanceTest);

  iTest = String(i);
  distanceTest = String(distance);
  value = (iTest + "," + distanceTest);





  //Serial.print(" VALUE: ");
  //Serial.print(value);
  
  //webSocket.sendTXT
  webSocket.broadcastTXT(value);

  }

  //Repeats the previous lines from 165 to 15 degrees
  for(int i=165;i>15;i--){  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();
  //Serial.print(i);
  //Serial.print(",");
  //Serial.print(distance);
  //Serial.print(".");

  // String iTest2 = String(i);
  // String distanceTest2 = String(distance);
  // String value2 = (iTest2 + "," + distanceTest2);

  iTest2 = String(i);
  distanceTest2 = String(distance);
  value2 = (iTest2 + "," + distanceTest2);


  webSocket.broadcastTXT(value2);

  }


//------------------------------------

  //String value = "55";

  //arduino values 
  // int a = 55.5;
  // int b = 200;

  // String atest = String(a);
  // String btest = String(b);
  // String value = (atest + "," + btest);

  
  // webSocket.broadcastTXT(value);//IMPORTANT, THIS IS WHERE THE VALUE IS ACTUALLY SENT THROUGH THE WEBSOCKET


  //Serial.println(value);


  //unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis >= interval) {
    
  //   // save the last time you blinked the LED
  //   previousMillis = currentMillis;

  //   //String value = String(analogRead(ldrPin));
  //   String value = "hi";
  //   // Serial.println(value);

  //   webSocket.broadcastTXT(value);
  // }

}
