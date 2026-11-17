#pragma once
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

bool lampadaLigada = true;
int comandoCor = 0;
int comandoModo = 0;
float temperaturaC = 0;
float luminosidadePct = 0;

void onLampadaLigadaChange();
void onComandoCorChange();
void onComandoModoChange();

void initProperties() {
  ArduinoCloud.setThingId(SECRET_THING_ID);
  ArduinoCloud.setBoardId(SECRET_DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);
  ArduinoCloud.addProperty(lampadaLigada, READWRITE, ON_CHANGE, onLampadaLigadaChange);
  ArduinoCloud.addProperty(comandoCor, READWRITE, ON_CHANGE, onComandoCorChange);
  ArduinoCloud.addProperty(comandoModo, READWRITE, ON_CHANGE, onComandoModoChange);
  ArduinoCloud.addProperty(temperaturaC, READ, 5 * SECONDS, NULL);
  ArduinoCloud.addProperty(luminosidadePct, READ, 5 * SECONDS, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
