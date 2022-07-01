package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"net/url"
)

func TTS(key string, hl string, src string, v string, r string, c string, f string) ([]byte, error) {
	url := fmt.Sprintf(
		"http://api.voicerss.org/?key=%s&hl=%s&src=%s&v=%s&r=%s&c=%s&f=%s",
		key, hl, src, v, r, c, f)

	resp, err := http.Get(url)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()
	body, _ := ioutil.ReadAll(resp.Body)

	return body, nil
}

func main() {
	wav, err := TTS(
		"put_api_key_here",
		"en-us",
		url.QueryEscape("it's all good now!"),
		"Mike",
		"0",
		"WAV",
		"24khz_8bit_mono")

	if err != nil {
		log.Fatalln(err)
	}

	ioutil.WriteFile("tts-out.wav", wav, 0777)
}
