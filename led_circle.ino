#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LED_PIN 6
#define MANUAL_PIN 3
#define AUTOMATIC_PIN 4
#define RED_POT A2
#define GREEN_POT A1
#define BLUE_POT A3
#define NUMPIXELS 12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

const int button_pins[] = {MANUAL_PIN, AUTOMATIC_PIN};

int button_state[ARRAY_SIZE(button_pins)];
int button_state_last[ARRAY_SIZE(button_pins)];
int debounce[ARRAY_SIZE(button_pins)];
int debounce_time = 80;

void setup() {

  Serial.begin(115200);
  pixels.begin();


  for (int i=0; i < ARRAY_SIZE(button_pins); ++i) {
    pinMode(button_pins[i], INPUT_PULLUP);
    button_state[i] = 1;
    button_state_last[i] = 1;
    debounce[i] = 0;
  }
}

// http://www.instructables.com/id/How-to-Make-Proper-Rainbow-and-Random-Colors-With-/
const uint8_t HSVlights[61] =
{0, 4, 8, 13, 17, 21, 25, 30, 34, 38, 42, 47, 51, 55, 59, 64, 68, 72, 76,
81, 85, 89, 93, 98, 102, 106, 110, 115, 119, 123, 127, 132, 136, 140, 144,
149, 153, 157, 161, 166, 170, 174, 178, 183, 187, 191, 195, 200, 204, 208,
212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255};

void trueHSV(uint8_t* rgbColour, int angle) {

  if (angle<60) {rgbColour[0] = 255; rgbColour[1] = HSVlights[angle]; rgbColour[2] = 0;} else
  if (angle<120) {rgbColour[0] = HSVlights[120-angle]; rgbColour[1] = 255; rgbColour[2] = 0;} else
  if (angle<180) {rgbColour[0] = 0, rgbColour[1] = 255; rgbColour[2] = HSVlights[angle-120];} else
  if (angle<240) {rgbColour[0] = 0, rgbColour[1] = HSVlights[240-angle]; rgbColour[2] = 255;} else
  if (angle<300) {rgbColour[0] = HSVlights[angle-240], rgbColour[1] = 0; rgbColour[2] = 255;} else
                 {rgbColour[0] = 255, rgbColour[1] = 0; rgbColour[2] = HSVlights[360-angle];}
}

void loop() {
  uint8_t rgbColour[3];
  static int angle = 0;


  //Read switch, set state
  static bool state = 0; //0 is manual, 1 automatic
  for(int i=0; i < ARRAY_SIZE(button_pins); ++i) {
    button_state[i] = digitalRead(button_pins[i]);
    unsigned long now = millis() + (unsigned long)-3000;
    if (button_state[i] != button_state_last[i] && now - debounce[i] > debounce_time) {
      button_state_last[i] = button_state[i];
      debounce[i] = now;
      if(button_state[i] == LOW) {
        if(i == 0) {
          //manual mode
          state = 0;
          Serial.println("manual button");
        } else if (i == 1) {
          //automatic mode
          state = 1;
          angle = 0;
          Serial.println("automatic button");
        }
      }
    }
  }
  if(state == 0) {
    Serial.println("manual");
    rgbColour[0] = analogRead(RED_POT)/4;
    rgbColour[1] = analogRead(GREEN_POT)/4;
    rgbColour[2] = analogRead(BLUE_POT)/4;

    if(rgbColour[0] < 3) rgbColour[0] = 0;
    if(rgbColour[1] < 3) rgbColour[1] = 0;
    if(rgbColour[2] < 3) rgbColour[2] = 0;

    Serial.print(rgbColour[0]);
    Serial.print(" ");
    Serial.print(rgbColour[1]);
    Serial.print(" ");
    Serial.print(rgbColour[2]);
    Serial.println();
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(rgbColour[0],rgbColour[1],rgbColour[2]));
    }
  } else if(state ==1) {
    Serial.println("automagic");
    trueHSV(rgbColour, angle);
    angle = (1 + angle) % 360;
    Serial.print(rgbColour[0]);
    Serial.print(" ");
    Serial.print(rgbColour[1]);
    Serial.print(" ");
    Serial.print(rgbColour[2]);
    Serial.println();
    for(int i=0;i<NUMPIXELS;i++) {
      pixels.setPixelColor(i, pixels.Color(rgbColour[0], rgbColour[1], rgbColour[2]));
    }
  }
  pixels.show();
  delay(100);
}
