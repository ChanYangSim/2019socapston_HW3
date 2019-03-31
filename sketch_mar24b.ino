#include <login.h>

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClientSecure.h>

OneWire ourWire(D3);
DallasTemperature sensor(&ourWire);
login login1;
const char* host = login1.gethost();
String url = "/data?";  
const int httpPort = 8080;
int seq=0;

const char* ssid = login1.getid();   // Your own ssid here
const char* password = login1.getpw();  // Your own password here
String apiKey = login1.getapi(); 

String working() { 
  sensor.requestTemperatures();
  float temp = sensor.getTempCByIndex(0);
  return("seq="+String(seq++) +"&device=100&unit=0&type=D&" + String("value=")+String(temp)) ;
}

void delivering(String payload) { 
  WiFiClient client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpPort)) {
    Serial.print("connection failed: ");
    Serial.println(payload);
    return;
  }
  /* send to database 'mydb' through my ubuntu server*/
  String getheader = "GET "+ String(url) +"&"+ String(payload) +" HTTP/1.1";
  client.println(getheader);
  client.println("User-Agent: ESP8266 ChanYang Sim");  
  client.println("Host: " + String(host));  
  client.println("Connection: close");  
  client.println();
  
  /* send to thingspeak */
  float temp = sensor.getTempCByIndex(0);
  if(client.connect("api.thingspeak.com",80))
  {
    String sendData = apiKey+"&field1="+String(temp)+"\r\n\r\n"; 
       Serial.println(sendData);
       
       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(sendData.length());
       client.print("\n\n");
       client.print(sendData);
  }
  

  Serial.println(getheader);
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  Serial.println("Done cycle.");
}

void connect_ap() {
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n Got WiFi, IP address: ");
  Serial.println(WiFi.localIP());  
}

void setup() {
  Serial.begin(115200);
  connect_ap();
}


void loop() {
     String payload = working();
     delivering(payload);
     delay(60000);

}
