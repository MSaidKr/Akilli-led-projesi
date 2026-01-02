# Akilli-led-projesi
Bu proje, ev otomasyonu kapsamında geliştirilmiş bir akıllı aydınlatma sistemidir. Sistem; Manuel, Bluetooth ve Wi-Fi olmak üzere üç farklı arayüz üzerinden kontrol edilebilir. RGB LED ile dinamik renk yönetimi sağlarken, entegre akım sensörleri sayesinde enerji tüketimi hesabı yaparak Thingspeak üzerinden enerji verimliliği takibi sunar. 
## 💡 Prototip Simülasyon Ortamı

Sistemin farklı renk modlarının bir oda üzerindeki etkisi, ölçekli fiziksel model üzerinde gösterilmiştir.

<p align="center">
  <img src="Gorseller/pembe_mod_ana.jpeg" width="45%" alt="Rahatlama Modu (Magenta)">
  &nbsp; &nbsp; <img src="Gorseller/turkuaz_mod.jpeg" width="45%" alt="Okuma Modu (Cyan)">
</p>

*Solda: Rahatlama Modu, Sağda: Odaklanma/Okuma Modu*

<table>
  <tr>
    <th width="50%">🔌 Fiziksel Devre Kurulumu</th>
    <th width="50%">📱 Mobil Kontrol Terminali</th>
  </tr>
  <tr>
    <td valign="top"> <img src="Gorseller/Sistem_devresi.jpeg" width="100%" alt="Devre Kurulumu">
      <br><br>
      <p>
        İçerisindeki akım ölçer ile birlikte güç tüketimi hesaplayıp LDR sensör ile ışık seviyesini ölçerek enerji tasarrufu yapmayı planlayan proje. 
      </p>
      <p>
        Veriler <b>ThingSpeak</b>'e aktarılıp istendiği zaman uzaktan kontrol edilebilmektedir. Bağlantılar yapılırken lehimleme teknikleri kullanılmış ve kablo karmaşası kutu arkasına gizlenmiştir.
      </p>
    </td>
    
  <td valign="top">
      <div align="center">
        <img src="Gorseller/Bluetooth_terminal.jpeg" width="67%" alt="Bluetooth Ekranı">
      </div>
      <br>
      <p>
        ESP32 ile birlikte Bluetooth terminal üzerinden kontrol edilebilen sistem <b>13 farklı moda</b> sahiptir.
      </p>
      <p>
        Modlar: Otomatik mod, Kırmızı, Yeşil, Mavi, Mor, Cyan, Turuncu, Sarı, Pembe, Kahverengi, Gün ışığı, Beyaz ve Kapat.
      </p>
    </td>
  </tr>
</table>
<br> <div align="center">
  <h3>📊 Canlı Veri Takibi</h3>
  <a href="https://thingspeak.mathworks.com/channels/3187553">
    <img src="https://img.shields.io/badge/Dashboard'a_Git-ThingSpeak-F57421?style=for-the-badge&logo=thingspeak&logoColor=white" alt="ThingSpeak Linki">
  </a>
  <p><i>Veriler sistem açıkken 15 saniyede bir güncellenir.</i></p>
</div>
