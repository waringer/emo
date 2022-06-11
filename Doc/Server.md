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
