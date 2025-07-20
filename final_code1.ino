#include <Servo.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS D2
#define buzzer D6
#define sensorPower D4
#define sensorPin D5
#define soundPin D7

int servoPin = D8;
Servo servo;
int angle = 0;  // servo position in degrees

String apiWritekey = "YA80RRWWC9R9XF1C"; //Write your writeAPI here
const char* ssid = "TP-31_5G"; //Write your SSID
const char* password = "vasavicc" ; //Write your Wifi Password
const char* server = "api.thingspeak.com"; //To initializing the device with Thingspeak API
WiFiClient client;

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void ICACHE_RAM_ATTR ISRoutine();

bool mValue ;
const int limit=1019;


int soundVal = 0;
int previousState = LOW; 
bool soundDetected=0;


void setup(void)
{
  
  servo.attach(servoPin);
  pinMode(sensorPower, OUTPUT);

  // Initially keep the sensor OFF
  digitalWrite(sensorPower, LOW);
  pinMode(buzzer, OUTPUT);
  //pinMode (relayPin, OUTPUT);
  pinMode (soundPin, INPUT);
  pinMode(D2,INPUT);
  sensors.begin();  // Start up the library
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(soundPin),ISRoutine,RISING);
  WiFi.disconnect(); //disconnecting previously connected Wifi
  delay(10);
  WiFi.begin(ssid, password); //connecting to provided Wifi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println("NodeMcu connected to wifi"); //NodeMCU connected to Wifi
}

void loop(void)
{ 
  float temp;
 
  while(1){
    // Send the command to get temperatures
    sensors.requestTemperatures(); 
  
    //print the temperature in Celsius
    Serial.print("Temperature: ");
    temp=sensors.getTempCByIndex(0);
    Serial.print(temp);
    Serial.print((char)176);//shows degrees character
    Serial.print("C  |  ");
    
    //print the temperature in Fahrenheit
    Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
    Serial.print((char)176);//shows degrees character
    Serial.println("F");
    break;
  }
  //digitalWrite(relayPin,HIGH);
  if(soundDetected==1){
    for(angle = 0; angle < 160; angle++) {
        servo.write(angle);
        delay(15);
    }
    
    // now scan back from 180 to 0 degrees
    for(angle = 160; angle > 0; angle--) {
        servo.write(angle);
        delay(15);
    } 
  }

  while(1)
   {
     
     //get the reading from the function below and print it
     mValue = readSensor();
     break;
 }
 if (mValue==0 || temp>=37.2)
 {
  digitalWrite(buzzer,HIGH);
  mValue=1;
 }
 else{
  digitalWrite(buzzer,LOW);
  mValue=0;
 }
 Serial.print("Moisture:");  
 Serial.println(mValue);
 Serial.print("\n");

 if (client.connect(server,80)) //connecting to Thingspeak Server
  {
    String tsData = apiWritekey;
    tsData +="&field1=";  
    tsData += String(temp);
    tsData +="&field2=";  
    tsData += String(mValue);
    tsData +="&field3="; 
    tsData += String(soundDetected);
    
    tsData += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData); //temperature value sent to field1 of Thingspeak channel
     
    
}
 client.stop();
 delay(1000);
 soundDetected=0;
 mValue=0;
}

void ISRoutine ()
{
  soundDetected=1;
  Serial.println("sound detected");
  
  
}
int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // Allow power to settle
  int val = digitalRead(sensorPin); // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return val;             // Return analog moisture value
}
