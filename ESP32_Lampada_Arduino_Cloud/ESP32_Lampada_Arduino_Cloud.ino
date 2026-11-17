#include <Arduino.h>
#include <DHT.h>
#include <math.h>
#include "thingProperties.h"

constexpr uint8_t PIN_DHT = 4;
constexpr uint8_t PIN_LDR = 5;
constexpr uint8_t PIN_POT = 6;
constexpr uint8_t PIN_RED = 7;
constexpr uint8_t PIN_GREEN = 8;
constexpr uint8_t PIN_BLUE = 9;
constexpr uint8_t PIN_BUZZER = 10;
constexpr uint8_t PIN_BUTTON = 11;

constexpr bool LED_ANODO_COMUM = false;
constexpr bool LDR_CRESCE_COM_LUZ = true;

constexpr uint32_t INTERVALO_SENSORES_MS = 2500;
constexpr uint32_t INTERVALO_SERIAL_MS = 3000;

DHT sensorTemperatura(PIN_DHT, DHT11);

volatile bool interrupcaoBotao = false;

bool temperaturaAtiva = true;
bool luminosidadeAtiva = true;
bool buzzerAtivo = true;
bool luzAcesa = true;
bool alarmeAtivo = false;
bool buzzerAnterior = false;

float temperaturaAtual = NAN;
float umidadeAtual = NAN;
float luminosidadeAtual = 0;
float potenciometroAtual = 0;

uint32_t ultimoSensorMs = 0;
uint32_t ultimoSerialMs = 0;
uint32_t ultimoBotaoMs = 0;

void IRAM_ATTR detectarBotao() {
  interrupcaoBotao = true;
}

float lerPercentual(uint8_t pino) {
  return constrain(
    analogRead(pino) * 100.0f / 4095.0f,
    0.0f,
    100.0f
  );
}

void controlarRGB(
  uint8_t vermelho,
  uint8_t verde,
  uint8_t azul
) {
  analogWrite(
    PIN_RED,
    LED_ANODO_COMUM ? 255 - vermelho : vermelho
  );

  analogWrite(
    PIN_GREEN,
    LED_ANODO_COMUM ? 255 - verde : verde
  );

  analogWrite(
    PIN_BLUE,
    LED_ANODO_COMUM ? 255 - azul : azul
  );
}

void coletarSensores() {
  if (temperaturaAtiva) {
    temperaturaAtual = sensorTemperatura.readTemperature();
    umidadeAtual = sensorTemperatura.readHumidity();
  } else {
    temperaturaAtual = NAN;
    umidadeAtual = NAN;
  }

  if (luminosidadeAtiva) {
    float leituraLdr = lerPercentual(PIN_LDR);

    luminosidadeAtual = LDR_CRESCE_COM_LUZ
      ? leituraLdr
      : 100.0f - leituraLdr;
  }

  potenciometroAtual = lerPercentual(PIN_POT);

  temperaturaC = isfinite(temperaturaAtual)
    ? temperaturaAtual
    : 0.0f;

  luminosidadePct = luminosidadeAtual;
}

void processarBotao() {
  if (!interrupcaoBotao) {
    return;
  }

  noInterrupts();
  interrupcaoBotao = false;
  interrupts();

  uint32_t agora = millis();

  if (
    agora - ultimoBotaoMs >= 220 &&
    digitalRead(PIN_BUTTON) == LOW
  ) {
    lampadaLigada = !lampadaLigada;
    ultimoBotaoMs = agora;

    atualizarSaidas();

    Serial.println(
      lampadaLigada
        ? "Lampada ligada pelo botao"
        : "Lampada desligada pelo botao"
    );
  }
}

void calcularCor(
  uint8_t &vermelho,
  uint8_t &verde,
  uint8_t &azul
) {
  vermelho = 0;
  verde = 0;
  azul = 0;

  switch (comandoCor) {
    case 1:
      vermelho = 255;
      return;

    case 2:
      vermelho = 255;
      verde = 200;
      return;

    case 3:
      azul = 255;
      return;
  }

  float porcentagem = potenciometroAtual;

  if (porcentagem < 25.0f) {
    vermelho = 255;
    verde = (uint8_t)(porcentagem * 5.0f);
  } else if (porcentagem < 50.0f) {
    float t = (porcentagem - 25.0f) / 25.0f;

    vermelho = 255;
    verde = (uint8_t)(125.0f + t * 130.0f);
    azul = (uint8_t)(t * 255.0f);
  } else if (porcentagem < 75.0f) {
    float t = (porcentagem - 50.0f) / 25.0f;

    vermelho = (uint8_t)((1.0f - t) * 255.0f);
    verde = 255;
    azul = 255;
  } else {
    float t = (porcentagem - 75.0f) / 25.0f;

    verde = (uint8_t)((1.0f - t) * 255.0f);
    azul = 255;
  }
}

void atualizarSaidas() {
  luzAcesa = lampadaLigada;

  bool temperaturaPerigosa =
    temperaturaAtiva &&
    isfinite(temperaturaAtual) &&
    (
      temperaturaAtual < 0.0f ||
      temperaturaAtual > 25.0f
    );

  alarmeAtivo =
    lampadaLigada &&
    buzzerAtivo &&
    temperaturaPerigosa;

  if (alarmeAtivo != buzzerAnterior) {
    if (alarmeAtivo) {
      tone(PIN_BUZZER, 1800);
    } else {
      noTone(PIN_BUZZER);
    }

    buzzerAnterior = alarmeAtivo;
  }

  if (!luzAcesa) {
    controlarRGB(0, 0, 0);
    return;
  }

  uint8_t vermelho;
  uint8_t verde;
  uint8_t azul;

  calcularCor(vermelho, verde, azul);

  controlarRGB(vermelho, verde, azul);
}

void onLampadaLigadaChange() {
  atualizarSaidas();

  Serial.println(
    lampadaLigada
      ? "Arduino Cloud: Lampada LIGADA"
      : "Arduino Cloud: Lampada DESLIGADA"
  );
}

void onComandoCorChange() {
  atualizarSaidas();

  Serial.print("Cor selecionada: ");

  switch (comandoCor) {
    case 0:
      Serial.println("Potenciometro");
      break;

    case 1:
      Serial.println("Vermelho");
      break;

    case 2:
      Serial.println("Amarelo");
      break;

    case 3:
      Serial.println("Azul");
      break;

    default:
      Serial.println("Valor desconhecido");
      break;
  }
}

void onComandoModoChange() {
  switch (comandoModo) {
    case 1:
      temperaturaAtiva = false;
      Serial.println("Sensor de temperatura desativado");
      break;

    case 2:
      temperaturaAtiva = true;
      Serial.println("Sensor de temperatura ativado");
      break;

    case 3:
      luminosidadeAtiva = false;
      Serial.println("Detector de luz desativado");
      break;

    case 4:
      luminosidadeAtiva = true;
      Serial.println("Detector de luz ativado");
      break;

    case 5:
      buzzerAtivo = false;
      Serial.println("Buzzer desativado");
      break;

    case 6:
      buzzerAtivo = true;
      Serial.println("Buzzer ativado");
      break;
  }

  atualizarSaidas();

  Serial.printf(
    "Modo selecionado no Arduino Cloud: %d\n",
    comandoModo
  );
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  analogReadResolution(12);

  sensorTemperatura.begin();

  lampadaLigada = true;

  controlarRGB(255, 0, 0);

  attachInterrupt(
    digitalPinToInterrupt(PIN_BUTTON),
    detectarBotao,
    FALLING
  );

  initProperties();

  ArduinoCloud.begin(
    ArduinoIoTPreferredConnection
  );

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  ultimoSensorMs = millis() - INTERVALO_SENSORES_MS;
  ultimoSerialMs = millis();

  Serial.println("ESP32-S3 INICIADO");
  Serial.println("Sensor DHT22 configurado");
  Serial.println("LED RGB ligado por padrao");
  Serial.println("Controles persistentes habilitados");

  atualizarSaidas();
}

void loop() {
  ArduinoCloud.update();

  processarBotao();

  uint32_t agora = millis();

  if (
    agora - ultimoSensorMs >= INTERVALO_SENSORES_MS
  ) {
    ultimoSensorMs = agora;

    coletarSensores();
  }

  atualizarSaidas();

  if (
    agora - ultimoSerialMs >= INTERVALO_SERIAL_MS
  ) {
    ultimoSerialMs = agora;

    Serial.println();
    Serial.println("===== ESP32-S3 =====");

    Serial.printf(
      "LED RGB: %s\n",
      luzAcesa ? "LIGADO" : "DESLIGADO"
    );

    Serial.printf(
      "Cor selecionada: %d\n",
      comandoCor
    );

    Serial.printf(
      "Modo selecionado: %d\n",
      comandoModo
    );

    if (isfinite(temperaturaAtual)) {
      Serial.printf(
        "Temperatura: %.1f C\n",
        temperaturaAtual
      );
    } else {
      Serial.println("Temperatura: leitura indisponivel");
    }

    if (isfinite(umidadeAtual)) {
      Serial.printf(
        "Umidade: %.1f%%\n",
        umidadeAtual
      );
    } else {
      Serial.println("Umidade: leitura indisponivel");
    }

    Serial.printf(
      "Luminosidade: %.1f%%\n",
      luminosidadeAtual
    );

    Serial.printf(
      "Potenciometro: %.1f%%\n",
      potenciometroAtual
    );

    Serial.printf(
      "Buzzer: %s\n",
      alarmeAtivo ? "LIGADO" : "DESLIGADO"
    );

    Serial.printf(
      "Arduino Cloud: %s\n",
      ArduinoCloud.connected()
        ? "CONECTADO"
        : "DESCONECTADO"
    );

    Serial.println("====================");
  }

  delay(2);
}
