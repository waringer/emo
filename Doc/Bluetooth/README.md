# Bluetooth

- Uses Bluetooth 4.2 LE
- Device name is "EMO-" and the last 2 Bytes in hex from the Bluetooth adress.
- EMO uses LE Advertisements and GATT

## GATT Services and Characteristics:  
- 00001801-0000-1000-8000-00805f9b34fb  
    - 00002a05-0000-1000-8000-00805f9b34fb  
        - canRead:False  
        - canWrite:False  
        - canNotify:False   
- 00001800-0000-1000-8000-00805f9b34fb  
    - 00002a00-0000-1000-8000-00805f9b34fb  
        - canRead:True  
        - canWrite:False  
        - canNotify:False  
    - 00002a01-0000-1000-8000-00805f9b34fb  
        - canRead:True  
        - canWrite:False  
        - canNotify:False  
    - 00002aa6-0000-1000-8000-00805f9b34fb  
        - canRead:True  
        - canWrite:False  
        - canNotify:False  
- 0000ffe0-0000-1000-8000-00805f9b34fb  
    - 0000ffe1-0000-1000-8000-00805f9b34fb  
        - canRead:True  
        - canWrite:True  
        - canNotify:True  

## text commands:  
- write to 0000ffe1
- responses delivered over notifications
- 4 byte header
    - first 2 bytes: 0xbb, 0xaa
    - last 2 byte: lenght of text (little-endian)
- responses use the same header

### {"data":{"request":[**0**]},"type":"sta_req"}:   

- 0 => {"type":"sta_rsp","data":{"device":{"device_id":"*aabbccddeeff*"}}}  
- 1 => {"type":"sta_rsp","data":{"version":{"number":22,"name":"1.5.0"}}}  
- 2 => {"type":"sta_rsp","data":{"network":{"connected":1,"name":"*wlan ssid*"}}}  
- 3 => {"type":"sta_rsp","data":{"xiaoai":{"connected":0}}}  
- 4 => {"type":"sta_rsp","data":{"alexa":{"connected":0}}}  
- 5 => {"type":"sta_rsp","data":{"light":{"state":[]}}}  
- 6 => {"type":"sta_rsp","data":{"alarm":{"state":[]}}}  
- 7 => {"type":"sta_rsp","data":{"city":{"id":0,"name":"*cityname*","state":"","country":"","coord":{"lon":0,"lat":0}}}}  
- 8 => {"type":"sta_rsp","data":{"timezone":{"name":"*Europe/Berlin*","code":"","offset":*7200*}}}   
- 9 => {"type":"sta_rsp","data":{"achievements":{"unlocked":[*1*]}}}   
- 10 => {"type":"sta_rsp","data":{"dances":{"joris_unlocked":[*1,2*]}}}   
- 11 => {"type":"sta_rsp","data":{"personality":{"name":"*EMO Name*","age":*1*,"birthday":"*12.31.*","color":"*pink*","lucky_number":*99*,"sign":"*leo*","number":"*eeff*"}}}   
- 12 => {"type":"sta_rsp","data":{"preference":{"volume":1,"temperature":0,"length":0,"auto_update":0,"schedule_sound":1}}}   
- 13 => {"type":"sta_rsp","data":{}}   

### {"data":{"op":"in"},"type":"face_req"}:  
- => {"type":"face_rsp","data":{"result":1}} 

### {"data":{"op":"syn"},"type":"face_req"}:  
- => {"type":"face_rsp","data":{"faces":{"0":"*perosn a*","1":"*perosn b*"},"result":1}}  

### {"data":{"op":"in"},"type":"setting_req"}:  
- => {"type":"setting_rsp","data":{"result":1}}  

### {"data":{"op":"out"},"type":"setting_req"}:  
- => {"type":"setting_rsp","data":{"result":1}}  

### {"data":{"op":"in"},"type":"anim_req"}:
- => {"type":"anim_rsp","data":{"result":1}}

### {"type":"off_req"}:  
- on skateboard, no shutdown => {"type":"off_rsp","data":{"result":0}}
- off skateboard, power down => {"type":"off_rsp","data":{"result":1}}

### {"data":{"name":"*dancename*","op":"play"},"type":"anim_req"}:  
- works only after emo is in "anim" mode => {"data":{"op":"in"},"type":"anim_req"}
- => {"type":"anim_rsp","data":{"result":1}}
    - dancenames:
        - d1_EmoDance
        - d2_WontLetGo
        - d3_Blindless
        - d4_Click1
        - d5_TimeOfMyLife
        - d6_Rollercoaster
        - d7_FlashBack
        - d8_Click2
        - d9_BlameYourself
        - d10_CanITakeYouThere
        - d11_OceanBlue

### binany commands:  
- write to 0000ffe1
- responses delivered over notifications
- commands have 20 byte
- first 2 bytes 0xdd, 0xcc
- third byte is order?!
- four and five are command bytes?!
- six and after are parameters?!

### seen sended from app:
- ddcc 1f 0000 0100 00000000000000000000000000 // enable status updates for 0x0000 ? (actions)  
- ddcc 20 0200 0100 00000000000000000000000000 // enable status updated for 0x0200 ? (charge)  

### seen sended from emo:
- DDCC 00 0200 5000 00000000000000000000000000 // charge 80%  
- DDCC 00 0200 6400 00000000000000000000000000 // charge 100%  

- DDCC 00 0000 0100 00000000000000000000000000 // shaking  
- DDCC 00 0000 0200 00000000000000000000000000 // petted  
- DDCC 00 0000 0205 00000000000000000000000000  
- DDCC 00 0000 0301 00000000000000000000000000 // lifted  
- DDCC 00 0000 0303 00000000000000000000000000 // charging  
- DDCC 00 0000 0304 00000000000000000000000000  

- DDCC 00 0000 0400 00000000000000000000000000 // cliff  
- DDCC 00 0000 0403 00000000000000000000000000  

- DDCC 00 0000 0501 00000000000000000000000000 // Headset on/off  
- DDCC 00 0000 0503 00000000000000000000000000 // eyes anim?  
- DDCC 00 0000 0504 00000000000000000000000000 // eyes anim?  

- DDCC 00 0000 0601 00000000000000000000000000 // wall  
- DDCC 00 0000 0602 00000000000000000000000000  

- DDCC 00 0000 0A00 00000000000000000000000000 // play game animation - Roll dice, flip coin, lucky fruit...  
- DDCC 00 0000 0B00 00000000000000000000000000 // listening  
- DDCC 00 0000 0C00 00000000000000000000000000 // answering1  
- DDCC 00 0000 0E00 00000000000000000000000000 // dancing  
- DDCC 00 0000 0F00 00000000000000000000000000 // answer time date  
- DDCC 00 0000 1100 00000000000000000000000000 // Play animation  
- DDCC 00 0000 1200 00000000000000000000000000 // answering2  
- DDCC 00 0000 1300 00000000000000000000000000  
- DDCC 00 0000 1700 00000000000000000000000000 // standing  
- DDCC 00 0000 1800 00000000000000000000000000 // sleep  
- DDCC 00 0000 1900 00000000000000000000000000 // pet while sleep?  
- DDCC 00 0000 1A00 00000000000000000000000000 // wakeup?  
- DDCC 00 0000 1D00 00000000000000000000000000 // exploring  
- DDCC 00 0000 1F00 00000000000000000000000000 // look arround?  
- DDCC 00 0000 2000 00000000000000000000000000 // idle on Skateboard?  
- DDCC 00 0000 2100 00000000000000000000000000  
- DDCC 00 0000 2300 00000000000000000000000000 // play time of day animation   



