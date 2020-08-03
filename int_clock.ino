#include <Arduino.h>
#include <TM1637Display.h>
#define CLK 9
#define DIO 8
TM1637Display display(CLK, DIO);

//storage variables
boolean toggle1 = 0;
const int commonPin = 2;
const int buttonPins[] = {4,5,6}; // 4 menu, 5 up, 6 down, 13 led
int D1 = 1;
int D2 = 2;
int D3 = 5;
int D4 = 9;
int Sec = 0;
int select = 1;

void setup(){
  
  //set pins as outputs
  pinMode(13, OUTPUT);
  configureCommon(); // Setup pins for interrupt
  attachInterrupt(digitalPinToInterrupt(commonPin), pressInterrupt, FALLING);
  display.setBrightness(0x0f);
  cli();//stop interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / 1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

}//end setup

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  Sec = Sec + 1;
  if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
  }
}
  
void loop(){
  //do other things here
  if (Sec == 60)
  {
    Sec = 0;
    D4 = D4 + 1;
  }
  if (D4 == 10)
  {
    D4 = 0;
    D3 = D3 + 1;
  }
  if (D3 == 6)
  {
    D3 = 0;
    D2 = D2 + 1;
  }
  if (D2 == 10)
  {
    D2 = 0;
    D1 = D1 + 1;
  }
  if ((D1 == 1) && (D2 == 3))
  {
    D1 = 0;
    D2 = 1;
  }
// Apabila berkurang terus oleh down jadi negatif
  if (D4 == -1)
  {
    D4 = 9;
    D3 = D3 - 1;
  }
  if (D3 == -1)
  {
    D3 = 5;
    D2 = D2 - 1;
  }
  if (D2 == -1)
  {
    D2 = 9;
    D1 = D1 - 1;
  }
  if (D1 == -1)
  {
    D1 = 11;
  }
  if (select == 5)
  {
    select = 1;
  }

  display.showNumberDec(D1,D2,D3,D4);
}
void pressInterrupt() { 
  configureDistinct();
  if (!digitalRead(buttonPins[0])){
    select = select + 1;
  }
  if (!digitalRead(buttonPins[1])){
     switch(select){
          case 1:
            D1 = D1 + 1;
            break;
          case 2:
            D2 = D2 + 1;
            break;
          case 3:
            D3 = D3 + 1;
            break;
          case 4:
            D4 = D4 + 1;
            break;
        }
  }
  if (!digitalRead(buttonPins[2])){
    switch(select){
          case 1:
            D1 = D1 - 1;
            break;
          case 2:
            D2 = D2 - 1;
            break;
          case 3:
            D3 = D3 - 1;
            break;
          case 4:
            D4 = D4 - 1;
            break;
    }
  }
  configureCommon();
}
void configureCommon() {
  pinMode(commonPin, INPUT_PULLUP);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], LOW);
  }
}

void configureDistinct() {
  pinMode(commonPin, OUTPUT);
  digitalWrite(commonPin, LOW);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}
