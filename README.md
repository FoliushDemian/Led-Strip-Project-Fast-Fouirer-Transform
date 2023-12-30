# Led strip project with presence sensor and microphone
![ESP32-WROOM-32U](https://images.prom.ua/4578430808_w640_h640_esp32-wroom-32u-bluetooth-wifi.jpg)
## Microphone module which i use:
![inmp441](https://www.faranux.com/wp-content/uploads/2023/04/i2s.jpg)
## Presence sensor which i use:
![HLK-LD2410C](https://art.mysku-st.net/uploads/arts/07/83/19/2023/01/19/52dd15.png)
## Seven segment display with common cathode
![5161AS](https://radiokomponent.com.ua/wa-data/public/shop/products/32/36/13632/images/15712/15712.750x0@2x.jpg)
## Description
This device works in two modes: presence detection mode and response mode to different sound frequencies.
+ If it works in the first mode, then it turns on the LED strip only when it detects movement, also in this mode you can adjust one of the 9 modes of the strip's glow, from a moderate overflow to a sharp stroboscopic color change.
+ In the second mode, the device works as an equalizer. The software implements the Fast Fourier Transform, which uses the data received from the digital microphone to illuminate a specific section of the led strip. The sampling rate in my case is 20 kHz.
