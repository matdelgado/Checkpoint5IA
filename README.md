# Projeto IoT com ESP32: Sensores de Umidade e Temperatura + LEDs e MQTT

Este projeto demonstra a integração de um ESP32 com sensores de temperatura e umidade, comunicação via protocolo MQTT, e controle de LEDs para indicar a umidade do ambiente.

## Descrição

Este projeto usa o ESP32 para monitorar a umidade e a temperatura de um ambiente usando um sensor DHT22. Os dados são enviados para um servidor MQTT, que se comunica com LEDs conectados ao ESP32 para indicar os níveis de umidade. O sistema funciona da seguinte forma:
- Umidade menor que 40%: LED vermelho pisca.
- Umidade entre 40% e 70%: LED amarelo pisca.
- Umidade maior que 70%: LED verde pisca.

O servidor MQTT usado é o broker público da HiveMQ (`broker.hivemq.com`), que recebe e distribui as mensagens MQTT.

## Arquitetura

### Componentes
1. **ESP32**: Centraliza a lógica de controle e comunicação com o servidor MQTT.
2. **Sensor DHT22**: Mede a umidade e temperatura do ambiente.
3. **LEDs**: Indicam os diferentes níveis de umidade.
4. **Broker MQTT**: Recebe as leituras do sensor e controla os LEDs.

### Diagrama

```plaintext
+-------------+        +----------------+          +-------------+
|   Sensor    |        |    ESP32       |          |  Broker     |
|  DHT22      | -----> |  Publica dados | ----->   |  MQTT       |
+-------------+        +----------------+          +-------------+
                          |                        ^    
                          |                        |
                          v                        |
                    +--------------+         +--------------+
                    | LED Verde     |         | LED Vermelho |
                    +--------------+         +--------------+
                    | LED Amarelo   |
                    +--------------+
```

## Configuração do Servidor MQTT

Este projeto usa o broker MQTT da HiveMQ, que é gratuito e público. No código, a configuração está definida como:

```cpp
const char* mqtt_server = "broker.hivemq.com";
```

### Configurações no Broker MQTT
- Broker: `broker.hivemq.com`
- Porta: `1883`
- Tópicos utilizados:
  - Publicação de umidade: `iotfrontier/humidity`
  - Publicação de temperatura: `iotfrontier/temperature`

## Funcionamento

### Código 1: Controle de LEDs com Base na Umidade
1. O ESP32 se conecta à rede WiFi e ao broker MQTT.
2. Lê a umidade do sensor e publica no tópico MQTT `iotfrontier/humidity`.
3. O LED correspondente ao valor de umidade pisca:
   - <40%: Vermelho
   - 40-70%: Amarelo
   - >70%: Verde

### Código 2: Publicação de Temperatura e Umidade
1. O ESP32 lê os valores de temperatura e umidade do DHT22.
2. Os valores são publicados nos tópicos MQTT `iotfrontier/temperature` e `iotfrontier/humidity`.

## Diagrama de Funcionamento

```plaintext
+-----------+                +--------------+               +------------+
| Sensor    | -- Umidade --> |     ESP32     | -- MQTT Msg-> | HiveMQ     |
| DHT22     | -- Temperatura>| (Controlador) |               | Broker     |
+-----------+                +--------------+               +------------+
                                  |     ^                       ^     |
                                  v     |                       |     v
                             LEDs piscam |                       | Dados via MQTT
                                         |                       |
                                    ESP32 LED                    Cliente MQTT
```
