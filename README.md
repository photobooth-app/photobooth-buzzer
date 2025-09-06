# Photobooth Buzzer

Buzzer based on ESP32 connects to photobooth via bluetooth emulating a keyboard to trigger captures remotely.

**[photobooth-app](https://github.com/photobooth-app/photobooth-app)** - **[Documentation](https://photobooth-app.org/)** - **[3d printed box](https://photobooth-app.org/photobox3dprint/)**

## 😍 Features

- battery powered, no cords
- battery status reporting via bluetooth
- USB-C charge port (and programming)
- Possible to charge while using. Battery is charged even if buzzer is turned off.
- simple installation using Visual Studio Code and Platform IO plugin
- emulates bluetooth keyboard, easy integration in a photobooth
- Runtime on battery is ~18h. The power consumption measured is about 60mA - the battery has 1100mAh capacity.

## Images

![buzzer view 1](https://raw.githubusercontent.com/photobooth-app/photobooth-buzzer/main/images/buzzer1.jpg)
![buzzer view 2](https://raw.githubusercontent.com/photobooth-app/photobooth-buzzer/main/images/buzzer2.jpg)
![buzzer parts](https://raw.githubusercontent.com/photobooth-app/photobooth-buzzer/main/images/buzzer_open.jpg)


## BOM

- [3d printed parts](https://github.com/photobooth-app/photobooth-buzzer/tree/main/cad)
- Buzzer: [Not Aus Schalter/Taster, rastend, 240V, NC und NO Pilztaster rot IP65 1Ö 1S Buzzer P1C400E72](https://www.amazon.de/gp/product/B088F3NNV4)
- Battery: [4XEEMB 3.7V 1100mAh Lipo Batterie Akku 603449 Lithium Polymer Batterien mit JST Stecker.](https://www.amazon.de/gp/product/B08VRYS8FT)
- ESP board: [Adafruit ESP32-S3 Feather with STEMMA QT / Qwiic (8 or 4MB flash)](https://www.adafruit.com/product/5477)
- [on off switch](https://www.amazon.de/gp/product/B07VFV17SP/)
- some M2 screws

## Assembly

- Check images and CAD
- Connect button switch to GND and GPIO D12.
- Connect power switch to GND and EN.

## Flash ESP Firmware

- use platformio plugin in VCS
- load this project
- click build and upload
- watch serial output for some debug messages of the ESP device.

## Integrate with photobooth

To integrate the buzzer with the photobooth-app check the docs <https://photobooth-app.org/extras/buzzer/>
