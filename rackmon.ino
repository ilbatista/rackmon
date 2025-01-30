#include "PMS.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "RTClib.h"
#include "DHTesp.h"
#include "ThingSpeak.h"
#include "UniversalTelegramBot.h"
#include "Defines.h"

// Constantes
const int led = 2;
const unsigned long tempomedioscan = 1000;

// Variáveis
unsigned long ultimoscan;
int statusled = 0;

// Objetos
WiFiClientSecure clienteseguro;
UniversalTelegramBot bot(tokenbot, clienteseguro);

void setup(){
  Serial.begin(115200);
  delay(500);
  
  Serial.print("Conectando à rede ");
  Serial.print(redewifi);
  WiFi.begin(redewifi, senhawifi);
  clienteseguro.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nWi-Fi conectado. Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  if (millis() - ultimoscan > tempomedioscan){
    int novasMsgs = bot.getUpdates(bot.last_message_received + 1);

    while (novasMsgs){
      Serial.println("Recebeu comandos.");
      tratarMensagens(novasMsgs);
      novasMsgs = bot.getUpdates(bot.last_message_received + 1);
    }

    ultimoscan = millis();
  }
}

void tratarMensagens(int novasMsgs){
  Serial.print("Tratando novas mensagens: ");
  Serial.println(String(novasMsgs));

  for (int i = 0; i < novasMsgs; i++){
    String idchat = bot.messages[i].chat_id;
    String texto = bot.messages[i].text;

    String remetente = bot.messages[i].from_name;
    if (remetente == "")
      remetente = "Convidado";

    if (texto == "/liga"){
      statusled = 1;
      digitalWrite(led, statusled);
      bot.sendMessage(idchat, "LED ligado", "");
    }

    if (texto == "/desliga"){
      statusled = 0;
      digitalWrite(led, statusled);
      bot.sendMessage(idchat, "LED desligado", "");
    }

    if (texto == "/status"){
      if (statusled){
        bot.sendMessage(idchat, "LED ligado", "");
      }
      else{
        bot.sendMessage(idchat, "LED desligado", "");
      }
    }

    if (texto == "/opcoes"){
      String keyboardJson = "[[\"/liga\", \"/desliga\"],[\"/status\"]]";
      bot.sendMessageWithReplyKeyboard(idchat, "Escolha uma das opções", "", keyboardJson, true);
    }

    if (texto == "/start"){
      String mensagem = "Bem-vindo ao bot do RackMon, " + remetente + ".\n";
      mensagem += "Estes são os comandos disponíveis.\n\n";
      mensagem += "/liga : Liga o LED\n";
      mensagem += "/desliga : Desliga o LED\n";
      mensagem += "/status : Retorna o status do LED\n";
      mensagem += "/opcoes : Retorna a lista de opções\n";
      bot.sendMessage(idchat, mensagem, "Markdown");
    }
  }
}