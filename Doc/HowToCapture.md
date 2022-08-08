# How have I capured the traffic

Here I descripe how to capture the traffic from emo. It is not a in deep guide how to do it, you will need knowlage of network and system administration and programming.

## Network

### Tools:
    - ISC DHCP Server on FreeBSD
    - ISC DNS Server BIND on FreeBSD
    - TCP Dump on FreeBSD, CentOS
    - Nginx on CentOS
    - go programming language on CentOS
    - Wireshark on Windows
    - EdgeRouter X by Ubiquiti (Firewall)

### Description:
First I have looked with tcpdump on the DNS server what querys emo makes. Only 2 servers where queried, "api" and "tts". Then I have changed my DNS server to return custom records for the seen querys and blocked any traffic from emo to the internet in my firewall.

Now I have the traffic from emo on my own server.

On my own server I have used tcpdump to see what kind of traffic emo is generate. Some of the tcpdmp i have let wrote to a pcap file to anaylse it with wireshark. Here I have seen that requests to "api" are https and "tts" are http. Then i have set up a Nginx Webserver to handle the queries for the "api" and "tts" requests.

Now I have wrote a little webserver in go (emoProxy) to log and analyse the traffic. In Nginx I have set this go webserver to handle all requests from emo. Now I see all requests with all headers and have enhanced the go server to make the request emo makes by itself to the real servers and log the complete responces to analyse it. So I have a complete proxy for EMO and I see all network traffic.

## Bluetooth

### Tools:
    - Wireshark on Windows
    - nRF Tools from NORDIC Semiconductor
    - GeeekPi nRF52840 Micro Dev Kit USB Dongle
    - dotnet framework on Windows

### Setup:
The [nRF52840 dongle](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/) needed some preparion. First replace the UF2 Bootloader with the [Open Bootloader](https://github.com/makerdiary/nrf52840-mdk-usb-dongle/tree/master/firmware/open_bootloader). Then flash the [sniffer firmware](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_ble%2FUG%2Fsniffer_ble%2Fintro.html) from NORDIC Semiconductor.

Then setup Wireshark to [use the dongle](https://www.youtube.com/watch?v=JIh2YYwkzoE).

### Description:
Very strait forward, I use Wireshark with the nRF Sniffer to capture the traffic between the android app and emo.

The dotnet program is used to test around and confirm my assumptions.

### Disclaimer:
My knowlage with bluetooth are very limited and I learn while I work on this!
