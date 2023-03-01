#include <Arduino.h>
#include "DHT.h"
#include <MQUnifiedsensor.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <TridentTD_LineNotify.h>

#define DHTPin D7
#define RelayPin D0

#define         Board                   ("Arduino UNO")
#define         Pin                     (A0)  //Analog input 3 of your arduino
#define         Type                    ("MQ-2") //MQ2
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ2CleanAir        (9.83) //RS / R0 = 9.83 ppm 


#define BLYNK_TEMPLATE_ID "TMPLRxVqwoOV"
#define BLYNK_TEMPLATE_NAME "kasukabe"
#define BLYNK_AUTH_TOKEN "lq0krE4FHp2xOnSFw23MX7T0RDj7PVIP"

#define LINE_TOKEN "OZCd13CEHx6cBgrWvUVtnz2THL5ZoDx4J9nIqvtu1aK"

char ssid[] = "lawiet";
char pass[] = "kvnn6518";

MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);
DHT dht;

void setup() {
  Serial.begin(9600);

  LINE.setToken(LINE_TOKEN);

  pinMode(D3, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  
  dht.setup(DHTPin);

  MQ2.setRegressionMethod(1);
  MQ2.setA(574.25); 
  MQ2.setB(-2.222); 
  MQ2.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ2.update(); 
    calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
    Serial.print(".");
  }
  MQ2.setR0(calcR0/10);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void display() {
    MQ2.update();
    byte temp = dht.getTemperature();
    byte LPG = MQ2.readSensor();
    Blynk.virtualWrite(V4, temp);
    Blynk.virtualWrite(V5, LPG);
    int pureLPG = MQ2.readSensor();
    float pureTemp = dht.getTemperature();
    if (pureLPG >= 100){
      LINE.notifySticker("LPG gas leaks > 100 ppm", 6632, 11825375);
      delay(10000);
    }
    if (pureTemp >= 25){
      digitalWrite(RelayPin, LOW);
    } else {
      digitalWrite(RelayPin, HIGH);
    }
}

BLYNK_WRITE(V0) {
  int state = param.asInt();
  digitalWrite(D3, state);
}

BLYNK_WRITE(V1) {
  int state = param.asInt();
  digitalWrite(D5, state);
}

BLYNK_WRITE(V2) {
  int state = param.asInt();
  digitalWrite(D6, state);
}

BLYNK_WRITE(V3) {
  int state = param.asInt();
  digitalWrite(D8, state);
}

void loop() {
  Blynk.run();
  delay(500);
  display();
}