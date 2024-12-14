#include <DHT11.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

/*
This project use a DHT11 temperature and humidity sensor to read data in real-time
Which will then be displayed through Serial monitor and LCD screen.
The data that's collected will be logged in a 1GB SD card with an entry interval of every 10 seconds

Purpose of this project is to understand how temperature and humidity changes throughout the days
which can be used to track temperature for efficiency and energy saving in using air conditioner.
*/

long entryInterval = 10000;
long previousMillis = 0;
unsigned long currentMillis;
//To display in seconds interval without delays

const int tempHumidSensor = 7;
DHT11 sensorReading(tempHumidSensor); //Temperature and humidity sensor object
int tempF, tempC, humid;
int unit = 0;

File datalog; //File location to store data

//connecting LCD display to Arduino pins
const int RS = 8;
const int EN = 9;
const int D4 = 2;
const int D5 = 3;
const int D6 = 10;
const int D7 = 5;
LiquidCrystal lcdScreen(RS, EN, D4, D5, D6, D7);

void setup() {
  Serial.begin(9600);
  lcdScreen.begin(16,2);  //Set up LCD screen

  if(!SD.begin(4)){
    Serial.println("initializing");
    while(1);
  } else Serial.println("initialization done");

  datalog = SD.open("tracking.txt",FILE_WRITE); //open file in write mode
  if(datalog){
    Serial.println("File is open");
  }
}

void loop() {
  sensorReading.readTemperatureHumidity(tempC, humid);
  tempF = (tempC * (9/5)) + 32;   //converting Celcius to Fahrenheit degrees since
                                  //sensor only returns temperature in Celcius

  printIt(tempC, tempF, humid);   //Display in Serial monitor
  displayLCD(tempC,tempF,humid);  //Display on LCD screen

  char command = Serial.read();   //Reading command from Serial monitor
  currentMillis = millis();

  char status = writeToFile(tempC, tempF, humid, command, currentMillis);
  if(status == 'o'){          //only write to the file if input command is 'o'
    datalog = SD.open("tracking.txt",FILE_WRITE);
    Serial.println("File is open again for new entry");
  }
}

void printIt(int C, int F, int H){
  Serial.print("TEMP ");
  Serial.print(F);
  Serial.print("F   ");
  Serial.print("TEMP ");
  Serial.print(C);
  Serial.print("C   ");
  Serial.print("HUMIDITY ");
  Serial.print(H);
  Serial.println("%   ");
}
void displayLCD(int C, int F, int H){
  if(unit % 2 == 0){
    lcdScreen.setCursor(0,0);
    lcdScreen.print("TEMP ");
    lcdScreen.print(C);
    lcdScreen.print("C");
    unit += 1;
  }else {
    lcdScreen.setCursor(0,0);
    lcdScreen.print("TEMP ");
    lcdScreen.print(F);
    lcdScreen.print("F");
    unit += 1;
  } //Switching between displaying temperature in Celcius and Fahrenheit degrees

  lcdScreen.setCursor(0,1);
  lcdScreen.print("Humidity ");
  lcdScreen.print(H);
  lcdScreen.print("%");
  delay(1000);
  
}

char writeToFile( int C, int F, int H, char command, long current){
  int seconds = current / 1000;
  int minutes = 0;
  if(seconds > 60){
    minutes = seconds /60;
    seconds = seconds - (60 * minutes);
  }
  if(datalog){
    if(command != 'S' && command != 's'){     //If stop command, 's' or 'S', is not detected
      if(current - previousMillis > entryInterval){ //only write to the file after seconds interval
      previousMillis = current;
      Serial.println("Writing to the File");
      datalog.print(minutes);
      datalog.print("m");
      datalog.print(seconds);
      datalog.print("s   ");
      datalog.print("TEMP ");
      datalog.print(F);
      datalog.print("F   ");
      datalog.print("TEMP ");
      datalog.print(C);
      datalog.print("C   ");
      datalog.print("HUMIDITY ");
      datalog.print(H);
      datalog.println("%   ");
      datalog.close();
      Serial.println("File is closed now");
      return 'o';
      }else return 'r';
    } else{
      Serial.println("Stop writing to the file.");
      datalog.print(minutes);
      datalog.print("m");
      datalog.print(seconds);
      datalog.print("s   ");
      datalog.println("This entry session is ended.");
      datalog.close();
      return 'r';
    }
  }else{ 
    Serial.println("Not writing to File");
    return 'r';
  }
}

