#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, 17, 18, 19, 21, 20);

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

String read_matrix() {
  bool noActive = true;
  String s;

  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      digitalWrite(col_pins[col], LOW);
      if (!digitalRead(row_pins[row])) {
        if (!noActive)
          s += ", ";
        s += "(" + String(row + 1) + ", " + String(col + 1) + ")";
        noActive = false;
      }
      digitalWrite(col_pins[col], HIGH);
    }
  }

  return s;
}

void loop() {
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);

  String s = read_matrix();

  if (s.length() == 0)
    oled.print("no active keys");
  else
    oled.print(s);

  oled.display();

  delay(50);//Might be unnecessary
}
