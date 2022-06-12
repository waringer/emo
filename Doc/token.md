# /token/*\<emos mac>*

## Description:
Emo send this request to authenticate against the server.  
The Mac is all lower case and without any delimiter and 12 characters long.  

The Response is a Json with 3 Fields:
- access_token  
    Common JSON Web Token (JWT) where sub is *\<emos mac>*
- expire_in  
    Time in seconds this token is valid
- type  
    token type, allways "Bearer" *(as far a i have seen it)*

### Sample Request:
**URL:** GET /token/aabbccddeeff HTTP/1.0  
**Headers:**  

### Sample Response:
{"access_token":"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2NDA5OTg4MDAsInN1YiI6ImFhYmJjY2RkZWVmZiIsIm5iZiI6MTY0MDk5MTYwMCwiaWF0IjoxNjQwOTkxNjAwfQ.PEmljG3s2k5DqvvAOJZ3-lT5jSPtYGI0GuzSqQe-QCA","expire_in":3600,"type":"Bearer"}


