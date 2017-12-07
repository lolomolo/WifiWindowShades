/* Create a WiFi access point and provide a web server on it. */
// For more details see http://42bots.com.

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <AccelStepper.h>

#define HALFSTEP 8

#define motorPin1  5     // IN1 on the ULN2003 driver 1
#define motorPin2  4     // IN2 on the ULN2003 driver 1
#define motorPin3  0     // IN3 on the ULN2003 driver 1
#define motorPin4  2     // IN4 on the ULN2003 driver 1

IPAddress    apIP(42, 42, 42, 42);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1
                                    
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);                                    

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "TheDankTeaKettle";
const char *password = "derp";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);


void handleRoot() {
  int input =server.arg("pos").toInt();
  Serial.println(input);
  stepper1.moveTo(input); //0 = vertical, +/-2048 =clockwise/counterclockwise 180 degrees 
  char html[1000];
  server.send ( 200, "text/html", html );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

void wifiSetup() {
  analogWriteFreq(200);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void stepperSetup() {
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(200);
}

void setup(){
  wifiSetup();
  stepperSetup();
}
void loop() {
  stepper1.run();
  server.handleClient();
}
