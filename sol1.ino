#include <HardwareSerial.h>

#define RX_PIN 16
#define TX_PIN 17
#define LORA_BAUD 9600

HardwareSerial LoRaSerial(2);

String rxBuffer = "";

// ---- String -> Hex ----
String toHex(String str) {
  String hex = "";
  for (int i = 0; i < str.length(); i++) {
    char buf[3];
    sprintf(buf, "%02X", (unsigned char)str[i]);
    hex += buf;
  }
  return hex;
}

// ---- Hex -> String ----
String fromHex(String hex) {
  String out = "";
  for (int i = 0; i + 1 < hex.length(); i += 2) {
    String byteStr = hex.substring(i, i + 2);
    char c = (char) strtol(byteStr.c_str(), NULL, 16);
    out += c;
  }
  return out;
}

void flushLoRaResponse(unsigned long waitMs) {
  unsigned long start = millis();
  while (millis() - start < waitMs) {
    while (LoRaSerial.available() > 0) {
      char c = LoRaSerial.read();
      if (c == '\n') {
        rxBuffer.trim();
        if (rxBuffer.length() > 0) Serial.println(rxBuffer);
        rxBuffer = "";
      } else if (c != '\r') {
        rxBuffer += c;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  LoRaSerial.begin(LORA_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(10);

  Serial.println("--- LoRa String Haberlesme Aktif ---");
  Serial.println("AT ile baslayan yazilar ham komut olarak gider.");
  Serial.println("Diger her sey otomatik hex'e cevrilip gonderilir.");
}

void loop() {
  // 1) Bilgisayardan yazi geldiyse
  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();

    if (msg.length() > 0) {
      if (msg.startsWith("AT")) {
        LoRaSerial.print(msg + "\r\n");
        Serial.println(">> AT komutu gonderildi: " + msg);
      } else {
        String hexMsg = toHex(msg);

        LoRaSerial.print("AT+PRECV=0\r\n");
        flushLoRaResponse(300);
        delay(100);

        LoRaSerial.print("AT+PSEND=" + hexMsg + "\r\n");
        Serial.println(">> Gonderildi: " + msg);
        flushLoRaResponse(1000);
        delay(100);

        LoRaSerial.print("AT+PRECV=65535\r\n");
        flushLoRaResponse(300);
      }
    }
  }

  // 2) RAK modülünden gelen veriyi satır satır oku
  while (LoRaSerial.available() > 0) {
    char c = LoRaSerial.read();

    if (c == '\n') {
      rxBuffer.trim();

      if (rxBuffer.startsWith("+EVT:") && !rxBuffer.startsWith("+EVT:RXP2P")) {
        String hexData = rxBuffer.substring(5);
        String decoded = fromHex(hexData);
        Serial.println("Gelen mesaj: " + decoded);

        // Mesaj alindiktan sonra tekrar dinleme moduna gec
        LoRaSerial.print("AT+PRECV=65535\r\n");
        flushLoRaResponse(300);

      } else if (rxBuffer.length() > 0) {
        Serial.println(rxBuffer);
      }

      rxBuffer = "";
    } else if (c != '\r') {
      rxBuffer += c;
    }
  }
} 