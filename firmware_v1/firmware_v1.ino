#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, 17, 18, 19, 21, 20);

const int NUM_ROWS = 4;
const int NUM_COLS = 12;
const int row_pins[NUM_ROWS] = {0, 1, 2, 3};
const int col_pins[NUM_COLS] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16};
const int NOP = -1;

// key[0][0] should always be NOP. Logic ignores any defined functions because this is the layer switch key.

/* example layer
  const int layer[NUM_COLS][NUM_ROWS] = {
  {NOP,NOP,NOP,NOP,NOP,NOP,  NOP,NOP,NOP,NOP,NOP,NOP},
  {NOP,NOP,NOP,NOP,NOP,NOP,  NOP,NOP,NOP,NOP,NOP,NOP},
  {NOP,NOP,NOP,NOP,NOP,NOP,  NOP,NOP,NOP,NOP,NOP,NOP},
  {NOP,NOP,NOP,NOP,NOP,NOP,  NOP,NOP,NOP,NOP,NOP,NOP}
  };
*/

const int layer0[NUM_COLS][NUM_ROWS = {
  {NOP,     KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,      KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_BACKSPACE},
  {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,      KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_ENTER},
  {NOP,     KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,      KEY_H, KEY_J, KEY_K, KEY_L, KEY_UP, KEY_PERIOD},
  {NOP,     KEY_Z, KEY_X, KEY_C, KEY_V, KEY_SPACE,  KEY_B, KEY_N, KEY_M, KEY_LEFT, KEY_DOWN, KEY_RIGHT}
};

int layer = 0;

bool new_state[NUM_ROWS][NUM_COLS] = {
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false}
};

bool old_state[NUM_ROWS][NUM_COLS] = {
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false},
  {false, false, false, false, false, false,   false, false, false, false, false, false}
}

int lev_dist(String s1, String s2) {

}

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

void read_matrix(void) {
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      digitalWrite(col_pins[col], LOW);
      new_state[row][col] = digitalRead(row_pins[row]);
      digitalWrite(col_pins[col], HIGH);
    }
  }
}

void update_state(void) {
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      old_state[row][col] = new_state[row][col];
    }
  }

  read_matrix();
}



void keyboard_logic() {
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
     
    }
  }
}

void loop() {
  oled.clearDisplay();

  update_state();

  keyboard_logic();

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);

  switch (layer) {
    case -1:
      oled.println("layer change layer");
      break;
    case 0:
      oled.println("normal layer");
      break;
    default:
      oled.println("unknown layer");
  }


  oled.display();

  delay(10);//Might be unnecessary
}
