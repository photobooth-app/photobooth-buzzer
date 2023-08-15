# Photobooth Buzzer

Buzzer based on ESP32 connects to photobooth via bluetooth emulating a keyboard to trigger captures remotely.

**[photobooth-app](https://github.com/mgrl/photobooth-app)** - **[Documentation](https://mgrl.github.io/photobooth-docs/)** - **[3d printed box](https://mgrl.github.io/photobooth-docs/photobox3dprint/)**

## 😍 Features

- battery powered, no cords
- simple installation using Visual Studio Code and Platform IO plugin
- emulates keyboard, easy integration in a photobooth

## Images

![parts](https://raw.githubusercontent.com/mgrl/photobooth-buzzer/main/images/parts.jpg)

## BOM

- [3d printed part](https://github.com/mgrl/photobooth-buzzer/blob/main/cad/stl/inlet.STL)
- buzzer: [Not Aus Schalter/Taster, rastend, 240V, NC und NO Pilztaster rot IP65 1Ö 1S Buzzer P1C400E72](https://www.amazon.de/gp/product/B088F3NNV4/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- battery: [4XEEMB 3.7V 1100mAh Lipo Batterie Akku 603449 Lithium Polymer Batterien mit JST Stecker.](https://www.amazon.de/gp/product/B08VRYS8FT/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- esp board: [LOLIN32 Lite Board V1.0 mit ESP-32](https://www.amazon.de/dp/B086V8X2RM?ref=ppx_yo2ov_dt_b_product_details&th=1)
- [on off switch](https://www.amazon.de/gp/product/B07VFV17SP/)

## Assembly

Check images.

## Flash ESP

- use platformio plugin in VCS
- load this project
- click build and upload
- watch serial output for some debug messages of the ESP device.

## Integrate with photobooth

To integrate the buzzer with the photobooth-app check the docs <https://mgrl.github.io/photobooth-docs/extras/buzzer/>
