// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 72 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5
#define BUTTON     7
Adafruit_DotStar strip = Adafruit_DotStar(
                           NUMPIXELS, DATAPIN, CLOCKPIN);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  pinMode(BUTTON, INPUT_PULLUP); // sets the digital pin 7 as input
  Serial.begin(9600);      // open the serial port at 9600 bps:
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

#define RED 0x00FF00
#define GREEN 0xFF0000
#define BLUE 0x0000FF
#define OFF 0x000000
#define WHITE 0xFFFFFF
#define YELLOW 0xFFFF00

#define MAXLED 34

#define BUTTON_DEBOUNCE_THRESHOLD 5

#define BIGBANG 0
#define LEFTBLINKER 1
#define RIGHTBLINKER 2
#define MAX_SETTING 2

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = RED;      // 'On' color (starts red)
int      left  = MAXLED, right = 0; //this is the left and right pointers for movement. Default to maximums.
int      x = 0; //general purpose counter for FOR loops
int      blinkCounter = 0; //Counter for slow blinks
int      setting = BIGBANG; //Light setting.
int      buttonDebounceCount = 0; //Counter for the the number of loops the button is zero or one



void loop() {

  //
  //This loop cycles every 50ms. That means that currently the fastest movement is every 50ms.
  //This can be changed, but the counter values will also need to be changed to preserve the proper timing.
  //

  //
  //DEBUG PRINTS
  //Every loop send some useful data across the serial port
  //
  Serial.print("Button Count:");    // prints a tab
  Serial.print(buttonDebounceCount);       // print as an ASCII-encoded decimal - same as "DEC"
  Serial.print("\n");    // prints a tab

  //
  //DEBOUNCE BUTTON
  //Each loop check the button. If it's depressed more than BUTTON_DEBOUNCE_THRESHOLD
  //then wait for the button to be let go, then increment the setting.
  //
  if ((digitalRead(BUTTON) == 0) && (buttonDebounceCount < BUTTON_DEBOUNCE_THRESHOLD))
  {
    buttonDebounceCount++;
  }

  if ((digitalRead(BUTTON) == 1) && (buttonDebounceCount >= BUTTON_DEBOUNCE_THRESHOLD))
  {
    buttonDebounceCount = 0;
    if (setting < MAX_SETTING)
    {
      setting++;
      for (x = 0; x <= MAXLED; x++)
      {
        strip.setPixelColor(x, OFF); // 'On' pixel at head
      }
    }
    else setting = 0;
  }

  //
  //SETTING: BIG BANG
  //This is a red back and forth. When they meet in the middle FLASH!
  //
  if (setting == BIGBANG)
  {

    //zero things out every loop. It's fast.
    for (x = 0; x <= MAXLED; x++)
    {
      strip.setPixelColor(x, OFF); // 'On' pixel at head
    }

    //If we are at the limit (or over) reset the right/left
    //Otherwise move one position
    if (right >= MAXLED)
    {
      right = 0;
      left = MAXLED;
    }
    else
    {
      right++;
      left--;
    }

    //If they are right in the middle... FLASH!
    if (right == MAXLED / 2)
    {
      for (x = 0; x <= MAXLED; x++)
      {
        strip.setPixelColor(x, WHITE); // 'On' pixel at head
      }
      strip.show();
    }

    //Set the Leds so they are on for the loop delay
    strip.setPixelColor(left, RED); // 'On' pixel at left
    strip.setPixelColor(right, RED); // 'On' pixel at right
    strip.show();                     // Refresh strip

  }


  //
  //LEFT BLINKER
  //

  else if (setting == LEFTBLINKER)
  {

    //First turn the blinker on when counter = 0
    if (blinkCounter == 0)
    {
      for (x = MAXLED - 7; x <= MAXLED; x++)
      {
        strip.setPixelColor(x, YELLOW); // 'On' pixel at head
      }
    }

    //Then after 10 loops, turn it off.
    if (blinkCounter == 10)
    {
      for (x = MAXLED - 7; x <= MAXLED; x++)
      {
        strip.setPixelColor(x, OFF); // 'On' pixel at head
      }
    }

    //Then after 20 loops... start over. If we don't start over, increment count.
    if (blinkCounter >= 20)
    {
      blinkCounter = 0;
    }
    else
    {
      blinkCounter++;
    }

    strip.show();
  }

  //
  //RIGHT BLINKER
  //

  else if (setting == RIGHTBLINKER)
  {

    //First turn the blinker on when counter = 0
    if (blinkCounter == 0)
    {
      for (x = 0; x <= 7; x++)
      {
        strip.setPixelColor(x, YELLOW); // 'On' pixel at head
      }
    }

    //Then after 10 loops, turn it off.
    if (blinkCounter == 10)
    {
      for (x = 0; x <= 7; x++)
      {
        strip.setPixelColor(x, OFF); // 'On' pixel at head
      }
    }

    //Then after 20 loops... start over. If we don't start over, increment count.
    if (blinkCounter >= 20)
    {
      blinkCounter = 0;
    }
    else
    {
      blinkCounter++;
    }

    strip.show();
  }

  delay(50);
}
