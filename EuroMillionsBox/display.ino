#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(0);
int _display_auto_flush = 1;

void display_setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void display_draw_text(int s, int x, int y, const char *text) {
  display.setTextSize(s);
  display.setCursor(x, y);
  display.println(text);
  if (_display_auto_flush) {
    display.display();
  }
}

void display_flush() {
  display.display();
  _display_auto_flush = 1;
}

void display_clear(int auto_flush) {
  _display_auto_flush = auto_flush;
  display.clearDisplay();
  if (_display_auto_flush) {
    display.display();
  }
}

void display_draw_home_screen(const char *status) {
  display_clear(false);
  display_draw_text(2, 10, 0, I18N_TITLE_FIRST_LINE);
  display_draw_text(2, 128 - 10 - strlen(I18N_TITLE_SECOND_LINE)*12, 16, I18N_TITLE_SECOND_LINE);
  display_draw_text(1, 0, 40, status);
  display_flush();
}
