#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PW ""
#define MQTT_BROKER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "ledTeste"

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

#define NTP_SERVER     "pool.ntp.org"

#define UTC_OFFSET     -10800

#define UTC_OFFSET_DST 0

// Cliente WiFi
WiFiClient espClient;

// Cliete MQTT
PubSubClient client(espClient);

// Função chamada quando chega uma mensagem MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    
  Serial.print("Mensagem recebida no tópico: "); // Print out the topic
  Serial.println(topic);

  Serial.print("Mensagem:"); //Print Received message
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}


void conectaWIFI(){
  delay(10);
  Serial.println();
  Serial.print("Conectando a rede ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}


void conectaMQTT(){
  client.setSocketTimeout(60);
  client.setKeepAlive(5);
  client.setServer(MQTT_BROKER, MQTT_PORT); //Start MQTT connection
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32AlexM", "", "" )) {
      Serial.println("Conectado ao broker");
    } else {
      Serial.print("Falha ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe(MQTT_TOPIC);
}

void lcd() {
  static int8_t counter = 0;
  const char* glyphs = "\xa1\xa5\xdb";
  LCD.setCursor(15, 1);
  LCD.print(glyphs[counter++]);
  if (counter == strlen(glyphs)) {
    counter = 0;
  }
}

void printdahora() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    LCD.setCursor(0, 1);
    LCD.println("Connection Err");
    return;
  }

  LCD.setCursor(8, 0);
  LCD.println(&timeinfo, "%H:%M:%S");

  LCD.setCursor(0, 1);
  LCD.println(&timeinfo, "%d/%m/%Y   %Z");
}

void setup() {
  Serial.begin(115200);

  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("Conectando... ");
  LCD.setCursor(0, 1);
  LCD.print("WiFi ");

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    lcd();
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.println("Hora:");
  LCD.setCursor(0, 1);
  LCD.println("Atualizando hora...");

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  printdahora();
  delay(250);
}
