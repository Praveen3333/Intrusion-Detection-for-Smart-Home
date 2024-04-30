#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 8-RS, 9-EN, 10-D4, 11-D5, 12-D6, 13-D7
#define DEBUG 0
int temp;
int hum;
int count=0;
int timer_count=0;
int interruptPin = 1;
int value = 0;
int temp_pin = 0;
int fahrenheit = 0;

int connectionId = 0;

int gas_flag = 0;
int ir_flag = 0;


SoftwareSerial esp8266(6,7);

#define gas         2
#define ir          3
#define buzzer      4
#define load        8
#define fan         9


void title(void)
{
  lcd.clear();
  lcd.print("IOT BASED SAFETY");
  lcd.setCursor(0,1);
  lcd.print("INTRUSION SYSTEM");
}

void setup() 
  {
     
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  esp8266.begin(115200);
  esp8266.println("AT+UART_DEF=9600,8,1,0,0");
  delay(2000);
  esp8266.begin(9600);
  lcd.clear();
  lcd.begin(16, 2);
  title();
  delay(3000);
  pinMode(interruptPin, INPUT);
  pinMode(gas, INPUT);
  pinMode(ir, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(load, OUTPUT);
  pinMode(fan, OUTPUT);
 //// pinMode(Servo, OUTPUT);
  digitalWrite(interruptPin,HIGH);
  noInterrupts();           // disable all interrupts
  interrupts();             // enable all interrupts
  lcd.clear();
  title();
  delay(2000);
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  timer_count++;
}


void loop() 
     {
 
     if (ir_flag==0)
     {
       lcd.setCursor(0,0);
      lcd.print("UNKNOWN_PRSN:YES");
      digitalWrite(buzzer,HIGH);
     }
     else 
     {
       lcd.setCursor(0,0);
      lcd.print("UNKNOWN_PRSN:NO ");
       digitalWrite(buzzer,LOW);
     }

      if (gas_flag==0)
     {
       lcd.setCursor(0,1);
      lcd.print("GAS_STATUS:YES  ");
      digitalWrite(buzzer,HIGH);
      digitalWrite(fan,HIGH);
      digitalWrite(load,LOW);
          
     }
     
     else 
     {
       lcd.setCursor(0,1);
      lcd.setCursor(0,1);
      lcd.print("GAS_STATUS:NO   ");
      digitalWrite(buzzer,LOW);
      digitalWrite(fan,LOW);
      digitalWrite(load,HIGH);
      
      }
     

    if(connectionId >= 0)
    {
      String webpage = "<html><meta http-equiv=\"refresh\" content=\"10\">";
      webpage +=  "<center><h1 style=\"color:red;text-align:center\">IOT BASED SAFETY INTRUSION SYSTEM</h1>";

      webpage += "<p><h2>";
      if (ir_flag == 1)
        webpage+= "UNKNOWN PERSON: NO ";
         else
        webpage+= "UNKNOWN PERSON: YES ";
         webpage += "</p></h2>";

      webpage += "<p><h2>";
      if (gas_flag == 1)
        webpage+= "GAS_STATUS: SAFE   " ;
         else
        webpage+= "GAS_STATUS: UNSAFE ";
         webpage += "</p></h2>";

      webpage += "</p></h2>";
      webpage += "</h2></p></html>";
       
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend +=webpage.length();
      cipSend +="\r\n";
      
      sendData(cipSend,1000,DEBUG);
      sendData(webpage,1000,DEBUG);
      
      String closeCommand = "AT+CIPCLOSE="; 
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";
      
      sendData(closeCommand,3000,DEBUG);
  //  }
    
    count = 0;
    timer_count = 0;
    
    //Serial.print(alrt_flag1,DEC);  Serial.print(alrt_flag2,DEC);  Serial.print(alrt_flag3,DEC);  Serial.print(alrt_flag4,DEC);  
  }
  else
  {
    lcd.setCursor(14,0);
    int t_count = 15 - (timer_count/2);
    lcd.print((t_count)/10);
    lcd.print((t_count)%10);
    if(digitalRead(interruptPin) == 0) {
      count++;
      if(count == 1) {
        timer_count = 0;
      }
      while(digitalRead(interruptPin) == 0);
      delay(300);
    }
    if(esp8266.available()) // check if the esp is sending a message 
    {
      if(esp8266.find("+IPD,"))
      {
        delay(10);
        connectionId = esp8266.read()-48;
      }
    }
  }
  if(digitalRead(ir) == LOW)
      ir_flag = 0;
       else
      ir_flag = 1;
    if(digitalRead(gas) == LOW)
      gas_flag = 0;
       else
      gas_flag = 1;
      
  }
 
    if(debug)
    {
      Serial.print(response);
    }
    return response;  }