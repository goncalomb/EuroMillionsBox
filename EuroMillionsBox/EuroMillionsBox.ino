#include <ESP8266HTTPClient.h>

#include "button.h"
#include "wifi.h"

#define SERVER_URL "http://SOME_SERVER.example.com/euromillions.php"
#define TITLE_FIRST_LINE "Euro"
#define TITLE_SECOND_LINE "Milhoes"

bool _displaying_results = 0;

void draw_home_screen(const char *status) {
  display_clear(false);
  display_draw_text(2, 10, 0, TITLE_FIRST_LINE);
  display_draw_text(2, 128 - 10 - strlen(TITLE_SECOND_LINE)*12, 16, TITLE_SECOND_LINE);
  display_draw_text(1, 0, 40, status);
  display_flush();
}

void get_results() {
  Serial.println("request: start");
  HTTPClient http;
  http.setTimeout(15000);
  http.begin(SERVER_URL);
  int code = http.GET();
  if (code < 0) {
    Serial.println("request: error");
  } else if (code != HTTP_CODE_OK) {
    Serial.println("request: error (code != 200)");
  } else {
    Serial.println("request: ok");
    String result = http.getString();
    result.trim();
    Serial.println("request: result = \"" + result + "\"");
    int x = result.indexOf("|");
    int y = result.indexOf("|", x + 1);
    String date = result.substring(0, x);
    String n = result.substring(x + 1, y);
    String s = result.substring(y + 1);
    display_clear(false);
    display_draw_text(2, 4, 0, date.c_str());
    display_draw_text(2, 4, 16, "N:");
    display_draw_text(2, 28, 16, (n.substring(0, 8)).c_str());
    display_draw_text(2, 28, 32, n.substring(9).c_str());
    display_draw_text(2, 4, 48, "E:");
    display_draw_text(2, 28, 48, s.c_str());
    display_flush();
  }
}

void setup() {
  Serial.begin(74880);
  Serial.println("     ");
  display_setup();
  draw_home_screen("");
  button_setup();
  wifi_setup();
}

void loop() {
  int button_state = button_get_state();

  switch (wifi_process()) {
    case WIFI_EVENT_CONNECTING:
      draw_home_screen("A ligar...");
      break;
    case WIFI_EVENT_ERROR:
      draw_home_screen("Erro WIFI!");
      break;
  }

  if (wifi_ok()) {
    if (_displaying_results) {
      if (button_state == BUTTON_STATE_LONG_PRESS) {
        _displaying_results = 0; // ask for results again
      }
    } else {
      draw_home_screen("A obter dados...");
      get_results();
      _displaying_results = 1;
    }
  } else if (_displaying_results) {
    _displaying_results = 0;
  }
}
