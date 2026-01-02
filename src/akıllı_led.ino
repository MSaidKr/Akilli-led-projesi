#include <WiFi.h>
#include "ThingSpeak.h"
#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

// ================= AYARLAR (BURAYI DOLDUR) =================
const char* ssid = "-isim-";   
const char* password = "-sifre-"; 
unsigned long myChannelNumber = ---;            // ThingSpeak Channel ID (Sayı olarak)
const char * myWriteAPIKey = "---"; // ThingSpeak Write API Key

// ================= PIN TANIMLAMALARI =================
// RGB Modül Pinleri (Sırasıyla R, G, B)
const int PIN_RED   = 13; 
const int PIN_GREEN = 12; 
const int PIN_BLUE  = 14; 

// LDR Pini (VP - Pin 36)
const int PIN_LDR = 36; 

// ================= NESNELER & DEĞİŞKENLER =================
BluetoothSerial SerialBT;
Adafruit_INA219 ina219;
WiFiClient  client;

bool otomatikMod = true;        // Başlangıç modu
unsigned long sonGonderim = 0;  // Zamanlayıcı

// ThingSpeak'e gönderilecek renk hafızası
int currentR = 0;
int currentG = 0;
int currentB = 0;

void setup() {
  Serial.begin(115200);

  // 1. Bluetooth Başlat
  SerialBT.begin("ESP32_RGB_Lamba"); 
  Serial.println("Bluetooth Aktif! Baglanabilirsiniz.");

  // 2. Pin Modları
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_LDR, INPUT);

  // 3. INA219 Başlat (Donma Korumalı)
  if (!ina219.begin()) {
    Serial.println("UYARI: INA219 Sensoru Bulunamadi! (Devam ediliyor...)");
    // while(1) sildik, kod burada donmayacak.
  } else {
    Serial.println("INA219 Sensoru Basariyla Baglandi.");
  }

  // 4. Wi-Fi ve ThingSpeak
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  
  // Açılış Testi: Sırayla Kırmızı-Yeşil-Mavi yakıp söndür
  renkAyarla(255, 0, 0); delay(500);
  renkAyarla(0, 255, 0); delay(500);
  renkAyarla(0, 0, 255); delay(500);
  renkAyarla(0, 0, 0);   // Kapat
}

void loop() {
  // --- A) BLUETOOTH KOMUTLARI ---
  if (SerialBT.available()) {
    char gelen = SerialBT.read();
    gelen = toupper(gelen); // Küçük harfi büyüt (k -> K)

    // Komut Gelirse Otomatik Moddan Çık
    if (gelen == 'K' || gelen == 'Y' || gelen == 'M' || gelen == 'S' || gelen == 'W' || gelen == 'G' || gelen == 'P' || gelen =='C' || gelen =='T' || gelen =='B' || gelen =='O' || gelen =='X') {
      otomatikMod = false; 
    }

    switch (gelen) {
      case 'A': // (A)uto
        otomatikMod = true;
        SerialBT.println("Mod: OTOMATIK (Sensor Devrede)");
        break;
      case 'K': // (K)irmizi
        renkAyarla(255, 0, 0);
        SerialBT.println("Renk: KIRMIZI");
        break;
      case 'Y': // (Y)esil
        renkAyarla(0, 255, 0);
        SerialBT.println("Renk: YESIL");
        break;
      case 'M': // (M)avi
        renkAyarla(0, 0, 255);
        SerialBT.println("Renk: MAVI");
        break;
      case 'O': // (O)Mor
        renkAyarla(255, 0, 255); // Kırmızı + Mavi
        SerialBT.println("Renk: MOR");
        break;
      case 'C': // (C)Cyan rengi
        renkAyarla(0, 255, 255);
        SerialBT.println("Renk:CYAN");
        break;
      case 'G': // (G)Gün ışığı rengi
        renkAyarla(255, 200, 100);
        SerialBT.println("Renk:GÜN IŞIĞI");
        break;
      case 'T': // (T)Turuncu rengi
        renkAyarla(255, 100, 0);
        SerialBT.println("Renk:TURUNCU");
        break;
      case 'B': // (B)Brown rengi
        renkAyarla(150, 75, 0);
        SerialBT.println("Renk:KAHVERENGİ");
        break;
      case 'P': // (P)embe rengi
        renkAyarla(255, 0, 140);
        SerialBT.println("Renk:PEMBE");
        break;
      case 'W': // (W)hite rengi
        renkAyarla(255, 255, 255);
        SerialBT.println("Renk:BEYAZ");
        break;
      case 'S': // (S)arı rengi
        renkAyarla(255, 255, 0);
        SerialBT.println("Renk:SARI");
        break;
      case 'X': // (X)
        renkAyarla(0, 0, 0);
        SerialBT.println("Lamba KAPATILDI");
        break;
    }
  }

  // --- B) OTOMATİK MOD & LDR ---
  int ldrDegeri = analogRead(PIN_LDR);

  if (otomatikMod) {
    // Eşik Değeri: Kendi odana göre 1000 sayısını değiştir.
    // LDR < 1000 ise (Karanlık) -> Beyaz Işık Yak
    if (ldrDegeri < 900) { 
      renkAyarla(200, 200, 200); // Orta parlaklıkta Beyaz
    } else {
      renkAyarla(0, 0, 0);       // Gündüzse Söndür
    }
  }

  // --- C) ENERJİ ÖLÇÜMÜ (INA219) ---
  // Sensör yoksa 0 döndürür, kod çökmez.
  float akim_mA = ina219.getCurrent_mA();
  float guc_mW = akim_mA * 3.3; // Güç = Akım * Voltaj

  // --- D) BULUTA GÖNDERME (20 Saniyede Bir) ---
  if ((millis() - sonGonderim) > 20000) {
    
    // Wi-Fi Kontrolü
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("WiFi Baglaniyor...");
      WiFi.begin(ssid, password);
      while(WiFi.status() != WL_CONNECTED){
        delay(500); // Bekle
      }
      Serial.println("Baglandi.");
    }

    // ThingSpeak Paketleme (6 Alan)
    ThingSpeak.setField(1, ldrDegeri); // Field 1: Işık
    ThingSpeak.setField(2, akim_mA);   // Field 2: Akım
    ThingSpeak.setField(3, guc_mW);    // Field 3: Güç
    ThingSpeak.setField(4, currentR);  // Field 4: Kırmızı Şiddeti
    ThingSpeak.setField(5, currentG);  // Field 5: Yeşil Şiddeti
    ThingSpeak.setField(6, currentB);  // Field 6: Mavi Şiddeti
    
    // Gönder
    int sonuc = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(sonuc == 200){
      Serial.println("--- Veriler Buluta Gonderildi ---");
      Serial.print("R: "); Serial.print(currentR);
      Serial.print(" G: "); Serial.print(currentG);
      Serial.print(" B: "); Serial.println(currentB);
    } else {
      Serial.println("Gonderim Hatasi: " + String(sonuc));
    }
    sonGonderim = millis();
  }
}

// --- YARDIMCI FONKSİYON: RENK AYARLAMA ---
void renkAyarla(int r, int g, int b) {
  // Değerleri hafızaya kaydet (ThingSpeak için)
  currentR = r;
  currentG = g;
  currentB = b;

  // LED'lere yaz (ESP32 için analogWrite kullanıyoruz)
  analogWrite(PIN_RED, r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE, b);
}
