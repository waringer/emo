# EMO Proxy server
This part of the knowledge bank is a Proxy server to analyze the traffic that goes between the EMO robot and the living.ai servers.

## Start the proxy
`go run emoProxy.go`

## Docker setup
You can find a simplified setup using Docker Compose by SebbeJohansson here: https://github.com/SebbeJohansson/emo-proxy-docker
It uses nginx, dnsmasq, and mitmproxy to be able to pass through the api requests to the EMO Proxy.
