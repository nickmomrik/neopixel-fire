// Idea from https://www.twitch.tv/adafruit/v/107941601
// Based on http://www.academictutorials.com/graphics/graphics-fire-effect.asp

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN        6
#define UNUSED_PIN 2
#define PIXEL_W    8
#define PIXEL_H    4

int fire[PIXEL_W][PIXEL_H];
int palette[256][3];

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_W * PIXEL_H, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // This is for Trinket 5V 16MHz
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  for (int i = 0; i < PIXEL_W * PIXEL_H; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }

  delay(2000);

  /*
     HSLtoRGB is used to generate palette colors
     Hue goes from 0 to 85: red to yellow
     Saturation is always the maximum: 255
     Lightness is 0..255 for x=0..128, and 255 for x=128..255
  */
  for (int x = 0; x < 256; x++) {
    hsl_to_rgb(x / 3.0, 255, min(255, x * 2), palette[x]);
  }

  randomSeed(analogRead(UNUSED_PIN));
}

void loop() {  
  // randomize the bottom row of the fire buffer
  for (int x = 0; x < PIXEL_W; x++) {
    fire[x][PIXEL_H - 1] = random(10000) % 256;
  }

  // do the fire calculations for every pixel, from top to bottom
  for (int y = 0; y < PIXEL_H - 1; y++) {
    for (int x = 0; x < PIXEL_W; x++) {
      fire[x][y] =
        ((fire[(x - 1 + PIXEL_W) % PIXEL_W][(y + 1) % PIXEL_H]
          + fire[(x) % PIXEL_W][(y + 1) % PIXEL_H]
          + fire[(x + 1) % PIXEL_W][(y + 1) % PIXEL_H]
          + fire[(x) % PIXEL_W][(y + 2) % PIXEL_H])
         * 32) / 350;
    }
  }

  for (int x = 0; x < PIXEL_W; x++) {
    for (int y = 0; y < PIXEL_H; y++) {
      pixels.setPixelColor(pixel_number(x, y), pixels.Color(palette[fire[x][y]][0], palette[fire[x][y]][1], palette[fire[x][y]][2]));
      pixels.show();
    }
  }

  delay(500);
}

int pixel_number(int x, int y) {
  return y * PIXEL_W + x;
}

// hue_to_rgb and hsl_to_rgb adapted from http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c

float hue_to_rgb(float p, float q, float t) {
  if (t < 0) {
    t += 1;
  }

  if (t > 1) {
    t -= 1;
  }

  if (t < 1.0 / 6) {
    return p + (q - p) * 6 * t;
  } else if (t < 1.0 / 2) {
    return q;
  } else if (t < 2.0 / 3) {
    return p + (q - p) * (2.0 / 3 - t) * 6;
  } else {
    return p;
  }
}

void hsl_to_rgb(float h, float s, float l, int rgb[]) {
  float r, g, b;
  h /= 255;
  s /= 255;
  l /= 255;

  if (s == 0.00) {
    r = g = b = l;
  } else {
    float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    r = hue_to_rgb(p, q, h + 1.0 / 3);
    g = hue_to_rgb(p, q, h);
    b = hue_to_rgb(p, q, h - 1.0 / 3);
  }

  rgb[0] = int(r * 255);
  rgb[1] = int(g * 255);
  rgb[2] = int(b * 255);
}


