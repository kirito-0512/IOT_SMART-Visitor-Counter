#include <Ultrasonic.h>
#include<HTTPClient.h>
#include <WiFi.h>
#include "time.h"
#include <WiFi.h>
#include "ThingSpeak.h"
#define CSE_IP      "192.168.216.108"
#define CSE_PORT    5089
#define HTTPS     false
#define OM2M_ORGIN    "admin:admin"
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "Hrudai_Aditya"
#define OM2M_DATA_CONT  "IOT_PROJECT/Data_Tuple"
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 
long randNumber;
time_t rawtime;
struct tm * timeinfo;
struct tm  ts;
char   buf[80];
time_t     now;
HTTPClient http;
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now); 
  return now;
}
char* ssid = "Plus"; 
char* passphrase = "12345678"; 
WiFiServer server(80);
WiFiClient client;
unsigned long myChannelNumber =  1969665;
const char * myWriteAPIKey = "FRV647AQG34VZJZB";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
Ultrasonic ultrasonic1(5, 18);
Ultrasonic ultrasonic2(12,14);	
int number =0;
void setup()
{
Serial.begin(115200); 
Serial.print("Connecting to ");
Serial.println(ssid);
configTime(1, 0, ntpServer);
WiFi.begin(ssid, passphrase);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print("wait");
}
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
server.begin();
ThingSpeak.begin(client); 
}
void loop() {
  epochTime = getTime();
  static int i=0;
  if ((millis() - lastTime) > timerDelay)
{
delay(1000);
  if(ultrasonic1.read(CM)<40){
    Serial.print("\nIN\n");
  number = number+1;
Serial.print(number);
  }
  if(ultrasonic2.read(CM)<40){
    Serial.print("\nOUT\n");
  number = number-1;
  Serial.print(number);    
  } 
  delay(500);
  ThingSpeak.setField(1, number);
  int x = ThingSpeak.writeFields(myChannelNumber,myWriteAPIKey);
  time(&now);
  ts = *localtime(&now);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

if(x == 200){
Serial.println("Channel update successful.");
}
String data="[" + String(buf) + ", " + String(number) + " , " + String(56)+"]";
String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
Serial.println(data);
http.begin(server + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/");
http.addHeader("X-M2M-Origin", OM2M_ORGIN);
http.addHeader("Content-Type", "application/json;ty=4");
http.addHeader("Content-Length", "100");
String label = "Node-1";
String req_data = String() + "{\"m2m:cin\": {"

  + "\"con\": \"" + data + "\","

  + "\"rn\": \"" + "cin_"+String(i++) + "\","

  + "\"lbl\": \"" + label + "\","

  + "\"cnf\": \"text\""

  + "}}";
int code = http.POST(req_data);
http.end();
Serial.println(code);
lastTime = millis();
}
}
