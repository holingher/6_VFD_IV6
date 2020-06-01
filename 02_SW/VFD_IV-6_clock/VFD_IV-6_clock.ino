//MiniCore on atmega88pa
//16MHz
//ATmega88

#include <Time.h>
#include "RTClib.h"

/*
 * ADC6 - blank -> NOK!!!!! -> but it is relevant?
 * PB5 - digital 13 - CLK
 * PB2 - digital 10 - Load
 * PD7 - digital 7 - DIN
 * 
 * 
 *    AAAA          6666
 *   F    B        2    7
 *   F    B        2    7
 *    GGGG          5555
 *   E    C        1    4
 *   E    C        1    4
 *    DDDD  DP      0000  3
 *    
 * OUT0 - D
 * OUT1 - E
 * OUT2 - F
 * OUT3 - DP
 * OUT4 - C
 * OUT5 - G
 * OUT6 - A
 * OUT7 - B
 * 
 * OUT8 - grid 0
 * OUT9 - grid 1
 * OUT10 - grid 2
 * OUT11 - grid 3
 * OUT12 - grid 4
 * OUT13 - grid 5
 */
//VFD ARDUINO PINS
#define SER_DATA_PIN  7     // serial data pin for VFD-grids shift register
#define SER_LAT_PIN  10      // latch pin for VFD-grids shift register
#define SER_CLK_PIN  13      // clock pin for VFD-grids shift register

//int blank = TODO;
const int din = 7;
const int load = 2;
const int clk = 5;
 
byte numericalValue[10] = 
{
  //ZERO // 0  // ZERO // 
  0b11101011,//zero -> 6, 7, 4, 0, 1, 2
  //ONE // 1  // ONE //
  0b00001001,//one -> 7, 4
  //TWO // 2  // TWO //  
  0b11000111,//two -> 6, 7, 5, 1, 0
  //THREE // 3  // THREE //
  0b10001111,//three -> 6, 7, 5, 4, 0
  //FOUR // 4  // FOUR // 
  0b00101101,//four -> 2, 5, 7, 4
  //FIVE // 5  // FIVE // 
  0b10101110,//five -> 6, 2, 5, 4, 0
  //SIX // 6  // SIX // 
  0b11101110,//six -> 6, 2, 5, 1, 0, 4
  //SEVEN // 7  // SEVEN // 
  0b00001011,//seven -> 6, 7, 4
  //EIGHT // 8  // EIGHT //
  0b11101111,//eigth -> 0, 1, 2, 4, 5, 6, 7
  //NINE // 9  // NINE //
  0b10101111//nine -> 0, 2, 4, 5, 6, 7
  //DOT // .  // DOT //
  //0b00010000,//dot -> 3
  //Empty //    // empty //
  //{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}//gol
};

RTC_DS1307 RTC;
DateTime now;

void setup()
{
  RTC.begin();
  DDRB |= _BV(5);//pinMode(clk, OUTPUT);
  DDRB |= _BV(2);//pinMode(load, OUTPUT);
  DDRD |= _BV(7);//pinMode(din, OUTPUT);

  //write all on low
  PORTB &= ~(_BV(2) | _BV(5));
  PORTD &= ~(_BV(7));
  analogReference(INTERNAL); //Internal 1.1V ADC reference
  //if (RTC.lostPower())
  {
    //Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  delay(200);
}

unsigned long map_digit(int input_number)
{
  //get bits based on the input number
  unsigned long data_output = numericalValue[input_number];
  //shift with 12 position to properly output on MAX6921
  return (data_output<<12);
}
/* PB5 - digital 13 - CLK
 * PB2 - digital 10 - Load
 * PD7 - digital 7 - DIN*/

void clearVFD(void)
{
  PORTD &= ~(_BV(din));          //clear data and latch pins
  PORTB &= ~(_BV(load));
  for (byte i=0; i<20; i++)
  {            
    //once for each bit of data
    PORTB &= ~(_BV(clk));
    PORTB |= _BV(clk);  //register shifts bits on rising edge of clock
  }
  //set clock pin low
  PORTB &= ~(_BV(clk));
  PORTB |= _BV(load);          //Latch the word to update the VFD
}
  
void timetoIV6(int tube_position, int number_to_display) 
{ 
  //set elements
  unsigned long data = map_digit(number_to_display);
  //set tube
  bitSet(data, tube_position);
  //write all on low
  PORTB &= ~(_BV(load));
  PORTB &= ~(_BV(clk));
  PORTD &= ~(_BV(din));
  //Shiftin 20 bits
  for(byte i=0; i<20; i++) 
  { 
    //clockPin LOW
    PORTB &= ~(_BV(clk));
    //datapin bit to 1 OR 0 
    digitalWrite(SER_DATA_PIN,bitRead(data, i));//Send bit to register
    //Set clockPin HIGH
    PORTB |= _BV(clk);    //register shifts bits on rising edge of clock      
    PORTD &= ~(_BV(din));     //reset the data pin to prevent bleed through
  }
  PORTB &= ~(_BV(clk));
  PORTB |= _BV(load);
  delay(2); //This delay slows down the multiplexing to get get the brightest display (but too long causes flickering)
      
  clearVFD();
}
#define TUBE_HOURS_TENS 6
#define TUBE_HOURS_UNITS 7
#define TUBE_MINUTES_TENS 8
#define TUBE_MINUTES_UNITS 9
#define TUBE_SECONDS_TENS 10
#define TUBE_SECONDS_UNITS 11
void loop()
{
  // Read from the real time clock
  now = RTC.now();
  
  timetoIV6(TUBE_HOURS_TENS, now.hour()/10);
  timetoIV6(TUBE_HOURS_UNITS, now.hour()%10);

  timetoIV6(TUBE_MINUTES_TENS, now.minute()/10);
  timetoIV6(TUBE_MINUTES_UNITS, now.minute()%10);
  
  timetoIV6(TUBE_SECONDS_TENS, now.second()/10);
  timetoIV6(TUBE_SECONDS_UNITS, now.second()%10);
}
