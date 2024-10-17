// Inclui as bibliotecas necessárias para conexão WiFi e comunicação MQTT
#include <WiFi.h>
#include <PubSubClient.h>

// Definindo o SSID (nome da rede WiFi), a senha (nesse caso, está vazia) e o servidor MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

// Criando instâncias para o cliente WiFi e o cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Definindo os pinos aos quais os LEDs estão conectados
int ledRed = 2;    // Pino do LED vermelho
int ledYellow = 4;  // Pino do LED amarelo
int ledGreen = 5;   // Pino do LED verde

// Variável para armazenar o valor da umidade
float humidity = 0;  

// Definindo o tempo de piscada dos LEDs (em milissegundos)
int tempo = 1000;  

// Variáveis para controlar qual LED está aceso (inicialmente, todos desligados)
bool ledRedOn = false;
bool ledYellowOn = false;
bool ledGreenOn = false;

// Função para conectar ao WiFi
void setup_wifi() {
  delay(10);  // Pequeno atraso para garantir que tudo esteja pronto
  Serial.println();  // Pular linha no monitor serial
  Serial.print("Conectando a ");
  Serial.println(ssid);  // Exibe no serial a rede WiFi à qual está tentando se conectar

  WiFi.begin(ssid, password);  // Inicia a conexão WiFi

  // Loop que espera a conexão ser estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Espera 500ms
    Serial.print(".");  // Exibe um ponto no monitor serial enquanto espera
  }

  // Quando conectado, exibe mensagens de sucesso e o endereço IP obtido
  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função de callback chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");

  // Converte o payload recebido em uma string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Verifica se o tópico é o esperado (umidade) e converte a mensagem em um valor de float
  if (String(topic) == "iotfrontier/humidity") {
    humidity = message.toFloat();
    verificarUmidade();  // Chama a função que define qual LED piscará com base na umidade
  }
}

// Função que verifica a umidade e define qual LED deve piscar
void verificarUmidade() {
  // Se a umidade for menor que 40%, pisca o LED vermelho
  if (humidity < 40) {
    ledRedOn = true;
    ledYellowOn = false;
    ledGreenOn = false;
    Serial.println("Umidade baixa - LED vermelho piscando.");
  
  // Se a umidade estiver entre 40% e 70%, pisca o LED amarelo
  } else if (humidity < 70) {
    ledRedOn = false;
    ledYellowOn = true;
    ledGreenOn = false;
    Serial.println("Umidade moderada - LED amarelo piscando.");
  
  // Se a umidade for maior que 70%, pisca o LED verde
  } else {
    ledRedOn = false;
    ledYellowOn = false;
    ledGreenOn = true;
    Serial.println("Umidade alta - LED verde piscando.");
  }
}

// Função para piscar os LEDs com base nas variáveis de controle
void piscarLEDs() {
  // Se o LED vermelho estiver ativado, pisca ele
  if (ledRedOn) {
    digitalWrite(ledRed, HIGH);  // Acende o LED
    delay(tempo);  // Espera
    digitalWrite(ledRed, LOW);   // Apaga o LED
    delay(tempo);  // Espera novamente
  }
  
  // Se o LED amarelo estiver ativado, pisca ele
  if (ledYellowOn) {
    digitalWrite(ledYellow, HIGH);  // Acende o LED
    delay(tempo);  // Espera
    digitalWrite(ledYellow, LOW);   // Apaga o LED
    delay(tempo);  // Espera novamente
  }
  
  // Se o LED verde estiver ativado, pisca ele
  if (ledGreenOn) {
    digitalWrite(ledGreen, HIGH);  // Acende o LED
    delay(tempo);  // Espera
    digitalWrite(ledGreen, LOW);   // Apaga o LED
    delay(tempo);  // Espera novamente
  }
}

// Função para reconectar ao broker MQTT caso a conexão seja perdida
void reconnect() {
  // Enquanto não estiver conectado ao MQTT, tenta reconectar
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-";  // Gera um ID de cliente aleatório
    clientId += String(random(0xffff), HEX);

    // Se a conexão for bem-sucedida, exibe mensagem e se inscreve no tópico
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe("iotfrontier/humidity");  // Inscreve no tópico de umidade
    } else {
      // Se falhar, exibe o código de erro e tenta novamente após 5 segundos
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

// Função de setup que é executada uma vez ao iniciar
void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial com baud rate de 115200
  setup_wifi();  // Conecta ao WiFi

  // Configura o servidor MQTT e define a função de callback
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Define os pinos dos LEDs como saída
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
}

// Função de loop que é executada continuamente
void loop() {
  // Verifica se está conectado ao servidor MQTT, se não, tenta reconectar
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Mantém a conexão MQTT ativa

  piscarLEDs();  // Chama a função para piscar os LEDs de acordo com a umidade
}
