/*
 * HTTPS Secured Client POST Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
const char *ssid = "ssid";  //ENTER YOUR WIFI SETTINGS
const char *password = "password";


//Web/Server address to read/write from 
const char *host = "XXXXXXXXXXX-api.us-east-1.amazonaws.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

const String Link = "/dev/message";

//SHA1 finger print of certificate use web browser to view and copy
//const char fingerprint[] PROGMEM = ".... 0D 3D FD 8B B2 B3";
const char fingerprint[] PROGMEM = "...... 13 A5 7F D6 C9 CD";
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  pinMode(14, INPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void push_to_slack(WiFiClientSecure* httpsClient, char* msg){
/*
   POST /post HTTP/1.1
   Host: postman-echo.com
   Content-Type: application/x-www-form-urlencoded
   Content-Length: 
*/
  String msgString(msg);
  String payload = "{\"service\":\"slack\", \"channel\":\"general\", \"username\":\"Shhlack\", \"text\":\"" + msgString + "\"}";
  String msgLength = (String) payload.length();
  String shitToSend = String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "X-Api-Key: whwNWYPTwl6Y4d8UCt5Oq5gvpn1LnW4w2XgO7JAR" + "\r\n" +
               "Content-Length: "+ msgLength + "\r\n\r\n" + payload + "\r\n" +
               "Connection: close\r\n\r\n";
  Serial.println("Trying to send: " + shitToSend);
  httpsClient->print(shitToSend);  
}

void loop() {
  int val = digitalRead(14);
  if(val == 0){
    
    WiFiClientSecure httpsClient; //Declare object of class WiFiClient
    
    Serial.println(host);
    
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
    }
    if(r==30) {
      Serial.println("Connection failed");
    }
    else {
      Serial.println("Connected to web");
    }
    
    String getData;
    
    Serial.print("requesting URL: ");
    Serial.println(host);
    
    
    push_to_slack(&httpsClient, "There is a lot of noise in the office, please make silence!");
     
    Serial.println("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    Serial.println("==========");
    Serial.println("closing connection");
  }
  delay(1000);  //POST Data at every 2 seconds
}
//=======================================================================
