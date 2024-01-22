#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <DS3231.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define DHTPIN 3
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT_Unified dht(DHTPIN, DHTTYPE);

const int streetLights = 4;

const int trafficLightsOne_RED = 12;
const int trafficLightsOne_YLW = 13;
const int trafficLightsOne_GRN = 11;

const int trafficLightsTwo_RED = 5;
const int trafficLightsTwo_YLW = 6;
const int trafficLightsTwo_GRN = 7;

const int studentLights_GRN = 8;
const int studentLights_BLE = 9;
const int studentLights_RED = 10;

const int buttonTrafficLight = 2;

const int microphonePin = A0;
const int pinLDR = A3;

//Claping Stuff
int clapThreshold = 150;  // Adjust this threshold based on your environment and microphone sensitivity
int clapCount = 0;
unsigned long lastClapTime = 0;
bool ledsOn = false;

//RTC Stuff
DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;
byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits;
bool alarmDy, alarmH12Flag, alarmPmFlag;

//DHT11 Stuff
uint32_t delayMS;

void setup() {

  Serial.begin(9600);
  Wire.begin();

  screenSetup();
  displayAxiometa();

  dht.begin();
  sensor_t sensor;
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  //traffic lights one
  pinMode(trafficLightsOne_RED, OUTPUT);
  pinMode(trafficLightsOne_YLW, OUTPUT);
  pinMode(trafficLightsOne_GRN, OUTPUT);

  //traffic lights two
  pinMode(trafficLightsTwo_RED, OUTPUT);
  pinMode(trafficLightsTwo_YLW, OUTPUT);
  pinMode(trafficLightsTwo_GRN, OUTPUT);

  //student lights
  pinMode(studentLights_RED, OUTPUT);  //window b
  pinMode(studentLights_GRN, OUTPUT);  //window g
  pinMode(studentLights_BLE, OUTPUT);  //window r

  //street lights
  pinMode(streetLights, OUTPUT);  //street lights

  //button on traffic light one
  pinMode(buttonTrafficLight, INPUT_PULLUP);  // button

  //setTime(); // run it only when needed disable on operation

  //for fun
  ledStatus(studentLights_RED, 1);
}

void loop() {

  //studentsFlat();

  //trafficLightOne();
  //trafficLightTwo();

  //autoStreetLights();

  //DHT11OLED();

  //displayAxiometa();

  //clapLights();

  //serialPrintTime();

  oledPrintTime();
}



void oledPrintTime() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 25);
  display.print(myRTC.getHour(h12Flag, pmFlag), DEC);
  display.print(":");
  display.print(myRTC.getMinute(), DEC);
  display.print(":");
  display.println(myRTC.getSecond(), DEC);
  display.display();
}

void clapLights() {
  // Read analog value from the microphone
  int micValue = analogRead(microphonePin);

  // Check for claps
  if (micValue > clapThreshold && millis() - lastClapTime > 200) {
    clapCount++;
    lastClapTime = millis();

    if (clapCount == 2) {
      // Toggle LEDs on/off when two claps are detected
      if (ledsOn) {
        digitalWrite(streetLights, HIGH);
        ledsOn = false;
      } else {
        digitalWrite(streetLights, LOW);
        ledsOn = true;
      }
      clapCount = 0;  // Reset clap count
    }
  }
}

void autoStreetLights() {
  if (analogRead(pinLDR) < 300) {
    digitalWrite(streetLights, HIGH);
  } else {
    digitalWrite(streetLights, LOW);
  }
}

void trafficLightOne() {
  digitalWrite(trafficLightsOne_RED, HIGH);
  if (digitalRead(buttonTrafficLight) == 0) {
    delay(2000);
    digitalWrite(trafficLightsOne_RED, LOW);
    digitalWrite(trafficLightsOne_YLW, HIGH);
    delay(1000);
    digitalWrite(trafficLightsOne_YLW, LOW);
    digitalWrite(trafficLightsOne_GRN, HIGH);
    delay(2000);
    blinkLED(trafficLightsOne_GRN);
    digitalWrite(trafficLightsOne_GRN, LOW);
    digitalWrite(trafficLightsOne_YLW, HIGH);
    delay(1000);
    digitalWrite(trafficLightsOne_YLW, LOW);
  }
}

void trafficLightTwo() {
  digitalWrite(trafficLightsTwo_GRN, HIGH);
}


void studentsFlat() {
  int micValue = analogRead(microphonePin);
  int ledIntensity = map(micValue, 0, 1023, 0, 255);

  // Update the RGB LEDs using PWM
  analogWrite(studentLights_RED, ledIntensity);
  analogWrite(studentLights_GRN, ledIntensity);
  analogWrite(studentLights_BLE, ledIntensity);
}

void blinkLED(int blinkingLEDPin) {
  for (unsigned int i = 0; i < 5; i++) {
    digitalWrite(blinkingLEDPin, HIGH);
    delay(500);
    digitalWrite(blinkingLEDPin, LOW);
    delay(500);
  }
}

void screenSetup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
}

void displayAxiometa() {
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(30, 10);
  // Display static text
  display.println("AXIO");
  display.setCursor(30, 40);
  display.println("META");

  display.display();
}

void DHT11OLED() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    display.setCursor(0, 25);
    display.print("Temp: ");
    display.println(event.temperature - 2.7, 1);

    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  display.display();
}

void setTime() {
  myRTC.setClockMode(false);  // set to 24h
  //setClockMode(true); // set to 12h
  myRTC.setYear(2024);
  myRTC.setMonth(1);
  myRTC.setDate(23);
  myRTC.setDoW(2);
  myRTC.setHour(1);
  myRTC.setMinute(4);
  myRTC.setSecond(10);
}

void serialPrintTime() {
  Serial.print(myRTC.getHour(h12Flag, pmFlag), DEC);
  Serial.print(":");
  Serial.print(myRTC.getMinute(), DEC);
  Serial.print(":");
  Serial.println(myRTC.getSecond(), DEC);
}

void ledStatus(int LED, int status) {
  if (status == 1) {
    digitalWrite(LED, HIGH);
  } else if (status == 0) {
    digitalWrite(LED, LOW);
  } else {
    //dunno
  }
}