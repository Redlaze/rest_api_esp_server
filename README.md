# REST-API server on ESP 32
## Server run on browser like a WiFi-router
### Example GET-request:
#### URL adress: www.{your_server_name}.com/data
```json
{
  "T_CENTRE_1": 42.3,
  "T_AIR_1": 32.1,
  "PRESS_1": 2.1,
  "V_BAT_1": 3.56,
  "VIBRO_1": 4653.4,
  "VIBRO_L_1": [
    4567,
    4563,
    4678,
    4890,
    4569
  ],
  "ACU_1": 2454.2,
  "ACU_L_1": [
    2345,
    2785,
    2567,
    2340,
    2234
  ],
  "N_TX_1": 2,
  "W_TX_1": 22,
  ...
}
```
### Example POST-request:
#### URL adress: www.{your_server_name}.com/statsus
```json
{
  "status": "RecieveDataOK"
}
```

### Boot this code without compile:
#### 1.Open cmd on your PC and run commands:
```bash
cd "{name_of_disk}:\Users\{name_of_user}\AppData\
Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1"
```
```bash
"esptool.exe" --chip esp32 --port "COM7" --baud 921600  --before default_reset --after hard_reset write_flash  -z --flash_mode dio
 --flash_freq 80m --flash_size 4MB 0x1000 "path\to\your\folder_sketch\rest_api_esp_server\build\esp32.esp32.esp32da\Web_server_sts.ino.bootloader.bin" 
 0x8000 "path\to\your\folder_sketch\rest_api_esp_server\build\esp32.esp32.esp32da\Web_server_sts.ino.partitions.bin" 
 0xe000 "{name_of_disk}:\Users\{name_of_user}\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.14/tools/partitions/boot_app0.bin" 
 0x10000 "path\to\your\folder_sketch\rest_api_esp_server\build\esp32.esp32.esp32da\Web_server_sts.ino.bin"
```