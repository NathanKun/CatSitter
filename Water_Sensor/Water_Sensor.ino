const int WATER_SENSOR_PIN = A0; //Sensor AO pin to Arduino pin A0

int i;

int waterSensorData;          //Variable to store the incomming data
int deep = 0;
int deepSum = 0;

void setup()
{
  //Begin serial communication
  Serial.begin(9600);
  
}

void loop()
{
  for(i = 0; i < 10; i++){
    waterSensorData = analogRead(WATER_SENSOR_PIN); //Read data from analog pin and store it to value variable
  
    if (waterSensorData<=560){ 
      deep = 0;
    }
    else if (waterSensorData>560 && waterSensorData<=600){ 
      deep = 10;
    }
    else if (waterSensorData>600 && waterSensorData<=620){ 
      deep = 20;
    }
    else if (waterSensorData>620 && waterSensorData<=640){ 
      deep = 30;
    }
    else if (waterSensorData>640 && waterSensorData<=660){ 
      deep = 40;
    }
    else if (waterSensorData>660){ 
      deep = 50;
    }
    deepSum += deep;
    delay(100);
    
    Serial.print(deep);
    Serial.print("  ");
  }
  
  Serial.print(deepSum * 100 / 10 / 50);
  Serial.println("%");
  deepSum = 0;

}

