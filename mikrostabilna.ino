#define outputA 3
#define outputB 2
#define outputC 4

const int strobe = 7;
const int clock = 9;
const int data = 8;

char CODE[9] = "00000000";
const int COUNT = 4;

char CODE2[9] = "";
char MAX[9] = "99999999";
long maxInteg = 99999999;
int counter = 0;
int aState;
int aLastState;
int cState;
int cLastState; 
int cPushCount=0;
int PosCounter=1;
bool state=false;
long countdownMillis = 0;
double countdownInterval = 1;
unsigned long blinkMillis = 0;
const long blinkInterval = 200;
unsigned long longClickMillis = 0;
const long longClickInterval = 1000;
bool blinkState=true;
bool menu=false;
int menuClicks=0;
 
void sendCommand(uint8_t value)
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}
 
void reset()
{
  sendCommand(0x40);
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}
 
void display(long digit,int cell)
{
  int numb=0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0+(cell-1)*2);
 
switch (digit)
{
case 0: numb= 0b00111111; break;
case 1: numb= 0b00000110; break;
case 2: numb= 0b01011011; break;
case 3: numb= 0b01001111; break;
case 4: numb= 0b01100110; break;
case 5: numb= 0b01101101; break;
case 6: numb= 0b01111101; break;
case 7: numb= 0b00000111; break;
case 8: numb= 0b01111111; break;
case 9: numb= 0b01101111; break;
case 10: numb= 0b00000000; break;
}
  shiftOut(data, clock, LSBFIRST, numb);
  digitalWrite(strobe, HIGH);
}
 
 
void display_int()
{
int i;
for(i=8-COUNT;i<8;i++){
  if(isdigit(CODE[i]))
  {
  int temp=CODE[i] - '0';
      display(temp,i+1);
     }
  else
  {
    display(10,i+1);
  }
}
}

 
void display_int2()
{
  int i;
for(i=8-COUNT;i<8;i++){
    if(isdigit(CODE2[i]))
    {
    int temp=CODE2[i] - '0';
        display(temp,i+1);
    }
    else
    {
      display(10,i+1);
    }
  }
}

long toInteg()
{
  long i,suma=0,k=1;
  for(i=8;i>8-COUNT;i--)
  {
    long temp = CODE[i-1]-'0';
    suma+= k*temp;
    k=10*k;
  }
  return suma;
}

long toIntegMax()
{
  long i,suma=0,k=1;
  for(i=8;i>8-COUNT;i--)
  {
    int temp = MAX[i-1]-'0';
    suma+= k*temp;
    k=10*k;
  }
  return suma;
}

void toChar(long num)
{
  int i=0;
  long tem;
  while(num>=1)
  {
    tem=num % 10;
    CODE[7-i]=tem+'0';
    num=(num-tem)/10;
    i++;
  }
  while(i<COUNT)
  {
    CODE[7-i]='0';
    i++;
  }
}
 
void setup()
{
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  pinMode (outputC,INPUT);
  
  aLastState = digitalRead(outputA);
  cLastState = digitalRead(outputC);
  countdownMillis = millis();
  blinkMillis = millis();
  Serial.begin(9600);
  sendCommand(0x8f);
  reset();
  sendCommand(0x44);
  maxInteg=toIntegMax();
}
 
void loop()
{
  long number;
  unsigned long currentMillis = millis();
  cState = digitalRead(outputC);
  if (cState != cLastState) //count clicks
  {
    cLastState=cState;
    if(cState==LOW)
    {
      longClickMillis=millis();
      if(!menu)
      {
        cPushCount++;
      }
      if(menu)
        menuClicks++;
    }
    else
    {
      if(millis()-longClickMillis >= longClickInterval)
      {
        menu = !menu;
        menuClicks=0;
        cPushCount=0;
      }
    }
  }
  if(menu==false && cState==HIGH && cPushCount%2==1) // start countdown
  {
    number=toInteg();
    if(number==0)
    {
       strcpy(CODE2,"PPPPPPPP");
       if(currentMillis - blinkMillis >= blinkInterval)
       {
        blinkMillis = currentMillis;
        if(blinkState){
          display_int2();
          blinkState=false;
        }
        else
        {
          display_int();
          blinkState=true;
        }
       }
    }
    else
    {
      aState = digitalRead(outputA);
      if(aState != aLastState){
      aLastState = aState; 
      if(aState==HIGH)
      {
       if(digitalRead(outputB) != aState) { 
         PosCounter++;
         if(PosCounter>=10)
          PosCounter=10;
       }
       else
       {
        PosCounter--;
        if(PosCounter<=1)
          PosCounter=1;
       }
      }
     }
     double a=(60000/maxInteg - 1000)/9;
     countdownInterval=a*PosCounter+1000-a;
     if(PosCounter==10 && COUNT>3)
        countdownInterval=100;  
    if(currentMillis - countdownMillis >= countdownInterval)
    {
      countdownMillis = currentMillis;
      if(PosCounter==10 && COUNT>3)
      {
        long c=maxInteg/600;
        number=number-c;
      }
      else
      {
        number--;
      }
      toChar(number);
    }
      display_int();
    }
  }
  if(menu==false && cPushCount%2==0)
  {
    display_int();
  }
    
  if(menu)          // menu
  {
   if(menuClicks%2==0)          // pick position
   {
   aState = digitalRead(outputA);
   if(aState != aLastState){
    aLastState = aState; 
    if(aState==HIGH)
    {
     if (digitalRead(outputB) != aState) { 
       counter++;
       if(counter>COUNT-1)
        counter=0;
     }
     else
     {
       counter--;
       if(counter<0)
        counter=COUNT-1;
     }
    }
   }
   }
    else                  // pick number
    {
     aState = digitalRead(outputA);
     if(aState != aLastState){
      aLastState = aState; 
      if(aState==HIGH)
      {
       if (digitalRead(outputB) != aState) { 
         long temp = CODE[7 - counter] - '0';
         temp--;
         if(temp<0)
          temp=9;
         CODE[7 - counter] = temp + '0';
       }
       else
       {
         long temp = CODE[7 - counter] - '0';
         temp++;
         if(temp>9)
          temp=0;
         CODE[7 - counter] = temp + '0';
       }
      }
    }
  }
  strcpy(CODE2,CODE);
     CODE2[7 - counter]='P';
     if(currentMillis - blinkMillis >= blinkInterval)
     {
      blinkMillis = currentMillis;
      if(blinkState){
        display_int2();
        blinkState=false;
      }
      else
      {
        display_int();
        blinkState=true;
      }
     }
  }
}

