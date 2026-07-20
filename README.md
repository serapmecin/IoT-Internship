# ESP32 + RAK3172 LoRa P2P Mesajlaşma Sistemi

İki adet ESP32 (FireBeetle-ESP32) kartı, üzerlerine bağlı RAK3172 LoRa modülleri
üzerinden P2P (point-to-point) modda haberleşerek düz metin mesajlaşması yapar.
Konuşma kaydı bir Python script ile txt dosyasına loglanır.

## Proje Yapısı

- `sol1` — Her iki ESP32 kartına da yüklenen Arduino kodu. Serial
  Monitor'e yazılan metni otomatik olarak hex'e çevirip LoRa üzerinden gönderir,
  gelen hex veriyi çözüp okunabilir metin olarak gösterir.
- `serial_logger.py` — Bilgisayarda çalışan, seçilen COM portunu dinleyip
  gönderilen/alınan mesajları zaman damgasıyla `sohbet_gecmisi.txt` dosyasına
  kaydeden Python script.

## Donanım

- 2x ESP32 (FireBeetle-ESP32)
- 2x RAK3172 LoRa modülü (UART üzerinden ESP32'ye bağlı: RX=16, TX=17)

## Kurulum ve Çalıştırma

1. `sol1` dosyasını her iki ESP32 kartına da Arduino IDE ile yükleyin.
2. Her iki tarafta da Serial Monitor'den (115200 baud, Newline) şu komutları girin:
   ```
   AT+NWM=0
   AT+P2P=868100000:7:125:0:8:14
   AT+PRECV=65535
   ```
3. Konuşmayı loglamak istediğiniz taraf için Arduino IDE Serial Monitor'ü kapatın.
4. Bilgisayarda:
   ```
   pip install pyserial
   python serial_logger.py
   ```
   (Script içindeki `PORT` değişkenini kendi COM numaranıza göre ayarlayın.)
5. Konuşma `sohbet_gecmisi.txt` dosyasına otomatik kaydedilir.

## Notlar

- Aynı anda bir COM portuna hem Arduino IDE hem Python script bağlanamaz.
- LoRa P2P parametreleri (frekans, SF, BW, CR) her iki tarafta da birebir aynı olmalıdır.
