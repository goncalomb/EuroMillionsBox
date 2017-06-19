#include <ESP8266HTTPClient.h>

String _request_result;
int _request_result_pos = -1;

void _request_draw_result() {
    int i = _request_result.indexOf("|", _request_result_pos);
    if (_request_result.substring(_request_result_pos, i) != "EM") {
      Serial.println("request: invalid data");
      display_draw_home_screen(I18N_DATA_ERROR);
      _request_result_pos = -1;
      return;
    }
    int x = _request_result.indexOf("|", i + 1);
    int y = _request_result.indexOf("|", x + 1);
    int z = _request_result.indexOf("\n", y + 1);
    String date = _request_result.substring(i + 1, x);
    String n = _request_result.substring(x + 1, y);
    String s = _request_result.substring(y + 1, z);
    display_clear(false);
    display_draw_text(2, 4, 0, date.c_str());
    display_draw_text(2, 4, 16, "N:");
    display_draw_text(2, 28, 16, (n.substring(0, 8)).c_str());
    display_draw_text(2, 28, 32, n.substring(9).c_str());
    display_draw_text(2, 4, 48, "E:");
    display_draw_text(2, 28, 48, s.c_str());
    display_flush();
}

void request_get_results() {
  _request_result_pos = -1;
  Serial.println("request: start");
  HTTPClient http;
  http.setTimeout(15000);
  http.begin(SERVER_URL);
  int code = http.GET();
  if (code < 0) {
    Serial.println("request: error");
    display_draw_home_screen(I18N_DATA_ERROR);
  } else if (code != HTTP_CODE_OK) {
    Serial.println("request: error (code != 200)");
    display_draw_home_screen(I18N_DATA_ERROR);
  } else {
    Serial.println("request: ok");
    _request_result = http.getString();
    _request_result_pos = 0;
    Serial.println("request: result = \"" + _request_result + "\"");
    _request_draw_result();
  }
}

void request_next_result() {
  if (_request_result_pos >= 0) {
    _request_result_pos = _request_result.indexOf("\n", _request_result_pos) + 1;
    if (_request_result_pos >= _request_result.length()) {
       _request_result_pos = 0;
    }
    Serial.println("request: result_pos = " + String(_request_result_pos));
    _request_draw_result();
  }
}

