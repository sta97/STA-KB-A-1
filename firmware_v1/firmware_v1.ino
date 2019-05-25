#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, 19, 20, 21, 22, 23);

const int NUM_ROWS = 4;
const int NUM_COLS = 12;
const int row_pins[NUM_ROWS] = {0, 1, 2, 3};
const int col_pins[NUM_COLS] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16};

const uint16_t NOP = 0;
const uint16_t MODE_SWITCH = 0xFFFF;
const uint16_t PAUSE_SENDING = MODE_SWITCH-1;

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
    {KEY_ESC, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,             KEY_Y, KEY_U, KEY_I, KEY_O,     KEY_P, KEY_BACKSPACE},
    {NOP, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,                 KEY_H, KEY_J, KEY_K, KEY_L,     KEY_ENTER, NOP},
    {MODIFIERKEY_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,   KEY_N, KEY_M, MODIFIERKEY_GUI,  MODIFIERKEY_ALT, MODIFIERKEY_CTRL, MODIFIERKEY_RIGHT_SHIFT},
    {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_SPACE,           NOP, KEY_6, KEY_7, KEY_8, KEY_9,     KEY_0},
  },
  {
    {NOP, KEY_QUOTE, NOP, NOP, NOP, KEY_TAB,                             NOP, NOP, KEY_EQUAL, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_DELETE},
    {NOP,  NOP, NOP, NOP,  NOP, NOP,    NOP, NOP, NOP, NOP, KEY_PRINTSCREEN, NOP},
    {NOP, NOP, NOP, NOP, NOP,        NOP, NOP, NOP, NOP, NOP, NOP},
    {KEY_PERIOD, NOP, NOP, NOP, NOP, KEY_COMMA,               NOP, NOP, NOP, KEY_HOME, KEY_PAGE_DOWN, KEY_END}
  },
  {
    {NOP, NOP, KEY_UP, NOP, NOP, NOP, NOP,                   NOP, KEY_BACKSLASH, NOP, NOP, KEY_DELETE},
    {NOP, KEY_LEFT, KEY_DOWN, KEY_RIGHT, NOP, NOP,                NOP, NOP, NOP, NOP, NOP, NOP},
    {NOP, NOP, NOP, NOP, NOP, NOP,              NOP, NOP, NOP, NOP, NOP, NOP},
    {KEY_SLASH, NOP, NOP, NOP, NOP, KEY_MINUS,       NOP, NOP, NOP, KEY_MEDIA_VOLUME_DEC, KEY_MEDIA_MUTE, KEY_MEDIA_VOLUME_INC},
  },
  {
    {KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,       KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12},
    {NOP, NOP, NOP, NOP, NOP, NOP,            NOP, NOP, NOP, NOP, NOP, NOP},
    {NOP, NOP, NOP, NOP, NOP, NOP,                NOP, NOP, NOP, NOP, NOP, NOP},
    {NOP, NOP, NOP, NOP, NOP, KEY_SEMICOLON,     NOP, NOP, MODE_SWITCH, NOP, NOP, NOP},
  }
};

unsigned layer = 0;
unsigned mode = 0;

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
  oled.clearDisplay();
  oled.display();
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
  if (new_state[1][0])
    layer = layer | 1;
  if (new_state[1][11])
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

  delay(50);
  /*
    oled.clearDisplay();

    oled.setTextSize(1);
    oled.setTextColor(WHITE);yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy
    oled.setCursor(0, 24);

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
  */
}
