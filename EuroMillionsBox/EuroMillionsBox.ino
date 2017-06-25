/*
 * Copyright (c) 2017 Gonçalo Baltazar <me@goncalomb.com>
 *
 * EuroMillionsBox is released under the terms of the MIT License.
 * See LICENSE.txt for details.
 */

// start configuration

#define LANG        'EN' // language for the screen text (PT or EN)
#define BUTTON_PIN  14
#define WIFI_SSID   "YOUR_WIFI_SSID_HERE"
#define WIFI_PASS   "YOUR_WIFI_PASS_HERE"

// I'm hosting the server script at 'stuff.goncalomb.com'.
// See 'server' directory, if you want to host it yourself.

#define SERVER_URL  "http://stuff.goncalomb.com/euromillions.php"

// end configuration

#include "i18n.h"
#include "button.h"
#include "wifi.h"

bool _displaying_results = 0;

void setup() {
  Serial.begin(74880);
  Serial.println("     ");
  display_setup();
  display_draw_home_screen("");
  button_setup();
  wifi_setup();
}

void loop() {
  int button_state = button_get_state();

  switch (wifi_process()) {
    case WIFI_EVENT_CONNECTING:
      display_draw_home_screen(I18N_CONNECTING);
      break;
    case WIFI_EVENT_ERROR:
      display_draw_home_screen(I18N_WIFI_ERROR);
      break;
  }

  if (wifi_ok()) {
    if (_displaying_results) {
      if (button_state == BUTTON_STATE_LONG_PRESS) {
        _displaying_results = 0; // ask for results again
      } else if (button_state == BUTTON_STATE_SHORT_PRESS) {
        request_next_result();
      }
    } else {
      display_draw_home_screen(I18N_FETCHING_DATA);
      request_get_results();
      _displaying_results = 1;
    }
  } else if (_displaying_results) {
    _displaying_results = 0;
  }
}
