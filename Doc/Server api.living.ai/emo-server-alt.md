# /emo/server/alt

## Description:
Get alternative api servers.  

### Sample Request:
**URL:** GET /emo/server/alt HTTP/1.0  

**Headers:**  
- *Content-Type*
    Allways "application/x-www-form-urlencoded"
- *Authorization*  
    JWT Token, for example "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA"  

### Sample Response:
{"servers":["us-api.living.ai","eu-api.living.ai","as-api.living.ai"],"switch":""}
