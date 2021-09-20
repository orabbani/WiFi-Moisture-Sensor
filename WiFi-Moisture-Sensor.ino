#include <ESP8266WiFi.h>

#define LED_PIN D4
#define SENSOR_PIN A0
#define SAMPLE_INTERVAL 1 // Delay between samples in minutes
#define SENSOR_ID 0001

const char* ssid = "ESPHub";
const char* password = "esphub_sensor";
const char* host = "192.168.4.1";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Starting moisture sensor");

  // WiFi begin
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    delay(250); 
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    Serial.print(".");
  }
  Serial.println(" connected");
  for(int i; i<10; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    delay(50);
  }
}

void loop() {
  String sensor_data = String(analogRead(SENSOR_PIN));  
  bool data_sent = false;

  while(!data_sent) {
    WiFiClient client;
  
    Serial.printf("\n[Connecting to %s ... ", host);
    digitalWrite(LED_PIN, LOW);
    if(client.connect(host, 80)) {
      Serial.println("connected]");
  
      Serial.println("[Sending a request]");
      
      client.println("POST /sensor/index.php HTTP/1.1");
      client.println(strcat("Host: ", host));
      client.println("User-Agent: Arduino/1.0");
      client.println("Content-Type: application/x-www-form-urlencoded;");
      client.print("Content-Length: ");
      client.println(sensor_data.length());
      client.println();
      client.println("data=" + sensor_data + "&id=" + SENSOR_ID);
  
      Serial.println("[Response:]");
      while(client.connected() || client.available()) {
        if(client.available()) {
          String line = client.readStringUntil('\n');
          Serial.println(line);
          if(line.indexOf("data_sent_ok") > 0) { data_sent = true; }
        }
      }
      client.stop();
      Serial.println("\n[Disconnected]");
    }
    else
    {
      Serial.println("connection failed!]");
      client.stop();
    }
    delay(10);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
  }
  
  ESP.deepSleep(SAMPLE_INTERVAL * 60000);
}
