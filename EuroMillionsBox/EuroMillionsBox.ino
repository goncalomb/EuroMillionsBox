// start configuration

#define LANG        'EN' // language for the screen text (PT or EN)
#define BUTTON_PIN  14
#define WIFI_SSID   "YOUR_WIFI_SSID_HERE"
#define WIFI_PASS   "YOUR_WIFI_PASS_HERE"
#define SERVER_URL  "http://SOME_SERVER.example.com/euromillions.php"

// end configuration

#include <ESP8266HTTPClient.h>

#include "i18n.h"
#include "button.h"
#include "wifi.h"

bool _displaying_results = 0;

void draw_home_screen(const char *status) {
  display_clear(false);
  display_draw_text(2, 10, 0, I18N_TITLE_FIRST_LINE);
  display_draw_text(2, 128 - 10 - strlen(I18N_TITLE_SECOND_LINE)*12, 16, I18N_TITLE_SECOND_LINE);
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
    draw_home_screen(I18N_DATA_ERROR);
  } else if (code != HTTP_CODE_OK) {
    Serial.println("request: error (code != 200)");
    draw_home_screen(I18N_DATA_ERROR);
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
      draw_home_screen(I18N_CONNECTING);
      break;
    case WIFI_EVENT_ERROR:
      draw_home_screen(I18N_WIFI_ERROR);
      break;
  }

  if (wifi_ok()) {
    if (_displaying_results) {
      if (button_state == BUTTON_STATE_LONG_PRESS) {
        _displaying_results = 0; // ask for results again
      }
    } else {
      draw_home_screen(I18N_FETCHING_DATA);
      get_results();
      _displaying_results = 1;
    }
  } else if (_displaying_results) {
    _displaying_results = 0;
  }
}
