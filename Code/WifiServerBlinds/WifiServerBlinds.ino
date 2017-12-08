/* Create a WiFi access point and provide a web server on it. */
// For more details see http://42bots.com.

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <AccelStepper.h>



// Motor pin definitions
#define motorPin1  5     // IN1 on the ULN2003 driver 1
#define motorPin2  4     // IN2 on the ULN2003 driver 1
#define motorPin3  0     // IN3 on the ULN2003 driver 1
#define motorPin4  2     // IN4 on the ULN2003 driver 1
#define HALFSTEP 8

IPAddress    apIP(42, 42, 42, 42);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1
                                    
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "TheDankBlinds";
const char *password = "21plkoyr92";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);


char htmlPage[1000];
int currTarget;

void handleRoot() {
  if(server.args()) {
    currTarget = server.arg("pos").toInt();
    stepper1.moveTo(currTarget);
    Serial.print("input = ");
    Serial.println(currTarget);
  }

  char html[1000];
  
  
// Build an HTML page to display on the web-server root address
  snprintf ( htmlPage, 1000,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='10'/>\
        <title>ESP8266 WiFi Network</title>\
        <style>\
          body { background-color: #FF8000; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
          h1 { Color: #AA0000; }\
        </style>\
      </head>\
      <body>\
        <h1>The Dank Wifi-Enabled Blinds</h1>\
        <p>Current Target: %d</p>\
        <p>Return to position</p>\
        <p>\
          <a href='b1'>\
            <button>Button 1</button>\
          </a>\
          <a href='b2'>\
            <button>Button 2</button>\
          </a>\
        </p>\ 
      </body>\
    </html>",
    stepper1.targetPosition());
  server.send ( 200, "text/html", htmlPage);
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

void motorSetup() {
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(1000);
  
}

void wifiSetup() {
  
  delay(1000);
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
  server.on ( "/pos=", handleRoot);


//Handle button presses
  server.on("/b1", [](){
    stepperRun(2000);
    server.send(200, "text/html", htmlPage);
    Serial.println("Button on");
    Serial.print("Target Postition: ");
    Serial.println(stepper1.targetPosition());
  });
  server.on("/b2", [](){
    stepperRun(-2000);
    server.send(200, "text/html", htmlPage);
    Serial.println("Button off");
    Serial.print("Target Postition: ");
    Serial.println(stepper1.targetPosition());
  });
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("HTTP server started");
}

void stepperRun(int pos){
  stepper1.enableOutputs();
  stepper1.moveTo(pos);
}

void setup() {
  Serial.println("setup start...");
  wifiSetup();
  Serial.println("wifi setup complete");
  motorSetup();
  Serial.println("motor setup complete");
}
void loop() {
  stepper1.run();
  if(stepper1.targetPosition()==stepper1.currentPosition()) {
    stepper1.disableOutputs();
  }
  server.handleClient();
}
