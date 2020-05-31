/*
 * PB0 - on or of all tubes
 * ADC6 - blank -> NOK!!!!! -> but it is relevant?
 * PB5 - CLK
 * PB2 - Load
 * PD7 - DIN
 * 
 * OUT0 - A
 * 
 * OUT8 - grid 0
 * OUT9 - grid 1
 * OUT10 - grid 2
 * OUT11 - grid 3
 * OUT12 - grid 4
 * OUT13 - grid 5
 */
#define CLK PB5
#define LOAD PB2
#define DIN PD7

int a[20] = {1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1};
 
int k = 0;

void setup() {
  DDRB = 0b00000001; // PB0 is used
  pinMode(DIN, OUTPUT);
  pinMode(LOAD, OUTPUT);
  pinMode(CLK, OUTPUT);
  digitalWrite(CLK, LOW);
  digitalWrite(LOAD, LOW);
  digitalWrite(DIN, LOW);

}

void loop() {
  
  DDRB |= (1 << PB0);
  delay(1000);                       // wait for a second
  DDRB &= ~(1 << PB0);
delay(1000); 
#if 0 
  for (int i = 0; i < 20; i++) {
    if (i == k) digitalWrite(DIN, 1);
    else  digitalWrite(DIN, 0);
    digitalWrite(CLK, HIGH);
    //delay(10);
    digitalWrite(CLK, LOW);
    //delay(10);
  }
  digitalWrite(CLK, LOW);
 
  for (int i = 19; i >= 0; i--) {
    digitalWrite(DIN, a[i]);
    digitalWrite(CLK, HIGH);
    //delay(10);
    digitalWrite(CLK, LOW);
    //delay(10);
  }
 
  digitalWrite(LOAD, HIGH);
  //delay(10);
  digitalWrite(LOAD, LOW);
 
  delay(100);
 
  k = k + 1;
  if (k > 19) k = 0;
  #endif
}
