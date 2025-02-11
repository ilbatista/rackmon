#pragma region Bibliotecas

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

#pragma endregion

#pragma region Variáveis e constantes

// Constantes
const int led = 2;
const unsigned long tempoMedioScan = 1000;

// Variáveis
unsigned long ultimoScan, inicioTimer, agora;
int statusLed = 0;

#pragma endregion

#pragma region Objetos

// Objetos
WiFiClientSecure clienteSeguro;
UniversalTelegramBot bot(tokenBot, clienteSeguro);

#pragma endregion

#pragma region Métodos Arduino

void setup(){
  Serial.begin(115200);
  aguardar(1);
  
  Serial.print("Conectando à rede ");
  Serial.print(redeWifi);
  WiFi.begin(redeWifi, senhaWifi);
  clienteSeguro.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    aguardar(1);
  }

  Serial.print("\nWi-Fi conectado. Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  if (millis() - ultimoScan > tempoMedioScan){
    int novasMsgs = bot.getUpdates(bot.last_message_received + 1);

    while (novasMsgs){
      Serial.println("Recebeu comandos.");
      tratarMensagens(novasMsgs);
      novasMsgs = bot.getUpdates(bot.last_message_received + 1);
    }

    ultimoScan = millis();
  }
}

#pragma endregion

#pragma region Métodos Telegram

void tratarMensagens(int novasMsgs){
  Serial.print("Tratando novas mensagens: ");
  Serial.println(String(novasMsgs));

  for (int i = 0; i < novasMsgs; i++){
    String idChat = bot.messages[i].chat_id;
    String texto = bot.messages[i].text;

    String remetente = bot.messages[i].from_name;
    if (remetente == "")
      remetente = "Convidado";

    if (texto == "/liga"){
      statusLed = 1;
      digitalWrite(led, statusLed);
      bot.sendMessage(idChat, "LED ligado", "");
    }

    if (texto == "/desliga"){
      statusLed = 0;
      digitalWrite(led, statusLed);
      bot.sendMessage(idChat, "LED desligado", "");
    }

    if (texto == "/status"){
      if (statusLed){
        bot.sendMessage(idChat, "LED ligado", "");
      }
      else{
        bot.sendMessage(idChat, "LED desligado", "");
      }
    }

    if (texto == "/opcoes"){
      String keyboardJson = "[[\"/liga\", \"/desliga\"],[\"/status\"]]";
      bot.sendMessageWithReplyKeyboard(idChat, "Escolha uma das opções", "", keyboardJson, true);
    }

    if (texto == "/start"){
      String mensagem = "Bem-vindo ao bot do RackMon, " + remetente + ".\n";
      mensagem += "Estes são os comandos disponíveis.\n\n";
      mensagem += "/liga : Liga o LED\n";
      mensagem += "/desliga : Desliga o LED\n";
      mensagem += "/status : Retorna o status do LED\n";
      mensagem += "/opcoes : Retorna a lista de opções\n";
      bot.sendMessage(idChat, mensagem, "Markdown");
    }
  }
}

#pragma endregion

#pragma region Métodos Auxiliares

void aguardar(int segundos) {
  inicioTimer, agora = millis();
  while (millis() < inicioTimer, agora + (segundos * 1000));
}

#pragma endregion