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
const char *SSID = "Birdperson";  //ENTER YOUR WIFI SETTINGS
const char *PASSWORD = "holalucas";

const char *HOST = "hooks.slack.com";
const char *ENDPOINT = "/services/{webhook}/{hashed}/{url}";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "........  8B B2 B3";
//const char fingerprint[] PROGMEM = "....... B3 6B C8";

//=======================================================================
//                    Power on setup
//=======================================================================


void setup()
{
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(SSID, PASSWORD);     //Connect to your WiFi router
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
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================

char* createPostMessage(String jsonText){
  int content_length = strlen(jsonText);
  char content_length_str[sizeof(content_length) * 4 + 1] = (char)content_length;
  return " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "Content-Length: "+ content_length_str + "\r\n\r\n" +
               "{\"text\": " + jsonText + "\"}" + "\r\n" +
               "Connection: close\r\n\r\n"
}
void sendToSlack() {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
  Serial.println(HOST);
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(HOST, httpsPort)) && (r < 30)){
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

  Serial.print("requesting URL: ");
  Serial.println(HOST);
  char *postData = createPostMessage("This is my awesome message");
  httpsClient.print(String("POST ") + ENDPOINT + );

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

  delay(2000);  //POST Data at every 2 seconds
}
void loop() {

}
//=======================================================================

