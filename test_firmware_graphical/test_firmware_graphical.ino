#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, 19, 20, 21, 22, 23);

const int NUM_ROWS = 4;
const int NUM_COLS = 12;
const int row_pins[NUM_ROWS] = {0, 1, 2, 3};
const int col_pins[NUM_COLS] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16};

void setup_matrix() {
  for (int i = 0; i < NUM_ROWS; ++i) {
    pinMode(row_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_COLS; ++i) {
    pinMode(col_pins[i], OUTPUT);
    digitalWrite(col_pins[i], HIGH);
  }
}

void setup() {
  setup_matrix();
  oled.begin();
}

void loop() {
  oled.clearDisplay();

  int numActive = 0;

  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      digitalWrite(col_pins[col], LOW);
      const int x = row * 5;
      const int y = col * 5 + (col > 5 ? 5 : 0);
      oled.drawRect(y, x, 5, 5, WHITE);
      if (!digitalRead(row_pins[row])) {
        oled.drawPixel(y + 2, x + 2, WHITE);
        ++numActive;
      }
      digitalWrite(col_pins[col], HIGH);
    }
  }

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 30);

  switch (numActive) {
    case 0:
      oled.print("no active keys");
      break;
    case 1:
      oled.print("1 active key");
      break;
    default:
      oled.print(numActive);
      oled.print(" active keys");
  }

  oled.display();

  delay(50);//Might be unnecessary
}
