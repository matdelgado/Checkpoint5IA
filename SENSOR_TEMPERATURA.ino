#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

const int DHT_PIN = 9;  // Definindo o pino do sensor DHT

DHTesp dhtSensor;  // Criação de uma instância do sensor DHT

const char* ssid = "Wokwi-GUEST";  // Nome da rede Wi-Fi
const char* password = "";           // Senha da rede Wi-Fi
const char* mqtt_server = "broker.hivemq.com";  // Endereço do servidor MQTT

WiFiClient espClient;                // Cliente WiFi
PubSubClient client(espClient);      // Cliente MQTT
unsigned long lastMsg = 0;           // Armazena o último tempo de mensagem
#define MSG_BUFFER_SIZE  (50)        // Tamanho do buffer de mensagem
float temp = 0;                      // Variável para armazenar a temperatura
float hum = 0;                       // Variável para armazenar a umidade
int value = 0;                       // Variável de controle

void setup_wifi() {
  delay(10);  // Aguarda um curto período
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);  // Define o modo do WiFi como estação
  WiFi.begin(ssid, password);  // Inicia a conexão Wi-Fi

  // Aguarda até que a conexão Wi-Fi seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Aguarda meio segundo
    Serial.print(".");  // Imprime um ponto para indicar progresso
  }

  randomSeed(micros());  // Inicializa o gerador de números aleatórios

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // Imprime o endereço IP local
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  // Imprime o tópico da mensagem
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // Imprime o payload da mensagem
  }
  Serial.println();

  // Liga o LED se o primeiro caractere for '1'
  if ((char)payload[0] == '1') {
    digitalWrite(2, LOW);   // Liga o LED (LOW ativa o LED)
  } else {
    digitalWrite(2, HIGH);  // Desliga o LED (HIGH desativa o LED)
  }
}

void reconnect() {
  // Loop até estarmos reconectados
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Cria um ID de cliente aleatório
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Tenta conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      // Uma vez conectado, publica um anúncio...
      client.publish("iotfrontier/mqtt", "iotfrontier");
      // ... e resinscreve
      client.subscribe("iotfrontier/mqtt");  // Inscreve-se no tópico
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());  // Imprime o estado da conexão
      Serial.println(" try again in 5 seconds");
      // Aguarda 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  pinMode(2, OUTPUT);     // Inicializa o pino do LED como saída
  Serial.begin(115200);   // Inicia a comunicação serial a 115200 bps
  setup_wifi();           // Configura a conexão Wi-Fi
  client.setServer(mqtt_server, 1883);  // Define o servidor MQTT
  client.setCallback(callback);  // Define a função de callback
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);  // Configura o sensor DHT
}

void loop() {
  if (!client.connected()) {  // Verifica se está conectado ao MQTT
    reconnect();  // Se não estiver, tenta reconectar
  }
  client.loop();  // Mantém o cliente MQTT ativo

  unsigned long now = millis();  // Obtém o tempo atual
  if (now - lastMsg > 2000) {    // Verifica se passaram 2 segundos
    lastMsg = now;  // Atualiza o tempo da última mensagem
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();  // Obtém dados de temperatura e umidade

    String temp = String(data.temperature, 2);  // Converte a temperatura para String
    Serial.print("Temperature: ");  // Imprime a temperatura
    Serial.println(temp);
    client.publish("iotfrontier/temperature", temp.c_str());  // Publica a temperatura no tópico MQTT
    
    String hum = String(data.humidity, 1);  // Converte a umidade para String
    Serial.print("Humidity: ");  // Imprime a umidade
    Serial.println(hum);
    client.publish("iotfrontier/humidity", hum.c_str());  // Publica a umidade no tópico MQTT
  }
}
