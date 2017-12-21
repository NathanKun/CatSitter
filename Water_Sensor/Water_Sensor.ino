const int WATER_SENSOR_PIN = A0; //Sensor AO pin to Arduino pin A0

int i;

int waterSensorData;          //Variable to store the incomming data
int deep = 0;

void setup()
{
  //Begin serial communication
  Serial.begin(9600);

}

void loop()
{
  waterSensorData = analogRead(WATER_SENSOR_PIN); //Read data from analog pin and store it to value variable

  if (waterSensorData <= 100) { // 6 - 9
    deep = 3;
  }
  else if (waterSensorData > 100 && waterSensorData <= 700) { // 537 - 540
    deep = 2;
  }
  else if (waterSensorData > 700 && waterSensorData <= 900) { // 805 - 809
    deep = 1;
  }
  else if (waterSensorData > 900) { // 943 - 948
    deep = 0;
  }
  Serial.print(waterSensorData);
  Serial.print("  :  ");
  Serial.println(deep);
  delay(500);

}

