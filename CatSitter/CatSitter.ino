#include <avr/sleep.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include "Adafruit_VL53L0X.h"

/**
   Variables
*/

// pins
const int SERVO_PIN = 5;
const int WATER_SENSOR_PIN = A0;
const int ESP_RX_PIN = 10;
const int ESP_TX_PIN = 11;
const int ESP_RST_PIN = 12;

int i;

// servo
const int TURNING_TIME = 550;
const int CW_MIN = 90;
const int CW_MAX = 85;
const int CCW_MIN = 99;
const int CCW_MAX = 107;
const int TURN_STOP = 94;
Servo myservo;

// VL53L0X
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// water sensor
int waterSensorData;          //Variable to store the incomming data
int deep = 0;
int deepSum = 0;

// ESP8266
SoftwareSerial esp8266(ESP_RX_PIN, ESP_TX_PIN); // RX, TX

// watchdog
volatile int sleep_count = 0; // Keep track of how many sleep cycles have been completed.
const long INTERVAL = 470; // interval in minutes between waking and doing tasks.
const long SLEEP_TURN_SERVO = (INTERVAL * 60) / 8;
const long SLEEP_MESURE1 = SLEEP_TURN_SERVO / 4;
const long SLEEP_MESURE2 = SLEEP_MESURE1 * 2;
const long SLEEP_MESURE3 = SLEEP_MESURE1 * 3;



/**
   Functions
*/

// ESP8266
void espSleep() {
  esp8266.write("AT+GSLP=20000\r\n");
}

void espRestart() {
  digitalWrite(ESP_RST_PIN, LOW);
  delay(100);
  digitalWrite(ESP_RST_PIN, HIGH);
}

void espPrintResponse() {
  /*while (esp8266.available()) {
    Serial.println(esp8266.readStringUntil('\n'));
  }*/
}

// servo
void clockwiseStart() {
  for (int i = CW_MIN + 1; i > CW_MAX; i--) {
    myservo.write(i);
    //    delay(50);
  }
}

void conterclockwiseStart() {
  for (int i = CCW_MIN - 1; i < CCW_MAX; i++) {
    myservo.write(i);
    //    delay(25);
  }
}

void turn(const int TURNING_TIME) {
  clockwiseStart();
  delay(TURNING_TIME);
  myservo.write(TURN_STOP);
  delay(1000);
}

// watchdog
void watchdogOn() {
  MCUSR = MCUSR & B11110111;
  WDTCSR = WDTCSR | B00011000;
  WDTCSR = B00100001;
  WDTCSR = WDTCSR | B01000000;
  MCUSR = MCUSR & B11110111;
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
  sleep_enable(); // Enable sleep mode.
  sleep_mode(); // Enter sleep mode.

  sleep_disable(); // Disable sleep mode after waking.
}

// mesure and send
void mesureAndSend(boolean isfed) {
  int foodMM, foodPCT, waterMM, waterPCT;
  espRestart();
  delay(5000);
  espPrintResponse();

  // VL53L0X
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    foodMM = measure.RangeMilliMeter;
    foodPCT = 100 - (foodMM - 40) * 100 / (270 - 40);
  } else {
    foodMM = -1;
    foodPCT = -1;
  }

  // water sensor
  waterSensorData = analogRead(WATER_SENSOR_PIN); //Read data from analog pin and store it to value variable

  if (waterSensorData <= 100) { // 6 - 9
    waterMM = 3;
    waterPCT = 100;
  }
  else if (waterSensorData > 100 && waterSensorData <= 700) { // 537 - 540
    waterMM = 2;
    waterPCT = 66;
  }
  else if (waterSensorData > 700 && waterSensorData <= 900) { // 805 - 809
    waterMM = 1;
    waterPCT = 33;
  }
  else if (waterSensorData > 900) { // 943 - 948
    waterMM = 0;
    waterPCT = 0;
  }

  // ESP8266
  String fed = "";
  if (isfed) {
    fed = "1";
  } else {
    fed = "0";
  }

  String msg = String("GET /DashBoard/uploadData.php?") +
               "f=" + foodMM + "&fp=" + foodPCT + "&w=" + waterMM + "&wp=" + waterPCT + "&isfed=" + fed +
               " HTTP/1.1\r\nHost: catprogrammer.com\r\n";

  /*esp8266.println("AT+CIPSSLSIZE=4096");
    delay(1000);
    espPrintResponse();*/

  //esp8266.println("AT+CIPSTART=\"SSL\",\"104.224.146.172\",443");
  esp8266.println("AT+CIPSTART=\"TCP\",\"104.224.146.172\",80");
  delay(2000);
  espPrintResponse();

  esp8266.println("AT+CIPSEND=" + String(msg.length() + 2));
  delay(1000);
  espPrintResponse();

  esp8266.println(msg);
  delay(3000);
  espPrintResponse();

  delay(10000);
  espSleep();
}

/**
   setup
*/
void setup(void) {
  // watchdog init
  watchdogOn(); // Turn on the watch dog timer.

  // VL53L0X init
  if (!lox.begin()) {
    //Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

  // servo init
  myservo.attach(SERVO_PIN);

  // ESP8266 init
  pinMode(ESP_RST_PIN, OUTPUT);
  digitalWrite(ESP_RST_PIN, HIGH);
  esp8266.begin(115200);
  espPrintResponse();

  // turn, mesure and send
  turn(TURNING_TIME);
  mesureAndSend(true);

}

void loop(void) {
  goToSleep(); // ATmega328 goes to sleep for about 8 seconds


  if (sleep_count == SLEEP_MESURE1 || sleep_count == SLEEP_MESURE2
      || sleep_count == SLEEP_MESURE3) {
    mesureAndSend(false);
  } else if (sleep_count == SLEEP_TURN_SERVO) {
    turn(TURNING_TIME);
    mesureAndSend(true);
    sleep_count = 0;
  }
}

ISR(WDT_vect)
{
  sleep_count ++; // keep track of how many sleep cycles have been completed.
}
