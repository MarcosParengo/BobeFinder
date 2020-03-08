#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>//incluimos SoftwareSerial
#include <TinyGPS.h>//incluimos TinyGPS
#include <WifiLocation.h>
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>
#include <Arduino.h>

static char ssid[] = "Fibertel WiFi306 2.4GHz";
static char pass[] = "0142230299";

const char* googleApiKey = "AIzaSyC7tZT_kV-1qWh3hFQuz-9vzklUleLK4fA";
WifiLocation location(googleApiKey);

const static char pubkey[]  = "pub-c-f623deb2-63eb-4608-acf4-af48ed3e31cc";
const static char subkey[]  = "sub-c-5251cb74-f12b-11e9-ad72-8e6732c0d56b";
const static char channel[] = "whiteboard";

float lat;
float lon;
char contconexion = 0;

TinyGPS gps;//Declaramos el objeto gps
SoftwareSerial serialgps(5, 16); //Declaramos el pin 4 Tx y 3 Rx

//Declaramos la variables para la obtención de datos
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;

void setup() {

  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) { // Connect to WiFi.
    Serial.println("Couldn't connect to WiFi.");
    while (1) delay(100);
  }
  else {
    Serial.print("Connected to SSID: ");
    Serial.println(ssid);
    PubNub.begin(pubkey, subkey); // Start PubNub.
    Serial.println("PubNub is set up.");
  }
}
void loop() {
  Serial.println(" ---GPS Buscando señal--- ");
  while (!serialgps.available() and contconexion < 5000)
  {
    serialgps.begin(9600);//Iniciamos el puerto serie del gps
    ++contconexion;
    Serial.print(".");
    delay(100);
  }
  while (serialgps.available())//Si el gps tiene señal
  {
    int c = serialgps.read();

    if (gps.encode(c))
    {
      gps.f_get_position(&lat, &lon);
      Serial.print("Latitud/Longitud: ");
      Serial.print(lat, 6);
      Serial.print(", ");
      Serial.println(lon, 6);
      publish();
      delay(5000);
      Serial.println("Awake");
    }
  }
  if (!serialgps.available() and contconexion == 50)
  { 
    Serial.println("");
    Serial.println(" ---El GPS No ha podido establecer una conexion--- ");
    Serial.println("");
    location_t loc = location.getGeoFromWiFi();
    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));
    lat = l
    lon = loc.lon;
    publish();
  }
}


void publish() {
  {oc.lat;
    // Subscribe.
    Serial.println("Before subscribe.");
    PubSubClient* sclient = PubNub.subscribe(channel); // Subscribe.
    Serial.println("After subscribe.");
    if (0 == sclient) {
      Serial.println("Error subscribing to channel.");
      delay(1000);
      return;
    }
    while (sclient->wait_for_data()) { // Print messages.
      Serial.write(sclient->read());
    }
    sclient->stop();
  }
  { // Publish.

    char result[30];// = "\"";
    sprintf(result, "\"%0.6f,%0.6f\"", lat, lon);
    Serial.println("Result:");
    Serial.print(result);
    Serial.println("");
    
    WiFiClient* client = PubNub.publish(channel, result); // Publish message.

    contconexion = 0;
    if (0 == client) {
      Serial.println("Error publishing message.");
      delay(1000);
      return;
    }
    client->stop();
  }
}
