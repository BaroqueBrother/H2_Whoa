/*
 * Project H2Whoa
 * Description:
 * Author:
 * Date:
 */
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "credentials.h"

#include "Air_Quality_Sensor.h"


TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);
Adafruit_MQTT_Publish bmeTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Ambient Temperature");
Adafruit_MQTT_Publish bmeHumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish bmeAirPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Air Pressure");
Adafruit_MQTT_Publish soilSaturation = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Soil Saturation");
Adafruit_MQTT_Publish dustParticulates = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Dust Particulates");
Adafruit_MQTT_Publish airQuality = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Air Quality");
Adafruit_MQTT_Subscribe waterMe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/H20");


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BME280 bme;

bool status;
uint currentTime;
int onOrOff,ambientTemp;
const int relayPin = A5;
const int soilPin = A4;
const int dustPin = A3;
float soilReading;
float temperature;
float humidRH;
float pressPA;
float pressInhg;
uint last;
float inhg = .000295;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
AirQualitySensor sensor = (A2);
int quanAirValue;




void setup() {
    Serial.begin(9600);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)){
      for(;;);
    }
    WiFi.connect();
    while(WiFi.connecting()){
      Serial.printf(".");
    }
    status = bme.begin(0x76);
    if (sensor.init()) {
      Serial.println("Sensor ready.");
  }
    else {
      Serial.println("Sensor ERROR!");
  }
    display.display();
    mqtt.subscribe(&waterMe);
    pinMode(relayPin,OUTPUT);
    pinMode(soilPin,INPUT);
    pinMode(dustPin, INPUT);
}

void loop() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  MQTT_connect();
  if ((millis()-last)>120000) {
    Serial.printf("Pinging MQTT \n");
    if(! mqtt.ping()) {
      Serial.printf("Disconnecting \n");
      mqtt.disconnect();
      }
      last = millis();
  }
  currentTime = millis();
    if((currentTime - last) > 1800000) {
        readSoil();
        if(mqtt.Update()){
            soilSaturation.publish(soilReading);
        }
        last = millis();
    }
    currentTime = millis();
    if((currentTime - last) > 30000) {
        readTempF();
        readHumidity();
        readPressureInhg();
        readDust();
        readAirQuality();
        if(mqtt.Update()){
            bmeTemp.publish(temperature);
            bmeHumidity.publish(humidRH);
            bmeAirPressure.publish(pressInhg);
            dustParticulates.publish(concentration);
            airQuality.publish(quanAirValue);
        }
        last = millis();
    }

    Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &waterMe) {
      onOrOff = atoi((char *)waterMe.lastread);
      if((currentTime - last) > 500) {
        digitalWrite(relayPin, HIGH);
  }
      else {
        digitalWrite(relayPin, LOW);
  }
      last = millis();
          Serial.printf("Received %i from Adafruit.io feed H20 \n",onOrOff);
          
    }
  }
  if(onOrOff){
      digitalWrite(relayPin,HIGH);
    }
    else{
    digitalWrite(relayPin,LOW);
    }
}
float readTempF(){ // print temp every 60 seconds 
    float tempC;
    tempC = bme.readTemperature();
    temperature = map(tempC, 0.0, 100.0, 32.0, 212.0);
    display.clearDisplay();
    display.printf("Temperature %.01f \n",temperature);
    Serial.printf("Temperature %.01f \n",temperature);
    display.display();
    return(temperature);
}
float readHumidity() {
  humidRH = bme.readHumidity();
  display.clearDisplay();
  display.printf("Humidity %.01f \n",humidRH);
  Serial.printf("Humidity %.01f \n",humidRH);
  display.display();
  return(humidRH);
}
float readPressureInhg() {
  pressPA = bme.readPressure();
  pressInhg = pressPA * inhg;
  display.clearDisplay();
  display.printf("Air Pressure %.01f \n",pressInhg);
  Serial.printf("Air Pressure %.01f \n",pressInhg);
  display.display();
  return (pressInhg);
}
float readSoil(){
  soilReading = analogRead(soilPin);
  if(soilReading >= 3000.0){
    if((currentTime - last) > 500) {
    digitalWrite(relayPin, HIGH);
  }
  else {
    digitalWrite(relayPin, LOW);
  }
  last = millis();
  }
  Serial.printf("Soil Saturation %.01f \n", soilReading);
  display.clearDisplay();
  display.printf("Soil Saturation %.01f \n",soilReading);
  display.display();
  return (pressInhg);

}
float readDust(){
  duration = pulseIn(dustPin,LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
    ratio = lowpulseoccupancy/(sampletime*10.0);
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
    Serial.printf("Dust Concentration %.01f \n",concentration);
    lowpulseoccupancy = 0;
    starttime = millis();
  return(concentration);
}
int readAirQuality(){
  int quanAirValue = sensor.getValue();
  int quality = sensor.slope();
  Serial.printf("Air Quality %i \n", quanAirValue);
  
  if (quality == AirQualitySensor::FORCE_SIGNAL) {
    Serial.printf("High pollution! Force signal active \n");
  }
  else if (quality == AirQualitySensor::HIGH_POLLUTION) {
    Serial.printf("High pollution \n");
  }
  else if (quality == AirQualitySensor::LOW_POLLUTION) {
    Serial.printf("Low pollution \n");
  }
  else if (quality == AirQualitySensor::FRESH_AIR) {
    Serial.printf("Fresh air \n");
  }
return (quanAirValue);

}

void MQTT_connect() {
  int8_t ret;
 
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("%s\n",(char *)mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds..\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.printf("MQTT Connected!\n");
}