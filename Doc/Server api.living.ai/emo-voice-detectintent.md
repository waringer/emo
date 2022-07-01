# /emo/voice/detectintent

## Description:
Used to parse speech and get intent.
- Parameter *locale* = Configured City
- Parameter *timezone* = Timezone
- Parameter *languagecode* = Language
- Parameter *lon* = unused?!
- Parameter *lat* = unused?!
- Body is the recorded audio in pcm format (8000 Hz, 1 channel and 32bit unsigned big endian)

### Sample Request:
**URL:** POST /emo/voice/detectintent?locale=Bonn&timezone=Europe/Berlin&lon=0.00000&lat=0.00000&languagecode=en

**Headers:** 
- *Content-Type*  
    Allways "application/octet-stream"
- *Content-Length"*  
    Lenght of Body
- *Authorization*  
    JWT Token, for example "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA"  

### Sample Response:
{"queryId":"d3bebf60-87e1-4367-a1b7-b939-89f67c5a491b","queryResult":{"resultCode":"4a05af1c-4101-4da0-9eca-b939-f822a1668b34","queryText":"shut down","intent":{"name":"power_off","confidence":1},"rec_behavior":"power_off","behavior_paras":{}},"languageCode":"en"}
