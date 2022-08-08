# /emo/weather/forecast

## Description:
Used get weather forecast.
- Parameter *city* = Configured City
- Parameter *lon* = unused?!
- Parameter *lat* = unused?!

### Sample Request:
**URL:** POST GET /emo/weather/forecast?city=Bonn&later=0&lon=0.00000&lat=0.00000

**Headers:** 
- *Content-Type*  
    Allways "application/x-www-form-urlencoded"
- *Authorization*  
    JWT Token, for example "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA"  

### Sample Response:
{"weather":{"main":"cloudy","temp_c":23.87}}
