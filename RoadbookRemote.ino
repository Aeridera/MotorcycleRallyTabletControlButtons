/*
@author Daniel Garner
@date May 2023

This file provides the functionality needed to interact with an Android
tablet, emulating a custom keyboard layout and using a physical connection to the USB-C
port on the tablet.

Note that because of the dependency on the HID-Project library and the need for HID capability,
not any PCB will work, for example, Arduino Nano Every will not work. I use ProMicro Leonardo's,
they're cheap, reliable and easy.

By providing special keystrokes to the tablet it can control functions in
the RallyMoto roadbook app to do things like scrolling the roadbook and adjusting
the trip meter.

The wiring is based on CAT5 data network cable, for no special reason other than I
have a lot of it spare and it's good quality multi-core cable. I'm using the internal
pull up resistors on the board, so all that's needed to make a "button push" event is
to short the relevant pin to ground. I've included a second ground as a spare in case
one fails after potting the board, and I've included a Vcc line so that it's possible
to use a rotary encoder, which might be a nice way to interact with the app in addition
to buttons.

The wiring colours I used are these:
GND   Orange & White
2     Orange
3     Green
4     Blue & White
5     Green & White
6     Brown & White
GND   Blue
VCC   Brown
*/

// The Keyboard Arduino header library isn't good enough, it won't allow us  to use
// the volume, so I use this one instead.
#define HID_CUSTOM_LAYOUT
#define LAYOUT_UNITED_KINGDOM
#include "HID-Project.h"

// So that I can avoid code repetition later on, define a type called button,
// and use it to store the physical pin number, the action we want it to do, and where it is.
typedef struct {
  int pin;
  KeyboardKeycode associatedKeyPressEvent;
  String location;
} button;

// Ugh, we have to use a defined array length, so set up button count here.
const int buttonCount = 4;
button buttonArray[buttonCount];

// This is the ONLY place I define what each button does and what pin it's on
// so there's only one place to update it. The names don't matter as I just put
// them in the array. Normally 2, 3, 4, 5, Up arrow, down arrow, vol up, vol down.
button A = { 2, KEY_DOWN_ARROW, "Handlebar left" };
button B = { 3, KEY_UP_ARROW, "Handlebar right" };
button C = { 4, KEY_VOLUME_UP, "Bare cable 1" };
button D = { 5, KEY_VOLUME_DOWN, "Bare cable 2" };

// This happens once when the PCB is first powered on.
void setup() {

  // Pop each button into the array.
  buttonArray[0] = A;
  buttonArray[1] = B;
  buttonArray[2] = C;
  buttonArray[3] = D;

  // Configure each pin as an input channel, and apply the pull up.
  for (button thisButton : buttonArray) {
    pinMode(thisButton.pin, INPUT_PULLUP);
  }

  // Initialise control over the keyboard:
  Keyboard.begin();

  // Start the serial bus, just for debug to the IDE console.
  Serial.begin(9600);
}

// Do all of this indefinitely after the setup has run once.
void loop() {
  // For every button we have, get the state, print some debugging to to the serial
  // output, and then for every button that is pressed, do it's associated action.
  for (button thisButton : buttonArray) {
    int state = digitalRead(thisButton.pin);

    Serial.print("State of ");
    Serial.print(thisButton.location);
    Serial.print(" is ");
    Serial.print(state);
    Serial.print(" which has associated event ");
    Serial.println(thisButton.associatedKeyPressEvent);

    // Remember I'm using INPUT_PULLUP, so when not connected the pin goes to VCC.
    // When connected by the momentary switch it goes low.
    if (state == LOW) {
      Keyboard.write(thisButton.associatedKeyPressEvent);
    }
  }

  // Have a small delay to debounce the mechanical switch and give enough time to move your thumb.
  // If this is too long the keyboard feels laggy, to short and you get multiplesss. It's in milliseconds.
  delay(250);
}
