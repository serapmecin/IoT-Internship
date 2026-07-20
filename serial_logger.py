"""
LoRa Mesajlasma Loglayici
--------------------------
Bu script, ESP32 karti Serial Monitor'de gorunen mesajlari dinler
ve sadece gercek sohbet mesajlarini (AT komutlarini/OK/+EVT gibi
teknik satirlari haric tutarak) zaman damgasiyla bir txt dosyasina
kaydeder.

Kullanmadan once:
1) Arduino IDE'deki Serial Monitor'u KAPATIN (ayni COM portunu
   ayni anda hem Arduino IDE hem bu script kullanamaz).
2) Terminal/CMD'de su komutla pyserial kutuphanesini yukleyin:
       pip install pyserial
3) Asagidaki PORT degiskenini kendi COM numaraniza gore ayarlayin.
4) Scripti calistirin:
       python serial_logger.py
5) Durdurmak icin Ctrl+C basin.
"""

import serial
import sys
from datetime import datetime

# ==================== AYARLAR ====================
PORT = "COM5"          # Kendi portunuza gore degistirin: "COM5" veya "COM6"
BAUD_RATE = 115200
LOG_FILE = "sohbet_gecmisi.txt"
# ===================================================


def main():
    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except serial.SerialException as e:
        print(f"HATA: {PORT} portuna baglanilamadi -> {e}")
        print("Ipucu: Arduino IDE Serial Monitor acik olabilir, onu kapatip tekrar deneyin.")
        sys.exit(1)

    print(f"{PORT} dinleniyor... Cikmak icin Ctrl+C")
    print(f"Kayitlar '{LOG_FILE}' dosyasina yaziliyor.\n")

    with open(LOG_FILE, "a", encoding="utf-8") as log:
        log.write(f"\n===== Oturum baslangici: {datetime.now()} =====\n")
        log.flush()

        try:
            while True:
                raw = ser.readline()
                if not raw:
                    continue

                line = raw.decode("utf-8", errors="ignore").strip()
                if not line:
                    continue

                timestamp = datetime.now().strftime("%H:%M:%S")

                if line.startswith(">> Gonderildi:"):
                    mesaj = line.replace(">> Gonderildi:", "").strip()
                    entry = f"[{timestamp}] BEN: {mesaj}"
                elif line.startswith("Gelen mesaj:"):
                    mesaj = line.replace("Gelen mesaj:", "").strip()
                    entry = f"[{timestamp}] KARSI TARAF: {mesaj}"
                else:
                    # AT komutlari, OK, +EVT gibi teknik satirlari kaydetme
                    continue

                print(entry)
                log.write(entry + "\n")
                log.flush()

        except KeyboardInterrupt:
            print("\nDurduruldu.")
            log.write(f"===== Oturum sonu: {datetime.now()} =====\n")


if __name__ == "__main__":
    main()
