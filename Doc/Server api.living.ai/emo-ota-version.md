# /emo/ota/version

## Description:
Check for new firmware.  

### Sample Request:
**URL:** GET /emo/ota/version HTTP/1.0  

**Headers:**  
- *Authorization*  
    JWT Token, for example "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA"  

### Sample Response:
{"version-name":"1.4.0","version-num":21}
