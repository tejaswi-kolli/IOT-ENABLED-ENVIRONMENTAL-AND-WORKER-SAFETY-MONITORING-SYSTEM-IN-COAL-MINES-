 #include <LiquidCrystal.h>
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 =13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#include <TinyGPS.h>
TinyGPS gps;
float flat=0, flon=0,newData;
String strs[8] = {"0", "0", "0", "0", "0", "0", "0", "0"};
int StringCount = 0;
int buz=7;
int cnt=0;
#include <SoftwareSerial.h>
SoftwareSerial myserial(A0,A1);
void read_gps()
{

  unsigned long chars;
  unsigned short sentences, failed;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (myserial.available())
    {
      char c = myserial.read();
      if (gps.encode(c)) 
        newData = true;
    }
  }

  if (newData)
  {

    unsigned long age;
    gps.f_get_position(&flat,&flon,&age);

  }
}
void setup() {
 
  lcd.begin(16,2);
  lcd.setCursor(4,0);
  lcd.print("WELCOME");
  pinMode(buz,OUTPUT);
  
  digitalWrite(buz,0);
  lcd.setCursor(0,1);
  lcd.print("helmet  MNTG");
  delay(1500);
  lcd.clear();
  Serial.begin(9600);
  myserial.begin(9600);

}
void loop() {
    
  while (Serial.available())
  {
    String rcv = Serial.readStringUntil('\n');
    StringCount = 0;
   // Serial.print(rcv);
    while (rcv.length() > 0)
    {
      int index = rcv.indexOf(',');
      if (index == -1)
      {
        strs[StringCount++] = rcv;
        break;
      }
      else
      {
        strs[StringCount++] = rcv.substring(0, index);
        rcv = rcv.substring(index + 1);
      }
    }
    
   
  }cnt++;
    lcd.clear();
   lcd.setCursor(0,0);
    lcd.print("t:"+String(strs[0]) + "  h:"+String(strs[1])+ "  L:"+String(strs[5]));
     lcd.setCursor(0,1);
    lcd.print("g:"+String(strs[2]) + " x:"+String(strs[3])+ " y:"+String(strs[4]));
    delay(200);
    int temperature = strs[0].toInt();
    int humidity = strs[1].toInt();
    int gas = strs[2].toInt();
    int xval = strs[3].toInt();
    int yval = strs[4].toInt();

if (temperature > 40 || humidity > 90 || gas > 600) {
    digitalWrite(buz, HIGH);
   send_sms(1);
    digitalWrite(buz, LOW);
} else {
    digitalWrite(buz, LOW);
}
if (cnt>10){
    Serial.print("464192,3RF0359OZDHNRQH4,0,0,project,12345678,"+String(strs[0])+","+String(strs[2])+","+String(strs[3])+","+String(strs[5])+","+String(16.7491)+","+String(80.6348)+",\n");
    cnt=0;
   }
}
void send_sms(int k)
  {
   read_gps();
    
myserial.println("Sending SMS...");
myserial.println("AT");    
delay(1000);  
myserial.println("ATE0");    
delay(1000);  
myserial.println("AT+CMGF=1");    
delay(1000);  
myserial.print("AT+CMGS=\"6281209452\"\r\n");// Replace x with mobile number
delay(1000);
if(k==1)
myserial.print("Abnormal coal mine conditions please check");
myserial.println("https://www.google.com/maps/search/?api=1&query=" + String(flat,6)+ "," + String(flon,6));
delay(500);
myserial.print(char(26));
delay(2000); 
  }
