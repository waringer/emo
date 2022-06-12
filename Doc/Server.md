# Server

Server-URL: api.living.ai

## Known main entry points:
- /time
    - Description: Emo request the currnet unixtime in utc
    - don't need any auth from server side
- /token/
    - Description: Emo authenticate against the server
    - send his mac as part of the path
    - emo sends only a Secret as header with the request
- /emo/
    - Main api for emo
    - emo sends a Secret and Authorization as headers with every request

## Requests from Emo:
All requests from emo to this server are made over https, but without any checks of the certificate. With every request emo sends the folling http headers:
- *Secret*  
    Contains a string that change every second. the string is 22 characters long and can contains the characters a-z, A-Z, 0-9, -_  
    *Perhaps it is a Base64url encoded Md5 hash of something*
