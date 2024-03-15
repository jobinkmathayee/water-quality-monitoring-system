#include <math.h>
#include <Wire.h>

//ph sensor
const int potPin=32;
float ph;
float Value=0;
float PHsensor=0;

//temperature
#include <OneWire.h>
#include <DallasTemperature.h>
#define SENSOR_PIN  17 // ESP32 pin GPIO17 connected to DS18B20 sensor's DATA pin
OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit


//tds
#define TdsSensorPin 27
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point
#define SENSOR_PIN  17 // ESP32 pin GPIO17 connected to DS18B20 sensor's DATA pin
int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation



//Turbidity
const int turbPin = 34;
 float sensor=0;
 int sensorValue;

//water level
const int echoPin = 18;
const int trigPin = 19;
const int inputPumpPin =  22;    // Digital pin for input pump control
const int outputPumpPin = 23;   // Digital pin for output pump control



// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}




void setup()
{
Serial.begin(115200);
pinMode(TdsSensorPin,INPUT);//tds
DS18B20.begin();    // initialize the DS18B20 sensor(temperature)
  pinMode(echoPin, INPUT);//water level               
  pinMode(trigPin, OUTPUT);//water level
  pinMode(inputPumpPin, OUTPUT);
  pinMode(outputPumpPin, OUTPUT);


}



void loop(){


  //turbidity
  for (int i = 0; i < 50; i++) {
  sensor += analogRead(turbPin); // read the input on analog pin 3 (turbidity sensor analog output)
    delay(10);
  }
sensorValue = sensor / 50; // average the n values

  //int sensorValue = analogRead(35);
   Serial.print ("sensorValue=");
    Serial.println (sensorValue);
  float voltage = sensorValue * (3.33 / 4095);
 
  Serial.println ("Sensor Output (V):");
  Serial.println (voltage);
  Serial.println();
   float turbidity = ((-144.44*voltage)+294.44);
     Serial.println ("turbidity");
  Serial.println (turbidity);
  Serial.println ("ntu");
sensor = 0; // resets for averaging
  delay(100);


//temperature
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32; // convert °C to °F

  Serial.print("Temperature: ");
  Serial.print(tempC);    // print the temperature in °C
  Serial.print("°C");
  Serial.print("  ~  ");  // separator between °C and °F
  Serial.print(tempF);    // print the temperature in °F
  Serial.println("°F");

  delay(500);





//tds
static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }   
  
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4096.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
    }
  }

   delay(500);




   //ph

  for (int i = 0; i < 30; i++) {
  PHsensor += analogRead(potPin); // read the input on analog pin 3 (turbidity sensor analog output)
    delay(10);
  }
Value = PHsensor / 30; // average the n values
Serial.print("analog signal=");
Serial.println(Value);
float PHvoltage=Value*(3.33/4095.0);
Serial.print("PHvoltage=");
Serial.println(PHvoltage);
float vs=(PHvoltage*(10+20))/20;
Serial.print("orginal PHvoltage=");
Serial.println(vs);
ph=(1415-(375*vs))/62;
Serial.print("PH");
Serial.println(ph);
PHsensor = 0; // resets for averaging
delay(500);



//water level
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);         
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float pulse_duration = pulseIn(echoPin, HIGH);
  float distance = pulse_duration / 58.00;
  
  if (distance > 400) {    
    Serial.println("Out of Range");  
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  int waterLevel=(22.5-distance);
  Serial.println(waterLevel);
  
float lowerThreshold=8;
float upperThreshold=18;

 if (waterLevel < lowerThreshold) {
    // Water level is below the lower threshold, turn on the input pump
  Serial.println("water level is low");
    digitalWrite(inputPumpPin, HIGH);
    digitalWrite(outputPumpPin, LOW);  // Turn off the output pump
  } else if (waterLevel > upperThreshold) {
    // Water level is above the upper threshold, turn on the output pump
    Serial.println("water level is high");
    digitalWrite(outputPumpPin, HIGH);
    digitalWrite(inputPumpPin, LOW);  // Turn off the input pump
  } else {
    // Water level is within the acceptable range, turn off both pumps
    Serial.println("water level is normal");
    digitalWrite(inputPumpPin, HIGH);
    digitalWrite(outputPumpPin, HIGH);
  }

  delay(400);
}

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}/*
void PumpOn() {
    digitalWrite(inputPumpPin, HIGH);
    digitalWrite(outputPumpPin, LOW);  // Turn off the output pump
}
void PumpOff() {
    digitalWrite(outputPumpPin, HIGH);
    digitalWrite(inputPumpPin, LOW);  // Turn off the input pump
  } 
  void PumpConstant() {
    digitalWrite(inputPumpPin, LOW);
    digitalWrite(outputPumpPin, LOW);
  }*/
