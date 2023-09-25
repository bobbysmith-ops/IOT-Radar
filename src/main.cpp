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

//include additional source files
#include "index.html"//basic html web page with p5.js animation window embedded in it
#include "sketch.js"//javascript file containing p5.js animation 

// Make sure to include the ESP32 Servo library
#include <ESP32Servo.h>

//-----------------------VARIABLES---------------------------

// Defines Trig and Echo pins of the Ultrasonic Sensor
const int trigPin = 19;
const int echoPin = 18;

// Variables for the duration and the distance
long duration;
int distance;

Servo myServo; // Creates a servo object for controlling the servo motor

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

//-------------------FUNCTIONS----------------------


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

//----------- ULTRASONIC SENSOR CODE ---------------

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
//------------------------------------------------



void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  myServo.attach(13); // Defines which pin the servo motor is attached on


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


webSocket.loop();//starts the websocket loop going
webServer.handleClient();//handles the client, it calls the functions set with webServer.on, ie. it calls the route handlers


//--------------------- SERVO MOTOR CODE ----------------------

//rotates the servo motor from 15 to 165 degrees
  for(int i=15;i<=165;i++){  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
  

  iTest = String(i);
  distanceTest = String(distance);
  value = (iTest + "," + distanceTest);


  //send current degree of servo (degrees) and distance from the ultrasonic sensor (distanceTest) together through the websocket to the javascript running in html page on desktop browser
  webSocket.broadcastTXT(value);//IMPORTANT, THIS IS WHERE THE VALUE IS ACTUALLY SENT THROUGH THE WEBSOCKET

  }

  //repeats the servo motor rotation from 165 to 15 degrees
  for(int i=165;i>15;i--){  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();

  iTest2 = String(i);
  distanceTest2 = String(distance);
  value2 = (iTest2 + "," + distanceTest2);


  webSocket.broadcastTXT(value2);

  }


}
