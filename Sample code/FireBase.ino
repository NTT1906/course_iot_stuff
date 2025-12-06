#include <WiFi.h>
#include <FirebaseESP32.h>

// Wi-Fi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Firebase credentials
#define FIREBASE_HOST "YourHost"
#define FIREBASE_AUTH "YourKey"

// Firebase objects
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to WiFi");
  wifiConnect();

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Configure Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, "Nodes/led")) {
      Serial.println("Could not begin stream");
      Serial.println("REASON: " + firebaseData.errorReason());
  } else {
      Serial.println("Stream started successfully");
  }
}

void loop() {
  static int counter = 0;

  for (int i=0; i<10; i++)
  {
    Firebase.setInt(firebaseData, "/Nodes/led", i);

    if (Firebase.getInt(firebaseData, "/Nodes/led"))
    {
        if (firebaseData.dataType() == "int")
        {
          Serial.println(firebaseData.intData());
        }
    }

    delay(1000);
  }
  delay(1000);
}
