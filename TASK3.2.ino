//These lines include necessary libraries for working with WiFi and HTTP client.
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h> // hhtp request leta hai aur aage client ko send krta hai
#include <hp_BH1750.h>
hp_BH1750 sensor;

//adding ssID and password
char ssid[] = "iPhone";
char pass[] = "qazxcvbnm123";

// defining constants for the IFTTT service
const char* IFTTT_HOST = "maker.ifttt.com";// IFTTT hostname
const int IFTTT_PORT = 80;// HTTP port for IFTTT
const char* IFTTT_EVENT = "Sunlight_detected"; // Event name on IFTTT
 const char* IFTTT_KEY = "cIwj2kLeJMoYdVWejIU-2d7OtImQa76fklTWyHarOH";//IFTTT webhook key

// Light level threshold
const int threshold = 500;

//here i'm creating a Wi-Fi client and HTTP client
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, IFTTT_HOST, IFTTT_PORT);

void setup() {
  Serial.begin(9600); // shows serial monitor
  delay(2000);

  sensor.begin(BH1750_TO_GROUND); // initialising BH1750 , 

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



// this function sends data to IFTTT
void sendToIFTTT(String value) {
  // here i'm defining url and json data for the HTTP post request
  String url = "/trigger/light sensor/with/key/cIwj2kLeJMoYdVWejIU-2d7OtImQa76fklTWyHarOH"; 
  String json = "{\"value1\":\"" + value + "\"}";

  client.beginRequest(); // This function initiates the HTTP request using the client object
  client.post(url); // The client will send the request to this URL.
  client.sendHeader("Content-Type", "application/json"); // indicates that the request body will be in JSON format by setting the "Content-Type" header to "application/json."
  client.sendHeader("Content-Length", json.length()); // informs the server of the length of the data being sent in the request.
  client.beginBody(); // starts HTTP request body.
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
    if (lightSensor > 500)
    {
      // when the sunlight is detected, call the onSunlightChange function and it sends data to IFTTT.
     
      sendToIFTTT("SunlightDetected");
      
      // printing the light intensity 
      Serial.print("Light Intensity: ");
      Serial.print(lightSensor);
      Serial.println(", Sunlight: ON");
    }
    else
    {
      // when no sunlight is detected, print the light intensity
      sunlight = false;
      Serial.print("Light Intensity: ");
      Serial.print(lightSensor);
      Serial.println(", Sunlight: OFF");
    }

    delay(5000);
    
}
}
