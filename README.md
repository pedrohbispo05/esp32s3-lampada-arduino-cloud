# Lâmpada Inteligente ESP32-S3 | Arduino Cloud

Projeto acadêmico IoT para o **ESP32-S3 Dev Module**, usando Arduino Cloud para monitoramento dos sensores e controle do LED RGB.

## Recursos

- LED RGB ligado por padrão e comandado remotamente pela variável `lampadaLigada`.
- Seleção permanente de cor por `comandoCor` (`0` potenciômetro, `1` vermelho, `2` amarelo, `3` azul).
- `comandoModo` de `1` a `6` para habilitar/desabilitar sensores e buzzer.
- Leitura DHT11, fotorresistor e potenciômetro.
- Botão físico para ligar/desligar.
- Alarme sonoro de temperatura fora da faixa 0–25 °C, quando habilitado.
- Integração diretamente com Arduino Cloud, sem banco de dados e sem Docker nesta etapa.

## Hardware e pinos

| Componente | GPIO |
| --- | --- |
| DHT (DATA) | 4 |
| LDR | 5 |
| Potenciômetro | 6 |
| RGB vermelho | 7 |
| RGB verde | 8 |
| RGB azul | 9 |
| Buzzer | 10 |
| Botão | 11 |

O código presume LED RGB de **cátodo comum** e leitura analógica 0–4095. Use resistores apropriados e **3,3 V nos GPIOs** do ESP32-S3. Configure o tipo do LED e o sentido da leitura do LDR conforme o circuito.

## Arduino Cloud

1. Cadastre o ESP32-S3 como dispositivo e crie uma Thing.
2. Crie as variáveis abaixo, associadas ao dispositivo:

| Variável | Tipo | Permissão |
| --- | --- | --- |
| `lampadaLigada` | Boolean | Read & Write |
| `comandoCor` | Integer | Read & Write |
| `comandoModo` | Integer | Read & Write |
| `temperaturaC` | Float | Read Only |
| `luminosidadePct` | Float | Read Only |

3. O arquivo `thingProperties.h` já está no repositório. Copie `arduino_secrets.h.example` para `arduino_secrets.h` e preencha suas credenciais localmente, sem publicar este último arquivo.
4. Substitua apenas o `.ino` pelo arquivo disponível em `ESP32_Lampada_Arduino_Cloud/ESP32_Lampada_Arduino_Cloud.ino`.
5. Instale `ArduinoIoTCloud` (e dependências), `DHT sensor library` e `Adafruit Unified Sensor` no Arduino IDE.
6. Selecione **ESP32S3 Dev Module**, configure porta e grave o firmware.

## Comandos

**`comandoCor`**: `0` potenciômetro; `1` vermelho; `2` amarelo; `3` azul.

**`comandoModo`**: `1` desativa DHT; `2` ativa DHT; `3` desativa LDR; `4` ativa LDR; `5` desativa buzzer; `6` ativa buzzer. O valor selecionado não é zerado automaticamente.

## Segurança

Nunca publique `arduino_secrets.h`, tokens, senhas de Wi-Fi ou chaves do dispositivo. O arquivo `.gitignore` impede a inclusão de `arduino_secrets.h`. O arquivo `thingProperties.h` pode ser versionado, pois não contém chaves reais.

## Observação sobre a versão entregue

Este repositório conserva o código final recebido: `DHT sensorTemperatura(PIN_DHT, DHT11)`. Há uma mensagem de Monitor Serial que informa “Sensor DHT22 configurado”, embora o código esteja configurado como **DHT11**. Altere o log localmente se desejar; o log não afeta o sensor.

## Estado dos testes

O código foi organizado para GitHub, mas não foi compilado nem validado em uma placa física nesta entrega. O arquivo `thingProperties.h` faz parte do repositório, mas `arduino_secrets.h` não deve ser publicado. A configuração de exemplo pode ser versionada.
