#include <ESP8266WiFi.h>

#include "wifi.h"

int _wifi_last_status = WL_DISCONNECTED;
int _timer = 0;

int _wifi_set_timer(int delay) {
  _timer = millis() + delay;
}
int _wifi_check_timer() {
  if (_timer == 0) {
    return 1;
  } else if (millis() >= _timer) {
    _timer = 0;
    return 1;
  }
  return 0;
}

void wifi_setup() {
  WiFi.mode(WIFI_STA);
  Serial.println("wifi: mac = " + WiFi.macAddress());
}

int wifi_process() {
  int event = WIFI_EVENT_NONE;
  int wifi_status = WiFi.status();
  int try_connect = false;
  if (_timer == -1 && wifi_status != _wifi_last_status) {
    _timer = 0;
  }
  switch(wifi_status) {
    case WL_CONNECT_FAILED:
    case WL_CONNECTION_LOST:
    case WL_NO_SSID_AVAIL:
      if (wifi_status != _wifi_last_status) {
        Serial.println("wifi: connection error");
        Serial.println("wifi: waiting 10 seconds before trying again");
        event = WIFI_EVENT_ERROR;
        _wifi_set_timer(10000);
      }
      try_connect = true;
      break;

    case WL_DISCONNECTED:
      if (wifi_status != _wifi_last_status) {
        Serial.println("wifi: disconnected");
        event = WIFI_EVENT_ERROR;
      }
    case WL_SCAN_COMPLETED:
      try_connect = true;
      break;

    case WL_IDLE_STATUS:
      if (wifi_status != _wifi_last_status) {
        _wifi_set_timer(2000);
      }
      try_connect = true;
      break;

    case WL_CONNECTED:
      if (wifi_status != _wifi_last_status) {
        Serial.println("wifi: connected");
        Serial.println("wifi: ip = " + WiFi.localIP().toString());
        Serial.println("wifi: mask = " + WiFi.subnetMask().toString());
        Serial.println("wifi: gateway = " + WiFi.gatewayIP().toString());
        event = WIFI_EVENT_CONNECTED;
      }
      break;

    default:
      if (wifi_status != _wifi_last_status) {
        event = WIFI_EVENT_ERROR;
      } else {
        Serial.println("wifi: fatal error");
        ESP.deepSleep(0, WAKE_RF_DEFAULT); // if status persists, die
      }
      break;
  }
  if (try_connect && _wifi_check_timer()) {
    Serial.println("wifi: connecting");
    event = WIFI_EVENT_CONNECTING;
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    _timer = -1;
  }
  _wifi_last_status = wifi_status;
  return event;
}
