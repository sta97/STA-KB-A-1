#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, 19, 20, 21, 22, 23);

const int NUM_ROWS = 4;
const int NUM_COLS = 12;
const int row_pins[NUM_ROWS] = {0, 1, 2, 3};
const int col_pins[NUM_COLS] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16};

// key[0][0] should always be 0. Logic ignores any defined functions because this is the mode switch key.
/* modes will be added later. Modes will each have their own layers and logic so for example,
  the default layer for a WoW mode could have the actionbar keys moved down from the number row for reducing strain while another layer is for typing chat messages.
  Custom logic should allow modes to change layers in whatever way would be most useful, like a chat layer toggling on and off rather than having to hold a key down.
*/

/* example layer
  const int layer[NUM_COLS][NUM_ROWS] = {
  {0,0,0,0,0,0,  0,0,0,0,0,0},
  {0,0,0,0,0,0,  0,0,0,0,0,0},
  {0,0,0,0,0,0,  0,0,0,0,0,0},
  {0,0,0,0,0,0,  0,0,0,0,0,0},
  };
*/

const uint16_t layers[][NUM_ROWS][NUM_COLS] =
{
  {
    {0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,          KEY_6, KEY_7, KEY_8, KEY_9,     KEY_0, KEY_BACKSPACE},
    {MODIFIERKEY_ALT, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,          KEY_Y, KEY_U, KEY_I, KEY_O,     KEY_P, 0},
    {MODIFIERKEY_SHIFT, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,          KEY_H, KEY_J, KEY_K, KEY_L,     KEY_UP, 0},
    {MODIFIERKEY_CTRL, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_SPACE,      KEY_B, KEY_N, KEY_M, KEY_LEFT,  KEY_DOWN, KEY_RIGHT}
  },
  {
    {0, KEY_PERIOD, 0, 0, 0, 0,                             0, 0, KEY_EQUAL, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_ENTER},
    {MODIFIERKEY_ALT, KEY_QUOTE, 0, KEY_ESC, 0, KEY_TAB,    0, 0, 0, 0, KEY_PRINTSCREEN, 0},
    {MODIFIERKEY_SHIFT, 0, 0, 0, 0, MODIFIERKEY_GUI,        0, 0, 0, 0, KEY_PAGE_UP, 0},
    {MODIFIERKEY_CTRL, 0, 0, 0, 0, KEY_COMMA,               0, 0, 0, KEY_HOME, KEY_PAGE_DOWN, KEY_END}
  },
  {
    {0, KEY_SLASH, 0, 0, 0, 0, 0,                   0, KEY_BACKSLASH, 0, 0, KEY_DELETE},
    {MODIFIERKEY_ALT, 0, 0, 0, 0, 0,                0, 0, 0, 0, 0, 0},
    {MODIFIERKEY_SHIFT, 0, 0, 0, 0, 0,              0, 0, 0, 0, KEY_MEDIA_PLAY_PAUSE, 0},
    {MODIFIERKEY_CTRL, 0, 0, 0, 0, KEY_MINUS,       0, 0, 0, KEY_MEDIA_VOLUME_DEC, KEY_MEDIA_MUTE, KEY_MEDIA_VOLUME_INC},
  },
  {
    {0, KEY_SEMICOLON, 0, 0, 0, 0,        0, 0, 0, 0, 0, 0},
    {MODIFIERKEY_ALT, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0},
    {MODIFIERKEY_SHIFT, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0},
    {MODIFIERKEY_CTRL, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0},
  }
};

unsigned layer = 0;

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
};

//might be used later for fuzzy matching layer names
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
      new_state[row][col] = !digitalRead(row_pins[row]);
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

void remove_keys() {
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (!new_state[row][col])
        for (int i = 0; i < 4; ++i) //clear everything so keys aren't stuck on from layer switch.
          Keyboard.release(layers[i][row][col]);
    }
  }
}

void add_keys() {
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (!old_state[row][col] && new_state[row][col])
        Keyboard.press(layers[layer][row][col]);
    }
  }
}

void set_layer() {
  layer = 0;
  if (new_state[1][11])
    layer = layer | 1;
  if (new_state[2][11])
    layer = layer | 2;
}

void keyboard_logic() {
  set_layer();

  //Old keys must be removed before new keys are added so there is space in array for new key presses.
  remove_keys();

  add_keys();
}

void loop() {
  update_state();

  keyboard_logic();

  Keyboard.send_now();

  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 30);

  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      const int x = row * 5;
      const int y = col * 5 + (col > 5 ? 5 : 0);
      oled.drawRect(y, x, 5, 5, WHITE);
      if (new_state[row][col])
        oled.fillRect(y + 1, x + 1, 3, 3, WHITE);
    }
  }

  oled.print("layer ");
  oled.println(layer);

  for (int i = 0; i < 6; ++i) {
    oled.print(keyboard_keys[i]);
    if (i < 5)
      oled.print(",");
  }

  oled.display();

  delay(10);//Might be unnecessary
}
