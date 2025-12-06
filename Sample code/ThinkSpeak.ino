#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected");
}

const unsigned long channelId = yourChanelID; 
const char* writeAPIKey = "YourKey";
const char* readAPIKey = "YourKey";

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Connecting to WiFi");

  wifiConnect();

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);

}

void loop() {
  int temp_value = random(100);
  int ret = ThingSpeak.writeField(channelId, 1, temp_value, writeAPIKey);
  if(ret == 200) {
    Serial.println("Successful");
  }
  else {
    Serial.println("Error");
  }
  delay(5000); // this speeds up the simulation

  int temp = ThingSpeak.readIntField(channelId, 1, readAPIKey);
  Serial.println(temp);
  delay(5000);
}
