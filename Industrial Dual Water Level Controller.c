/*
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

/*
  Ultrasonic Range Finder HC-SR04 Connections:
  Pin-10 - Trig
  Pin-13 - Echo
  Pin-GNG - Gnd
  Pin-5V - Vcc
*/ 
// include the library code:
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
//#include <DS3231.h>

RTC_DS3231 rtc;
DateTime now;


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display



// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int Time_Limit=1,check=2;
bool day=false;

int min = 0, reset = 0, motorCount = 6, m1 = 0, m2 = 0, prev_level = 0, TIME = 0;
bool BREAK = false, Inst = false, resetlvl = true, constant_m1 = false, constant_m2 = false;

float duration;//Ultra Sensor
int sensor_pos = 7;  //
float distance = sensor_pos;  //Ultra Sensor, when tank is full sensor is 10cm above the water level.
float full_tank = 15;  //Tank capacity
int water_level_tank = 100;  //Water level full (100%)
int water_level_sump=100;
bool Manual = false;
bool Motor_ON = false, OFF = true;
unsigned char Motor_Num = 1;

bool Button_State1=true,Button_State2=true,Button_State3=true,Button_State4=true;//1-page_change, 2-choose motor, 3-change day
int page_no=1;
int Motor_Array[7];
int Motor_Index=0;
int Update_Index=0;

int Motor_Day=1;//7days
int Motor_Number=1;
int Option=1;

unsigned long startTime;
unsigned long currentTime;

bool stop=false;
bool initiate=true;

int Motor1_Time_Min=0;
int Motor2_Time_Min=0;

uint32_t ut;
bool freeze=false;
uint32_t diff1=0;
uint32_t diff2=0;
bool Alternate=false;

uint32_t Initial_Water=0;
uint32_t Current_Water=0;
int Current_Water_Level=0;

bool Terminate1=false;
bool Terminate2=false;
bool Motor_off=true;
bool initiate2=true;
int M1=0;//0-off,1-on,2-fault
int M2=0;
void setup() 
{
  if (! rtc.begin()) 
  {
    Serial.println(" RTC Module not Present");
    while (1);
  }
  if (rtc.lostPower()) 
  {
    Serial.println("RTC power failure, reset the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  Serial.begin(9600);
  for(int i=0;i<7;i++)
  {
    //Motor_Array[i]=((i%2)+1);
    Motor_Array[i]=1;
  }

  
  lcd.begin(16, 2);// set up the LCD's number of columns and rows:
/* Keypad (2,1,4,3)*/
  pinMode(13, OUTPUT);

  pinMode(9,INPUT_PULLUP);//for next page
  pinMode(10,INPUT_PULLUP);//for prev page
  pinMode(11,INPUT_PULLUP);//for Motor change
  pinMode(12,INPUT_PULLUP);//for day change or press

  /*UltraSonic sensor Tank*/
  
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);

  /*UltraSonic sensor Sump*/

  pinMode(4, OUTPUT);
  pinMode(5, INPUT);

  //For motor ON/OFF
  pinMode(7, OUTPUT);//for m1
  pinMode(8, OUTPUT);//for m2 
  pinMode(6, OUTPUT);//testing
  digitalWrite(7,HIGH);
  digitalWrite(8,HIGH);
  //Wire.setClock(10000);
}
void Motor_Status()//displays which motor is on/off
{
  lcd.setCursor(10,0);
  lcd.print("M1:");
  
  if(M1==0)
    lcd.print("x");
  else if(M1==1)
    lcd.print("o");
  else if(M1==2)
    lcd.print("F");
  lcd.setCursor(10,1);
  lcd.print("M2:");
  if(M2==0)
    lcd.print("x");
  else if(M2==1)
    lcd.print("o");
  else if(M2==2)
    lcd.print("F");
}
void Lcd_Clear()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}
void Select_Motor(int index)
{
  Motor_Num=Motor_Array[index];
}
void get_water_level()
{
  //Ultra Sensor
  digitalWrite(2, LOW);
  delayMicroseconds(2);
  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);
  duration = pulseIn(3, HIGH);
  //Speed of sound is 349m/sec at 30deg.C
  sensor_pos=7;
  full_tank=13;
  distance = (0.0349 * (duration/2)) - sensor_pos;  //water level = total distance - sensor position above water level
  if(distance > full_tank)
  {
    distance = full_tank;
  }
  water_level_tank = ((full_tank - distance) / full_tank) * 100;  //find percentage of water level
  // Print a message to the LCD.
  if(page_no==1)
  {
    lcd.setCursor(0, 0);
    lcd.print("TANK:    ");
    lcd.setCursor(5,0);
    lcd.print(water_level_tank);
    lcd.print("%");
  }

  digitalWrite(4, LOW);
  delayMicroseconds(2);
  digitalWrite(4, HIGH);
  delayMicroseconds(10);
  digitalWrite(4, LOW);
  duration = pulseIn(5, HIGH);
  //Speed of sound is 349m/sec at 30deg.C
  sensor_pos=6;
  full_tank=11;
  distance = (0.0349 * (duration/2)) - sensor_pos;  //water level = total distance - sensor position above water level
  if(distance > full_tank)
  {
    distance = full_tank;
  }
  water_level_sump = ((full_tank - distance) / full_tank) * 100;  //find percentage of water level
  if(page_no==1)
  {
    lcd.setCursor(0,1);
    lcd.print("SUMP:    ");
    lcd.setCursor(5,1);
    lcd.print(water_level_sump);
    lcd.print("%");
  }
}
void Display_Heading()
{
  lcd.setCursor(0,0);
  lcd.print("S M T W T F S ");
}
int Choose_Motor()
{
  lcd.setCursor(15,0);
  
  if(Motor_Number==1)
  {
    Motor_Number=2;
    return Motor_Number;
  }
  else
  {
    Motor_Number=1;
    return Motor_Number;
  }
}
void Schedule(int i)
{
  Motor_Array[i-1]=Choose_Motor();
}
void DayPrint(int n)
{
  lcd.setCursor(14,0);
  switch(n)
  {
    case 0:
    {
      lcd.print("Sun");
      Motor_Num=Motor_Array[0];
      break;
    }
    case 1:
    {
      lcd.print("Mon");
      Motor_Num=Motor_Array[1];
      break;
    }
    case 2:
    {
      lcd.print("Tue");
      Motor_Num=Motor_Array[2];
      break;
    }
    case 3:
    {
      lcd.print("Wed");
      Motor_Num=Motor_Array[3];
      break;
    }
    case 4:
    {
      lcd.print("Thu");
      Motor_Num=Motor_Array[4];
      break;
    }
    case 5:
    {
      lcd.print("Fri");
      Motor_Num=Motor_Array[5];
      break;
    }
    case 6:
    {
      lcd.print("Sat");
      Motor_Num=Motor_Array[6];
      break;
    }

  }
}
void Schedule_Option(int n)
{
  //Lcd_Clear();
  switch(n)
  {
    case 1:
    {
      lcd.print("Motor 1  ");
      Alternate=false;
      for(int i=0;i<7;i++)
        Motor_Array[i]=1;
      day=false;
      break;
    }
    case 2:
    {
      lcd.print("Motor 2  ");
      Alternate=false;
      for(int i=0;i<7;i++)
        Motor_Array[i]=2;
      day=false;
      break;
    }
    case 3:
    {
      lcd.print("Alternate");
      day=false;
      Alternate=true;
      break;
    }
    case 4:
    {
      lcd.print("Day      ");
      day=true;
      Alternate=false;
      break;
    }
  }
}
void Stop_Motors()
{
  digitalWrite(6,HIGH);//digitalWrite(6,LOW);
  digitalWrite(7,HIGH);//digitalWrite(7,LOW);
  digitalWrite(8,HIGH);
  Store_initial();
  if(page_no==1 && Terminate1==0 && Terminate2==0)
  {
    M1=0,M2=0;
    lcd.setCursor(10,0);
    lcd.print("M1:x");
    lcd.setCursor(10,1);
    lcd.print("M2:x");
  }
}
void Store_initial()
{
  now = rtc.now();
  ut=now.secondstime();
  Reset_Time();
}
void Reset_Time()
{
  if(Motor_Num==1)
  {
    Motor1_Time_Min=0;
    diff1=0;
  }
  if(Motor_Num==2)
  {
    Motor2_Time_Min=0;
    diff2=0;
  }
}
void Initial_water()
{
  now=rtc.now();
  Initial_Water=now.secondstime();
  Current_Water_Level=water_level_tank;
  Reset_Water();
}
void Reset_Water()
{
  now=rtc.now();
  Initial_Water=now.secondstime();
  Current_Water_Level=water_level_tank;
}
void loop() 
{
  if(Terminate1 && Terminate2)
  {
    stop=true;
  }
  else
  {
    stop=false;
  }
  now = rtc.now();

  Button_State1=digitalRead(9);//next page
  Button_State2=digitalRead(10);//prev page
  if(water_level_sump<10)
  {
    Stop_Motors();
    stop=true;
  }
  else
    stop=false;
  if(Button_State1==0)
  {
    if(page_no<4)
      page_no=page_no+1;
    else
    {
      page_no=1;
    }
    Button_State1=false;
    Lcd_Clear();
  }
  else if(Button_State2==0)
  {
    if(page_no>1)
      page_no=page_no-1;
    else
    {
      page_no=4;
    }
    Button_State2=false;
    Lcd_Clear();
  }
  if(initiate2)
  {
    Initial_water();
    initiate2=false;
  }
  Current_Water=now.secondstime();
  uint32_t diff=Current_Water-Initial_Water;
  Serial.println(diff);
  if(diff>60 && water_level_sump>10)
  {
    if(Current_Water_Level==water_level_tank && Motor_off==false)
    {
      if(Motor_Num==1)
      {
        Serial.println("Terminate Motor1 Repair");
        Terminate1=true;
      }
      else if(Motor_Num==2)
      {
        Serial.println("Terminate Motor2 Repair");
        Terminate2=true;
      }
      if(Alternate)
      {
        if(Motor_Num==1 && Terminate2==0)
          Motor_Num=2;
        else if(Motor_Num==2 && Terminate1==0)
          Motor_Num=1;
      }
      //freeze=true;
    }
    initiate2=true;
    diff=0;
  }
  if(page_no==1)
  {
    get_water_level();
    Motor_Status();
    lcd.setCursor(9,0);
    lcd.print("|");
    lcd.setCursor(9,1);
    lcd.print("|");
    
    //DayPrint(Motor_Index+1);

  }
  else if(page_no==2 && Alternate==false)
  {
    get_water_level();
    Display_Heading();
    
    Button_State1=digitalRead(9);
    Button_State2=digitalRead(11);
    Button_State3=digitalRead(12);



    if(Button_State2==0)
    {
      Initial_water();
      Schedule(Motor_Day);//for motor change
      freeze=false;
      Button_State2=false;
    }

    if(Button_State3==0 && Motor_Day<7)//Day change
    {
      Motor_Day=Motor_Day+1;
      Button_State3=false;
    }
    else if(Button_State3==0 && Motor_Day>=7)
    {
      Motor_Day=1;
      Button_State3=false;
    }
    lcd.setCursor(15,1);
    lcd.print(Motor_Day);
    
    lcd.setCursor(0,1);
    for(int i=0;i<7;i++)
    {
      lcd.print(Motor_Array[i]);
      lcd.print(" ");
    }
    DayPrint(now.dayOfTheWeek()+1);
  }
  else if(page_no==2 && Alternate)
  {
    lcd.setCursor(0,0);
    lcd.print("Time Limit:");
    lcd.print(Time_Limit);
    Button_State3=digitalRead(12);
    Button_State4=digitalRead(11);
    if(Button_State3==false)
    {
      Time_Limit=Time_Limit+1;
      Button_State3=false;
    }
    if(Button_State4==false)
    {
      if(Time_Limit>1)
        Time_Limit=Time_Limit-1;
      Button_State4=false;
    }
  }
  else if(page_no==3)
  {
    get_water_level();
    lcd.setCursor(0,0);
    lcd.print("Motor Schedule:");
    lcd.setCursor(0,1);
    Button_State1=digitalRead(9);
    Button_State2=digitalRead(12);
    if(Button_State2==0)//key press
    {
      Initial_water();
      //Store_initial();
      if(Option<5)
      {
        Option=Option+1;
      }
      else
        Option=1;
    }
    Schedule_Option(Option);
  }
  get_water_level();

  now=rtc.now();
  uint32_t present;
  if(freeze==false)
    present=now.secondstime();
  if(Motor_Num==1 && Terminate1==false)
    diff1=present-ut;
  else if(Motor_Num==2 && Terminate2==false)
    diff2=present-ut;
  if(diff1>=60)
  {
    if(Motor_Num==1 && Terminate1==false)
      Motor1_Time_Min=Motor1_Time_Min+1;
    diff1=0;
    ut=present;
    Lcd_Clear();
  }
  if(diff2>=60)
  {
    if(Motor_Num==2 && Terminate2==false)
      Motor2_Time_Min=Motor2_Time_Min+1;
    diff2=0;
    ut=present;
    Lcd_Clear();
  }
  if(page_no==4)
  {
    lcd.setCursor(0,0);
    lcd.print("M1:");
    lcd.print(Motor1_Time_Min);
    lcd.print("m ");
    lcd.print(diff1);
    lcd.print("s");

    lcd.setCursor(0,1);
    lcd.print("M2:");
    lcd.print(Motor2_Time_Min);
    lcd.print("m ");
    lcd.print(diff2);
    lcd.print("s");
  }
  
  delay(500);
  if(Alternate)
  {
    if(Motor_Num==1 && Terminate2==0)
    {
      if(Motor1_Time_Min>=Time_Limit)
      {
        Motor_Num=2;
      }
    }
    else if(Motor_Num==2 && Terminate1==0)
    {
      if(Motor2_Time_Min>=Time_Limit)
      {
        Motor_Num=1;
      }
    }
    if(Motor1_Time_Min>=Time_Limit && Motor2_Time_Min>=Time_Limit)
    {
      Store_initial();
    }
  }
  if(water_level_tank>=100 && stop==0)
  {
    digitalWrite(7,HIGH);//digitalWrite(7,LOW);
    digitalWrite(8,HIGH);//digitalWrite(8,LOW);
    M1=0;
    M2=0;
    if(Terminate1)
      M1=2;
    if(Terminate2)
      M2=2;
    if(Update_Index)
    {
      if(Motor_Index<6)
        Motor_Index=Motor_Index+1;
      else
        Motor_Index=0;
      Update_Index=false;
    }
    initiate=true;
    freeze=true;
    Motor_off=true;
  }
  else if(water_level_tank<=30 && stop==0)
  {
    
    Motor_off=false;
    Update_Index=true;
    if(Alternate==false)
      Select_Motor(Motor_Index);
    freeze=false;
    if(initiate)
    {
      Store_initial();
      initiate=false;
      Lcd_Clear();
    }

    if(Motor_Num==1 && Terminate1==0)
    {
      digitalWrite(7,LOW);//digitalWrite(7,HIGH);
      digitalWrite(8,HIGH);//digitalWrite(8,LOW);
      M1=1;
      M2=0;
    }
    else if(Motor_Num==2 && Terminate2==0)
    {
      digitalWrite(7,HIGH);//digitalWrite(7,LOW);
      digitalWrite(8,LOW);//digitalWrite(8,HIGH);
      M1=0;
      M2=1;
    }
    else if(Terminate1 && Terminate2)
    {
      digitalWrite(7,HIGH);//digitalWrite(7,LOW);
      digitalWrite(8,HIGH);//digitalWrite(8,LOW);
      M1=2;
      M2=2;
    }
    else if(Terminate1)
    {
      digitalWrite(7,HIGH);//digitalWrite(7,LOW);
      M1=2;
    }
    else if(Terminate2)
    {
      digitalWrite(8,HIGH);//digitalWrite(8,LOW);
      M2=2;
    }
    else
    {
      digitalWrite(7,HIGH);//digitalWrite(7,LOW);
      digitalWrite(8,HIGH);//digitalWrite(8,LOW);
      M1=0,M2=0;
    }
    if(Terminate1)
      M1=2;
    if(Terminate2)
      M2=2;
    if(water_level_sump<=10)
    {
      M1=0,M2=0;
      digitalWrite(7,HIGH);//digitalWrite(7,LOW);
      digitalWrite(8,HIGH);//digitalWrite(8,LOW);
    }
  }
}
