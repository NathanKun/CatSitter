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
  for(i = 0; i < 5; i++){
    waterSensorData = analogRead(WATER_SENSOR_PIN); //Read data from analog pin and store it to value variable
  
    if (waterSensorData<=480){ 
      deep = 0;
    }
    else if (waterSensorData>480 && waterSensorData<=530){ 
      deep = 5;
    }
    else if (waterSensorData>530 && waterSensorData<=615){ 
      deep = 10;
    }
    else if (waterSensorData>615 && waterSensorData<=660){ 
      deep = 15;
    } 
    else if (waterSensorData>660 && waterSensorData<=680){ 
      deep = 20;
    }
    else if (waterSensorData>680 && waterSensorData<=690){ 
      deep = 25;
    }
    else if (waterSensorData>690 && waterSensorData<=700){ 
      deep = 23;
    }
    else if (waterSensorData>700 && waterSensorData<=705){ 
      deep = 35;
    }
    else if (waterSensorData>705){ 
      deep = 40;
    }
    deepSum += deep;
    delay(200);
  }
  
  Serial.println(deepSum / 5);
  Serial.print(deepSum / 5 * 100 / 40);
  Serial.println("%");
  deepSum = 0;

}

