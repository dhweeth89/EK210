#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
int ThermistorPin1 = 0;
int ThermistorPin2 = 1;
int ThermistorPin3 = 2;
int ThermistorPin4 = 3;
int FanHeatPin = 8;
bool StuffOn = true;
bool StartDisplaying = false;


int Vo, V1, V2, V3;
float R0 = 10000; //Thermistor Resistance at 25 C
float T0 = 298.15; //Room Temperature in Kelvin
float Bcoeff = 3950; //Known B-Coefficient of our Thermistor
float R1 = 10000; //Value of the other Resistor
float R2, R3, R4, R5;
float T1, T2, T3, T4;
float logR2, logR3, logR4, logR5; //The ln of the ratio of read Resistance over Room Temperature resistance
//float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

float currentT;
float maxT = 0;
float minT = 1000;
float avgT;

float runsumT = 0;
int Tcounts = 0;


//Equation for calculating temperature is: 1/T = 1/T0 + (1/B) ln(R/R0)

void setup() {
Serial.begin(9600);

lcd.init();
lcd.init();

lcd.backlight();
lcd.setCursor(1,0);
lcd.print("Incubator EK210");
//lcd.setCursor(1,1);
//lcd.print("konichiwaa");

pinMode(FanHeatPin, OUTPUT);

delay(1000); 
}

void loop() {

  
  Vo = analogRead(ThermistorPin1);
  V1 = analogRead(ThermistorPin2);
  V2 = analogRead(ThermistorPin3);
  V3 = analogRead(ThermistorPin4);

  //Reading Resistance of Thermistor 1
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  
  //Reading Resistance of Thermistor 2
  R3 = R1 * (1023.0 / (float)V1 - 1.0);

  //Reading Resistance of Thermistor 3
  R4 = R1 * (1023.0 / (float)V2 - 1.0);

  //Reading Resistance of Thermistor 4
  R5 = R1 * (1023.0 / (float)V3 - 1.0);

  //Getting Temperature 1
  logR2 = log(R2/R0); 
  T1 = 1 / ((1/T0) + ((1/Bcoeff) * logR2));  
  T1 = T1 - 273.15;

  //Getting Temperature 2
  logR3 = log(R3/R0); 
  T2 = 1 / ((1/T0) + ((1/Bcoeff) * logR3));  
  T2 = T2 - 273.15;

  //Getting Temperature 3
  logR4 = log(R4/R0); 
  T3 = 1 / ((1/T0) + ((1/Bcoeff) * logR4));  
  T3 = T3 - 273.15;

  //Getting Temperature 4
  logR5 = log(R5/R0);
  T4 = 1 / ((1/T0) + ((1/Bcoeff) * logR5));
  T4 = T4 - 273.15;

  //Display Current Temperature of each Thermistor on the computer (For testing)  
  Serial.print("Temperature of Thermistor 1: "); 
  Serial.print(T1);
  Serial.println(" C"); 

  Serial.print("Temperature of Thermistor 2: "); 
  Serial.print(T2);
  Serial.println(" C");

  Serial.print("Temperature of Thermistor 3: "); 
  Serial.print(T3);
  Serial.println(" C");

  Serial.print("Temperature of Thermistor 4: "); 
  Serial.print(T4);
  Serial.println(" C");

  Serial.println("");
  
  
  //Serial.print("Vo is: ");
  //Serial.print(Vo);
  //Serial.println(" V");

//Determines whether the Fan and Heating element should be on
  if (StuffOn == true)
  {
    digitalWrite(FanHeatPin, HIGH);
  }
  else
  {
    digitalWrite(FanHeatPin, LOW);
  }


//Calculates the current Temperature of incubator
  currentT = (T1 + T2 + T3 + T4) / 4;


//Only start reading maxT once we want to start displaying these values
  if (currentT > maxT && StartDisplaying == true)
  {
    maxT = currentT;
  }

//Only start reading minT once we want to start displaying these values
  if (currentT < minT && StartDisplaying == true)
  {
    minT = currentT;
  }

//Only start calculating average Temperature once we want to start displaying value
  if (StartDisplaying == true)
  {
    Tcounts += 1;
    runsumT += currentT;
    avgT = runsumT / Tcounts;
  }

//Sets the lcd to start displaying only once it reaches 37 degrees Celsius for the first time
  if (currentT >= 37)
  {
    StartDisplaying = true;
    minT = currentT;
  }

//This is for lcd display to do its thing 
//lcd.setCursor(1,0);
//lcd.print("Incubator EK210");
//lcd.setCursor(1,1);
//lcd.print("konichiwaa");

lcd.setCursor(1,0);
lcd.print("Current temp: ");
lcd.print(currentT);

if (StartDisplaying == true)
{
  lcd.setCursor(1,1);
  lcd.print("Max temp: ");
  lcd.print(maxT);

  lcd.setCursor(1,2);
  lcd.print("Min temp: ");
  lcd.print(minT);

  lcd.setCursor(1,3);
  lcd.print("Avg temp: ");
  lcd.print(avgT);
}


//This part requires experimenting; we can adjust values as we test
  //We have the heating element and fan turn off once it reaches a certain temperature
  //Then it turns on again when it gets closer and then turns off again once it reaches 37

//Fan and heating element are on if Temperature is pretty low
if (currentT < 31)
{
  StuffOn = true;
}

//Fan and heating element turn off when Temperature is approaching 37; this is the part that requires experimenting
if (currentT >= 31 && currentT < 36.5)
{
  StuffOn = false;
}

//Fan and heating element turn back on when Temperature is very close to 37; this is for control when the temperature oscillates below 37 degrees
if (currentT >= 36.5 && currentT < 37)
{
  StuffOn = true;
}

//Fan and heating element turn off again when Temperature crosses 37; this is for control when the temperature oscillates above 37 degrees
if (currentT >= 37)
{
  StuffOn = false;  
}

  

delay(500);
}
