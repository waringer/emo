#include "WebConsole.h"

WebServer webServer(80);

void WebConsole::setup()
{
    Serial.print("ESP Board MAC Address: ");
    Serial.println(WiFi.macAddress());

    Serial.print("Connecting with WiFi network ");
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.print(" done. IPv4: ");
    Serial.print(WiFi.localIP());
    Serial.print(" IPv6: ");
    Serial.println(WiFi.localIPv6());

    webServer.on("/", handle_OnIndex);
    webServer.on("/style.css", handle_OnStyle);
    webServer.on("/favicon.ico", handle_Onfavicon);
    webServer.onNotFound(handle_NotFound);
    webServer.begin();
    Serial.println("HTTP server started");
}

void WebConsole::loop()
{
    webServer.handleClient();
}

//*********************

void handle_Onfavicon()
{
    webServer.send(200, "image/x-icon", SendFile("/favicon.ico"));
}

void handle_OnStyle()
{
    webServer.send(200, "text/css", SendFile("/style.css"));
}

void handle_OnIndex()
{
    // Serial.println("Index args:");
    // for (int i = 0; i < webServer.args(); i++)
    // {
    //     Serial.print(webServer.argName(i));
    //     Serial.print("=");

    //     if (webServer.hasArg(webServer.argName(i)))
    //         Serial.println(webServer.arg(webServer.argName(i)));
    //     else
    //         Serial.println("---");
    // }

    if (webServer.hasArg("cmdType"))
    {
        if (webServer.arg("cmdType") == "txt")
        {
            if (webServer.hasArg("T00"))
                emoCoreBLE::sendText(webServer.arg("T00").c_str());
        }

        if (webServer.arg("cmdType") == "bin")
        {
            uint8_t byte_array[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

            if (webServer.hasArg("B00"))
                byte_array[0] = strtol(webServer.arg("B00").c_str(), 0, 16);
            if (webServer.hasArg("B01"))
                byte_array[1] = strtol(webServer.arg("B01").c_str(), 0, 16);
            if (webServer.hasArg("B02"))
                byte_array[2] = strtol(webServer.arg("B02").c_str(), 0, 16);
            if (webServer.hasArg("B03"))
                byte_array[3] = strtol(webServer.arg("B03").c_str(), 0, 16);
            if (webServer.hasArg("B04"))
                byte_array[4] = strtol(webServer.arg("B04").c_str(), 0, 16);
            if (webServer.hasArg("B05"))
                byte_array[5] = strtol(webServer.arg("B05").c_str(), 0, 16);
            if (webServer.hasArg("B06"))
                byte_array[6] = strtol(webServer.arg("B06").c_str(), 0, 16);
            if (webServer.hasArg("B07"))
                byte_array[7] = strtol(webServer.arg("B07").c_str(), 0, 16);
            if (webServer.hasArg("B08"))
                byte_array[8] = strtol(webServer.arg("B08").c_str(), 0, 16);
            if (webServer.hasArg("B09"))
                byte_array[9] = strtol(webServer.arg("B09").c_str(), 0, 16);
            if (webServer.hasArg("B10"))
                byte_array[10] = strtol(webServer.arg("B10").c_str(), 0, 16);
            if (webServer.hasArg("B11"))
                byte_array[11] = strtol(webServer.arg("B11").c_str(), 0, 16);
            if (webServer.hasArg("B12"))
                byte_array[12] = strtol(webServer.arg("B12").c_str(), 0, 16);
            if (webServer.hasArg("B13"))
                byte_array[13] = strtol(webServer.arg("B13").c_str(), 0, 16);
            if (webServer.hasArg("B14"))
                byte_array[14] = strtol(webServer.arg("B14").c_str(), 0, 16);
            if (webServer.hasArg("B15"))
                byte_array[15] = strtol(webServer.arg("B15").c_str(), 0, 16);
            if (webServer.hasArg("B16"))
                byte_array[16] = strtol(webServer.arg("B16").c_str(), 0, 16);
            if (webServer.hasArg("B17"))
                byte_array[17] = strtol(webServer.arg("B17").c_str(), 0, 16);
            if (webServer.hasArg("B18"))
                byte_array[18] = strtol(webServer.arg("B18").c_str(), 0, 16);
            if (webServer.hasArg("B19"))
                byte_array[19] = strtol(webServer.arg("B19").c_str(), 0, 16);

            emoCoreBLE::sendBin(byte_array);
        }
    }

    webServer.send(200, "text/html", SendHTML());
}

void handle_NotFound()
{
    webServer.send(404, "text/plain", "Not found");
}

String SendHTML()
{
    String ptr;

    File file = SPIFFS.open("/index.html");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return "";
    }

    // Serial.println("File Content:");
    while (file.available())
    {
        // Serial.write(file.read());
        ptr = file.readString();
    }
    file.close();

    return ptr;
}

String SendFile(const char *Filename)
{
    String ptr;

    File file = SPIFFS.open(Filename);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return "";
    }

    // Serial.println("File Content:");
    while (file.available())
    {
        // Serial.write(file.read());
        ptr = file.readString();
    }
    file.close();

    return ptr;
}
