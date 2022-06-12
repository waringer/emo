# /emo/report/info

## Description:
Emo use this request to send status reports to the server.  

### Sample Request:
**URL:** POST /emo/report/info HTTP/1.0  

**Headers:**  
- *Content-Type*  
    Allways "application/json"
- *Content-Length"*  
    Lenght of Body
- *Authorization*  
    JWT Token, for example "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA"  

**Body:**  
The Data that emo sends differs, here are 2 samples:
- {"ver":21,"city":"Bonn","tzone":"Europe/Berlin","lang":"","age":42,"wkup":378,"kcrash":"0/0.84","batt":3720,"drop":0,"beh":26,"touch":[1000,1000,1000],"dis":440,"cliff":[1000,900,1200,700],"mpu":[1,7,50],"wifi":-55,"geo":120.38643646240234,"mic":[0,0,0,0]}
- {"ver":21,"city":"Bonn","tzone":"Europe/Berlin","lang":"","age":42,"wkup":386,"kcrash":"0/3.77","batt":3900,"drop":0,"beh":31,"touch":[1000,1000,1000],"wifi":-60,"mic":[0,0,0,0]}

### Sample Response:
{"result":"OK"}
