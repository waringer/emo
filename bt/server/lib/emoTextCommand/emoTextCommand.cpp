#include "emoTextCommand.h"

const char *parseTextCommand(const char *inMessage)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, inMessage);
    std::string response = "";

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
    else
    {
        std::string reqType((const char *)doc["type"]);

        if (reqType == "sta_req")
        {
            std::string responce = "{\"type\":\"sta_rsp\",\"data\":{";

            bool needComma = false;
            for (int i = 0; i < doc["data"]["request"].size(); i++)
            {
                long reqID = doc["data"]["request"][i];
                if (needComma)
                    responce += ",";
                else
                    needComma = true;

                switch (reqID)
                {
                case 0:
                    responce += "\"device\":{\"device_id\":\"*aabbccddeeff*\"}";
                    break;
                case 1:
                    responce += "\"version\":{\"number\":22,\"name\":\"1.5.0\"}";
                    break;
                case 2:
                    responce += "\"network\":{\"connected\":1,\"name\":\"wlan ssid\"}";
                    break;
                case 3:
                    responce += "\"xiaoai\":{\"connected\":0}";
                    break;
                case 4:
                    responce += "\"alexa\":{\"connected\":0}";
                    break;
                case 5:
                    responce += "\"light\":{\"state\":[]}";
                    break;
                case 6:
                    responce += "\"alarm\":{\"state\":[]}";
                    break;
                case 7:
                    responce += "\"city\":{\"id\":0,\"name\":\"Bonn\",\"state\":\"\",\"country\":\"\",\"coord\":{\"lon\":0,\"lat\":0}}";
                    break;
                case 8:
                    responce += "\"timezone\":{\"name\":\"Europe/Berlin\",\"code\":\"\",\"offset\":7200}";
                    break;
                case 9:
                    responce += "\"achievements\":{\"unlocked\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]}";
                    break;
                case 10:
                    responce += "\"dances\":{\"joris_unlocked\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]}";
                    break;
                case 11:
                    responce += "\"personality\":{\"name\":\"EMO Name\",\"age\":2000,\"birthday\":\"12.31.\",\"color\":\"pink\",\"lucky_number\":99,\"sign\":\"leo\",\"number\":\"eeff\"}";
                    break;
                case 12:
                    responce += "\"preference\":{\"volume\":1,\"temperature\":0,\"length\":0,\"auto_update\":0,\"schedule_sound\":1}";
                    break;

                default:
                    responce += "";
                    break;
                }
            }

            responce += "}}";

            // Serial.print("responce:");
            // Serial.println(responce.c_str());
            response += responce;
        }

        if (reqType == "setting_req")
            response += "{\"type\":\"setting_rsp\",\"data\":{\"result\":1}}";

        if (reqType == "anim_req")
            response += "{\"type\":\"anim_rsp\",\"data\":{\"result\":1}}";

        if (reqType == "face_req")
        {
            std::string op((const char *)doc["data"]["op"]);
            if (op == "in")
                response += "{\"type\":\"face_rsp\",\"data\":{\"result\":1}}";
            if (op == "out")
                response += "{\"type\":\"face_rsp\",\"data\":{\"result\":1}}";
            if (op == "syn")
                response += "{\"type\":\"face_rsp\",\"data\":{\"faces\":{\"0\":\"perosn a\",\"1\":\"perosn b\"},\"result\":1}}";
            if (op == "del")
                response += "{\"type\":\"face_rsp\",\"data\":{\"result\":1}}";
            if (op == "rescan")
                response += "{\"type\":\"face_rsp\",\"data\":{\"result\":1}}";
            if (op == "add")
                response += "{\"type\":\"face_rsp\",\"data\":{\"result\":1}}";
        }

        if (reqType == "customize_req")
            response += "{\"type\":\"eye_rsp\",\"data\":{\"result\":1}}";

        if (reqType == "off_rsp")
            response += "{\"type\":\"off_rsp\",\"data\":{\"result\":0}}";
    }

    return response.c_str();
}
