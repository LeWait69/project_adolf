#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "ASF
const char* password = "12345678";
#define BOT_TOKEN "" #token

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define N1 D6  // Пин, к которому подключен первый моторчик
#define N2 D5  // Пин, к которому подключен второй моторчик
#define N3 D4  // датчик

void setup() {
  Serial.begin(115200);
  pinMode(N1, OUTPUT);
  pinMode(N2, OUTPUT);
  pinMode(N3, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к Wi-Fi...");
  }
  Serial.println("Подключено к Wi-Fi!");

  client.setInsecure();
}

void wait(){
  while(!digitalRead(N3)){
    delay(500);
    Serial.println("Ожидание");
    Serial.println(digitalRead(N3));
    yield();
  }
}

void work(){
  int tik = 0;
  digitalWrite(N1, HIGH);
  wait();

  while(tik <= 10){
    delay(1000);
    tik += digitalRead(N3);
    Serial.println(tik);

    if (tik >= 5){
      digitalWrite(N1, LOW);
      digitalWrite(N2, HIGH);
      wait();
    } else if ((5 <= tik) && (tik >= 10)){
      digitalWrite(N2, LOW);
    }
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text; //перебор сообщений

    if (text == "/on2") {
      bot.sendMessage(chat_id, "Моторчики включены", "");
      work();
      digitalWrite(N1, LOW);
      digitalWrite(N2, LOW);
      bot.sendMessage(chat_id, "Моторчики выключены", "");
    }
  }
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1); //id сообщения

  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  } 
  delay(1000);
}
