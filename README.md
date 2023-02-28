# zeusCAM

The zeusCAM is a networked camera based on the ESP32-CAM board.

## Requirements

- [ESP-IDF](https://github.com/espressif/esp-idf) (use `release/v5.0` branch)
- [ESP-CAM](https://github.com/espressif/esp32-camera)

## Building

Once the Espressif tools are setup and linked, the project can be build in the home directory by running `idf.py build`, and flashed onto a device with `idf.py -p <PORT> flash`. 

To view serial debug data from the board you can use `idf.py -p <PORT> monitor` but I found it prone to hanging and instead opt to use `screen <PORT> 115200`. To exit `<ctrl + a>, k`.

## Serial Commands

If connected to a PC with a serial connection, the wifi name and password can be set like:

```
set ssid <WIFI NAME>
set pass <WIFI PASSWORD>
```

This will be stored in the non-volatile storage and the values should be used after a reset.

