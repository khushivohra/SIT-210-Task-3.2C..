//These lines include necessary libraries for working with WiFi and HTTP client.
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <hp_BH1750.h>
hp_BH1750 sensor;

//defining a variable for a LED pin.
int LED = 2;

//adding ssID and password
char ssid[] = "iPhone";
char pass[] = "qazxcvbnm123";

// defining constants for the IFTTT service
const char* IFTTT_HOST = "maker.ifttt.com";// IFTTT hostname
const int IFTTT_PORT = 80;// HTTP port for IFTTT
const char* IFTTT_EVENT = "Sunlight_detected"; // Event name on IFTTT
 const char* IFTTT_KEY = "cIwj2kLeJMoYdVWejIU-2d7OtImQa76fklTWyHarOH";//IFTTT webhook key

// defining analog pins connected to BH1750
const int SCLpin = A5;
const int SDApin = A4;

// Light level threshold
const int threshold = 500;

//here i'm creating a Wi-Fi client and HTTP client
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, IFTTT_HOST, IFTTT_PORT);

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  pinMode(LED, OUTPUT);// seting the LED pin as an output
  sensor.begin(BH1750_TO_GROUND); // initialising BH1750
  sensor.start();

  // Connect to Wi-Fi
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Connecting to WiFi. Please wait for a while");
    delay(1000);
  }
  Serial.println("Connected to WiFi :)");
}

void loop() {
  onLightSensorChange();
  delay(5000);
}

// this function handles the actions when the sunlight is detected
void onSunlightChange(){
  digitalWrite(LED, HIGH);
  delay(2000);

  digitalWrite(LED, LOW);
  delay(2000);
}

// this function sends data to IFTTT
void sendToIFTTT(String value) {
  // here i'm defining url and json data for the HTTP post request
  String url = "/trigger/light sensor/with/key/cIwj2kLeJMoYdVWejIU-2d7OtImQa76fklTWyHarOH"; 
  String json = "{\"value1\":\"" + value + "\"}";

  client.beginRequest();
  client.post(url);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

// getting the response from IFTTT
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  Serial.print("IFTTT Response Code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

//function that helps in checking the light sensor and then accordingly the actions are taken 
void onLightSensorChange(){
  if (sensor.hasValue())
  {
    float lightSensor = sensor.getLux();
    bool sunlight;
    if (lightSensor > 400)
    {
      // when the sunlight is detected, call the onSunlightChange function and it sends data to IFTTT.
      sunlight = true;
      onSunlightChange();
      sendToIFTTT("SunlightDetected");
      
      // printing the light intensity 
      Serial.print("Light Intensity: ");
      Serial.print(lightSensor);
      Serial.println(", Sunlight: ON");
    }
    else
    {
      // when no sunlight is detected, turn off the LED and print the light intensity
      sunlight = false;
      digitalWrite(LED, LOW);
      Serial.print("Light Intensity: ");
      Serial.print(lightSensor);
      Serial.println(", Sunlight: OFF");
    }

    delay(5000);
    sensor.start();
}
}
