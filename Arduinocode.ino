#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial GSerial(2,3); 

#define DELAY 30000
//#include "DHT.h"
//#define DHTPIN A1     // what digital pin we're connected to
#define LM35 A0       // what digital pin we're connected to
#define LED 13
#define SW 10
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
float temp,temp_max = 40,smoke;
float humidity;
unsigned int panic_switch,panic_state;;
unsigned long previousMillis;
int smoke_flag=0;
int temp_flag=0;

float find_temp()
{
  int i;
  long avg_val = 0;
  for(i=0;i<1000;i++)
  {
    avg_val += analogRead(LM35);
  }
  avg_val/=1000;
  temp = avg_val*(500.0/1023.0);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp,1);
  lcd.print("*C  ");

  if(temp>50)
  {
    temp_flag=1;
  }

  avg_val = 0;
  for(i=0;i<1000;i++)
  {
    avg_val += analogRead(A1);
  }
  avg_val/=1000;
  smoke = avg_val*(500.0/1023.0);
  smoke = map(smoke,75,250,0,100);
  lcd.print("S:");
  if(smoke>=0&&smoke<=100)
    lcd.print(int(smoke));
  else
    lcd.print("0");
  lcd.print("%        ");
  if(smoke>50)
  {
    smoke_flag=1;
  }
  //return temp;
}


void read_param()
{
  panic_switch = digitalRead(SW);
  if(panic_switch == LOW)
  {
    panic_state = 1;  
  }
  lcd.setCursor(0, 1);
  lcd.print("P:");
  if(panic_state)
    lcd.print("ACTIVE");
  else  
    lcd.print("INACTIVE");
  lcd.print("              "); 
}

void upload_values()
{
  if(millis() - previousMillis >= DELAY) // Sends values to WiFi Every 10 sec for uploading on Thingspeak
  {
    lcd.clear();
    lcd.print("UPLOADING...");
    previousMillis = millis();
    GSerial.print("<T");GSerial.print(temp);GSerial.println(">");
    delay(100);
    GSerial.print("<P");GSerial.print(panic_state);GSerial.println(">");
    delay(100);
    GSerial.print("<S");GSerial.print(smoke);GSerial.println(">");
    delay(100);
    GSerial.println("<U>");
    delay(100);
    if(smoke_flag==1)
    {GSerial.println("<1>");}
    if(panic_state==1)
    {GSerial.println("<2>");}
    if(temp_flag==1)
    {GSerial.println("<3>");}
    smoke_flag = 0;
    panic_state = 0;
    temp_flag=0;
  }
}

void setup() 
{
  Serial.begin(9600);
  GSerial.begin(9600); 
  Serial.println("START");
  lcd.begin(16, 2);
  pinMode(LED, OUTPUT);   
  digitalWrite(LED, HIGH);
  pinMode(SW,INPUT_PULLUP);    
  // Print a message to the LCD.
  lcd.print("   Emergency   ");
  lcd.setCursor(0, 1);
  lcd.print(" Response System");
  delay(1500);        // waits for the servo to get there
  digitalWrite(LED, LOW);   
  lcd.clear();  
  previousMillis = millis();
}

void loop() 
{
  find_temp();
  read_param();
  upload_values();
  delay(100);  
}
