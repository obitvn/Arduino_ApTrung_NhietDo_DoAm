#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>


#define DHTPIN 2     // what digital pin we're connected to

#define MODE 7
#define UP 8
#define DOWN 9

#define RELAY1 11  // dao trung
#define RELAY2 12  // quat 1,2
#define RELAY_SSR 10  // nhiet

#define DHTTYPE DHT11 
// Chân nối với Arduino
#define ONE_WIRE_BUS 3
//Thiết đặt thư viện onewire
OneWire oneWire(ONE_WIRE_BUS);
//Mình dùng thư viện DallasTemperature để đọc cho nhanh
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(13,A0,A1,A2,A3,A4,A5);

uint8_t mode, set_temp;
uint16_t set_time;
unsigned long start_time, time_now, minute;
unsigned long  time_read;
float temp = 0;

void time_to_minute()
{
  float minute_float;
  time_read = millis();
  time_now = time_read - start_time;
  minute_float = time_now/1000/60;
  minute = minute_float;
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Do An Tot Nghiep");
  pinMode(MODE, INPUT);
  pinMode(UP,   INPUT);
  pinMode(DOWN, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY_SSR, OUTPUT);
  set_temp = EEPROM.read(0);
  set_time =  EEPROM.read(2);
  Serial.begin(9600);
  dht.begin();
  delay(3200);
  start_time = millis();
  lcd .clear ();
}

void scan_button()
{
  if(digitalRead(MODE) == 0)
  {
   
    while (digitalRead(MODE) == 0) ;
      {
      };
    
    mode ++;
    if(mode > 5) mode = 0;
  }
  else if(mode==2)  //set time
  {
     if(digitalRead(UP) == 0)
       {
        while (digitalRead(UP) == 0) ;
        {
          
        };
        set_time++;
        if(set_time>65000) 
        {
          set_time = 65000;
        }
       }
     if(digitalRead(DOWN) == 0)
       {
        while (digitalRead(DOWN) == 0) ;
        {
          
        };
        set_time--;
        if(set_time<1) 
        {
          set_time = 0;
        }
       }
  }

  else if(mode==4)  //set temp
  {
      if(digitalRead(UP) == 0)
       {
         while (digitalRead(UP) == 0) ;
         set_temp++;
         if(set_temp>254)
         {
          set_temp = 254;
         }
       }
     if(digitalRead(DOWN) == 0)
       {
         while (digitalRead(DOWN) == 0) ;
         set_temp--;
         if(set_temp<1) 
        {
          set_temp = 0;
        }
       }
  }

  
}

void control_relay()
{
  if(set_time < (minute+1)) 
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dao Trung       ");
    digitalWrite(RELAY1,1);
    delay(5000); // delay 5s dao trung
    start_time = millis();
    minute = 0;
    lcd.clear();
  }
  else
  {
    digitalWrite(RELAY1,0);
  }
  if(set_temp > temp)
  {
    digitalWrite(RELAY_SSR,1);
  }
  else 
  {
    digitalWrite(RELAY_SSR,0);
    if(set_temp < temp-5 )
    {
      digitalWrite(RELAY2,1);
    }
    else
    {
     digitalWrite(RELAY2,0); 
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
 
  if(mode == 0)
  {
    /////// print temp to lcd
    lcd.setCursor(0, 0);
    lcd.print("Temp");
    sensors.requestTemperatures();
    lcd.setCursor(6, 0);
    temp = sensors.getTempCByIndex(0);
    float h = dht.readHumidity();    //Đọc độ ẩm
    lcd.print(temp);
    lcd.setCursor(13, 0);
    lcd.print(set_temp);
    /// print time to lcd
    lcd.setCursor(0, 1);
    lcd.print("T");
    lcd.setCursor(2, 1);
    lcd.print(minute);
    lcd.setCursor(7, 1);
    lcd.print(set_time);
    lcd.setCursor(10, 1);
    lcd.print("H");
    lcd.setCursor(12, 1);
    lcd.print(h);
    
  }
  else if(mode == 1)
  {
    lcd.clear();
    mode = 2;
  }
  else if(mode == 2)
  {
    lcd.setCursor(0, 0);
    lcd.print("Thoi Gian");
    lcd.setCursor(0, 1);
    lcd.print("Dao Trung:");
    lcd.setCursor(12, 1);
    lcd.print(set_time);
  }
  else if(mode == 3)
  {
    EEPROM.write(2,set_time);
    lcd.clear();
    mode =4;
  }
  else if(mode == 4)
  { 
    lcd.setCursor(0, 0);
    lcd.print("Nhiet Do");
    lcd.setCursor(0, 1);
    lcd.print("Cai Dat:");
    lcd.setCursor(12, 1);
    lcd.print(set_temp);
  }
  else if(mode == 5)
  {
    EEPROM.write(0,set_temp);
    lcd.clear();
    mode = 0;
  }
  control_relay();
  scan_button();
  time_to_minute();
}
