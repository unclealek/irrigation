#include <I2CSoilMoistureSensor.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include "arduino_secrets.h"

#include <SoftwareSerial.h>
SoftwareSerial espSerial(2, 3); // RX (ESP) to 2, TX (ESP) to 3

//#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Define your Wi-Fi credentials
const char* ssid = "koti_Dm4h";
const char* password = " ";

// Define ThingSpeak parameters
const char* thingSpeakAddress = "api.thingspeak.com";
const String apiKey = " "; // Replace with your ThingSpeak API Key
const long channelID = ; // Replace with your ThingSpeak Channel ID

// ...

// Define moisture sensor pins
#define SOIL_SENSOR_1 A0 // Use the same analog pin A0

// Define Relay Pin & pump
#define RELAY_PIN 4 // Change to GPIO pin 4 on ESP8266
bool pumpStatus = false;
int pump_trigger = 540;

// Define DHT22 Parameters
#define DHTPIN 5 // Change to GPIO pin 5 on ESP8266
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int DRY_THRESHOLD = 400;   // Replace with your desired dry threshold value
const int WET_THRESHOLD = 700;  // Replace with your desired wet threshold value

int soilMoisturePer;
int soilMoisturePercent;

//#include "thingProperties.h"

void setup() 
{
    Serial.begin(9600); // open serial port, set the baud rate to 9600 bps 
    Serial.println("Reading from the DHT22.....");
    delay(2000);
    dht.begin(); // Use dht.begin() instead of DHT.begin()
    delay(2000);  
    Serial.begin(9600); // Initialize hardware serial for debugging
    espSerial.begin(9600); // Initialize software serial for ESP-01S
    pinMode(SOIL_SENSOR_1, INPUT); 
    Serial.println("Reading from the sensor 1.....");
    delay(2000);

    // Set Relay as Output
    pinMode(RELAY_PIN, OUTPUT);
    
     // Initialize Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize ThingSpeak
    ThingSpeak.begin(client);
}

void loop() 
{ 
  // call Arduino IoT cloud for updates
  ArduinoCloud.update();

    // ...

    // Update ThingSpeak fields
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, hum);
    ThingSpeak.setField(3, soilMoisturePercent);

    // Write data to ThingSpeak
    int writeSuccess = ThingSpeak.writeFields(channelID, apiKey);

    if (writeSuccess) {
        Serial.println("Data sent to ThingSpeak successfully!");
    } else {
        Serial.println("Failed to send data to ThingSpeak...");
    }
  float temp;
  float hum;

  temp = dht.readTemperature();
  hum = dht.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C, Humidity: ");
  Serial.print(hum);
  Serial.println("% ");
  delay(3000);

// cloud varaibles 

current_Temp = temp;
current_Humidity =hum;

  // water pump
  digitalWrite(RELAY_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(RELAY_PIN, LOW);

  // get soil moisture value
  int soilMoistureValue1 = analogRead(SOIL_SENSOR_1);
 
  // Calculate average soil moisture value
  int Total_moisture = soilMoistureValue1;
  int Moisture = Total_moisture;
  Serial.print("Soil Moisture: ");
  Serial.println(Moisture);
  delay(3000);

  // Convert soil moisture value to percentage
  soilMoisturePer = map(Moisture, DRY_THRESHOLD, WET_THRESHOLD, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePer, 0, 100);
//cloud
current_moisture = soilMoisturePercent;
  // Print soil moisture percentage
  Serial.print(soilMoisturePercent);
  Serial.print("%");
  delay(3000);

  // See if pump should be triggered
  // See if moisture is below or equal to threshold
  if (Moisture >= pump_trigger) {
    // Turn pump on
    pumpOn();
  } else {
    // Turn pump off
    pumpOff();
  }
}

void pumpOn() {
  // Turn pump on
  digitalWrite(RELAY_PIN, HIGH);
  pumpStatus = true;
  delay(2000);
  if (pumpStatus == true){
    Serial.println("-Pump On");
  }
}

void pumpOff() {
  // Turn pump off
  digitalWrite(RELAY_PIN, LOW);
  pumpStatus = false;
  delay(2000);
  if (pumpStatus == false){
    Serial.println("-Pump Off");
  }  
}
