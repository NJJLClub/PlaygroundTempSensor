#include <Adafruit_CircuitPlayground.h>

#define RAISE_LIGHT_BY 2
#define LOWER_LIGHT_BY 2
#define TRIGGER_LIGHT 120
#define TRIGGER_SOUND_HI 350
#define TRIGGER_SOUND_LO 320

const int brightness = 30;

uint8_t blinkoff=0;
int  tempAdjust = -5;  // calibrated based on my household thermostat
int delayCount = 0;
bool lastLeftBtn = false;
bool lastRightBtn = false;


void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(brightness);  
  CircuitPlayground.clearPixels();
  CircuitPlayground.redLED(false);
 
}

void loop() {
 
  float temp =  CircuitPlayground.temperatureF()  ;		// grab the temperature value from the temp sensor
  uint8_t leftBtn = CircuitPlayground.leftButton();		// adjust temp value by -1 degree
  uint8_t rightBtn = CircuitPlayground.rightButton();	// adjust temp value by +1 degree
  uint16_t soundValue = CircuitPlayground.soundSensor();
  uint16_t lightValue = CircuitPlayground.lightSensor(); // between 0 and 1023 
  uint16_t red,green,blue;		// RGB value that indicates temperature range
  uint16_t ured,ugreen,ublue;   // actual RGB value that gets used
  bool     blink = false;		// start blinking the entire array of neoPixels
  uint16_t digitTemp = 0;
  uint16_t utemp;				// singles digit


  /*
  ** Adjust the sensor value up or down to help calibrate it
  */
  
  if ( leftBtn && !lastLeftBtn ) {
    tempAdjust--;
    delayCount=0;
  } else if ( rightBtn && !lastRightBtn) {
    tempAdjust++;
    delayCount=0;
  }
  temp += tempAdjust;

  //
  // check to see if a person wants to override the 10 second sleep mode
  //
  if ( delayCount > 5000) // if we are in low-power mode (ie - turn off all the neoPixels)
  {
      if ( lightValue> TRIGGER_LIGHT || soundValue>TRIGGER_SOUND_HI || soundValue < TRIGGER_SOUND_LO ) {
          delayCount = 0;  // this re-enables the temp lights again
      }
  }


  //
  // You can calibrate the temp reading by using either the LEFT button to decrease the value
  // or the RIGHT button to increase the value. Just look at an accurate temp gauge and 
  // adjust yours to match it
  //
  lastLeftBtn = leftBtn;
  lastRightBtn = rightBtn;

  //
  // Each temp range has it's own color
  // < 60 is blinking blue - all lights blink
  // < 70 is solid blue
  // < 80 is purple
  // < 90 is red
  // > 89 is blinking red - all lights blink
  //
  if ( temp < 60.0 )
  {
    red = 0;  green = 0;  blue = 255;
    digitTemp = 9;   // turn on all neoPixels
    blink = true;
  } 
  else if ( temp < 70.0 ) 
  {
     // 60's  60s = BLUE
     red = 0;   green = 0;  blue = 100;
     blink = false;
     blinkoff = false;
     utemp = (uint16_t) temp;
     digitTemp = utemp % 60;
  } 
  else if ( temp < 80.0 )
  {
     // 70's  70s = PURPLE
     red = 100;  green = 0;  blue = 100;
     blink = false;
     blinkoff = false;
     utemp = (uint16_t) temp;
     digitTemp = utemp % 70;
  }
  else if ( temp < 90.0 )
  {
     // 80's  80s = RED
     red = 100;  green = 0; blue = 0;
     blink = false;
     blinkoff = false;
     utemp = (uint16_t) temp;
     digitTemp = utemp % 80;    
  }
  else
  {
    blink = true;
    digitTemp = 9; // Turn on all the neoPixels
    red = 255;  green = 0; blue = 0;
  }

  if (  blink) blinkoff = ! blinkoff;
  if ( !blink) blinkoff = false;

  if ( blink && blinkoff )
  {
      red = 0; green = 0; blue = 0;
  }

  if ( delayCount < 5000 ) 
  {

      // it's ok to show the temp now - we show it for 5 seconds, then turn it off for 10 seconds to conserve power
      
      CircuitPlayground.clearPixels(); // erase all first
      
      if ( !  ( blink && blinkoff) ) // see if we are in blinking mode or normal mode
      {
          for ( uint16_t n = 0; n <= digitTemp ; n++)
          {
            // we light up the first neo-pixel to indicate the tens unit based on colors
            
            if ( n < 1 || blink ) {
              ured = red;  ublue = blue; ugreen = green;
            }
            else
            {
              // we light up the digit units by the number of neo-pixels lit up as white/gray (ie. not the color indicator)
              
              ured = 10;   ugreen = 10;   ublue = 10;
            }        
            CircuitPlayground.setPixelColor( n, ured, ugreen, ublue);
        
          }
      }
  }
  else if ( delayCount == 5000 )
  {
     // clear the pixels once at 5000 milli seconds - to conserve power
     // leave lights off for 10 seconds and then turn them on again.
     // you can cancel the timeout by making a loud sound or by
	 // shining a light at the light sensor
     
     CircuitPlayground.clearPixels();
  }
  else if ( delayCount > 15000 )
  {
    delayCount = 0; // start showing the lights again - to show the temperature
  }
  
  delayCount += 100; // 100 milli second delay
  delay(100);
 
}

