#include <SoftwareSerial.h>

// AT+CWMODE_DEF=1                                  station模式
// AT+CWJAP_DEF="SFR_BF88","efloncemonecre2sotoo"   连接wifi
// AT+CIPMODE=0                                     非透传模式
// AT+CIPSSLSIZE=4096
// AT+CIPSTART="SSL","catprogrammer.com",443
// AT+CIPSEND=18
// GET / HTTP/1.1
// GET /DashBoard/uploadData.php?dt=2017-10-07+14:40:01&t=23.7&h=56.6 HTTP/1.1\r\nHost: catprogrammer.com
// AT+CIPCLOSE

const int ESP_RX_PIN = 10;
const int ESP_TX_PIN = 11;
const int ESP_RST_PIN = 12;

SoftwareSerial esp8266(ESP_RX_PIN, ESP_TX_PIN); // RX, TX

void espSleep() {
  esp8266.write("AT+GSLP=20000\r\n");
}

void espRestart() {
  digitalWrite(ESP_RST_PIN, LOW);  
  delay(100);
  digitalWrite(ESP_RST_PIN, HIGH);
}

void espPrintResponse() {
  while (esp8266.available()) {
    Serial.println(esp8266.readStringUntil('\n')); 
  }
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("Start");

  String msg = String("GET /DashBoard/uploadData.php?") +
               "f=" + 99 + "&fp=" + 99 + "&w=" + 99 + "&wp=" + 99 + "&isfed=" + 1 +
               " HTTP/1.1\r\nHost: catprogrammer.com\r\n";

  
  Serial.println(msg);
  pinMode(ESP_RST_PIN, OUTPUT);
  digitalWrite(ESP_RST_PIN, HIGH);
  
  espRestart();
  delay(3000);
  
  
  // set the data rate for the SoftwareSerial port
  esp8266.begin(115200);
  espPrintResponse();
  
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

void loop() // run over and over
{ 
  if (esp8266.available())
    Serial.write(esp8266.read());
  if (Serial.available())
    esp8266.write(Serial.read());
}
