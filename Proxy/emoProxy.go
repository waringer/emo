package main

import (
	"bytes"
	"crypto/tls"
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strconv"
	"time"
)

type emo_time struct {
	Time int64 `json:"time"`
}

const (
	livingio_api_server = "3.66.68.197"   // api.living.ai => 3.66.68.197
	livingio_tts_server = "tts.living.ai" // tts.living.ai => 3.224.137.253
	postFS              = "/tmp/"
	logFileName         = "/var/log/emoProxy.log"
)

func main() {
	// disable ssl checks
	http.DefaultTransport.(*http.Transport).TLSClientConfig = &tls.Config{InsecureSkipVerify: true}

	// redirect log
	logFile, err := os.OpenFile(logFileName, os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		log.Panic(err)
	}
	defer logFile.Close()
	log.SetOutput(logFile)
	log.SetFlags(log.Lshortfile | log.LstdFlags)

	// parse flags
	Port := flag.Int("port", 8081, "http port")
	flag.Parse()
	log.Println("Port: ", *Port)

	// handle time requests
	http.HandleFunc("/time", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)
		resp := emo_time{time.Now().Unix()}

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusCreated)
		json.NewEncoder(w).Encode(resp)
	})

	// handle token requests
	http.HandleFunc("/token/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusCreated)

		body := makeApiRequest(r)
		fmt.Fprint(w, body)
	})

	// handle emo requests
	http.HandleFunc("/emo/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusCreated)

		body := makeApiRequest(r)
		fmt.Fprint(w, body)
	})

	// handle downloads
	http.HandleFunc("/download/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/octet-stream")
		w.WriteHeader(http.StatusCreated)

		body := makeTtsRequest(r)
		fmt.Fprint(w, body)
	})

	log.Fatal(http.ListenAndServe(":"+strconv.Itoa(*Port), nil))
}

func logRequest(r *http.Request) {
	log.Println("called: ", r)

	for k, v := range r.Header {
		log.Printf("Request-Header field %q, Value %q\n", k, v)
	}
}

func logResponse(r *http.Response) {
	log.Println("called: ", r)

	for k, v := range r.Header {
		log.Printf("Response-Header field %q, Value %q\n", k, v)
	}
}

func makeApiRequest(r *http.Request) string {
	var request *http.Request
	switch r.Method {
	case "GET":
		request, _ = http.NewRequest("GET", "https://"+livingio_api_server+r.URL.RequestURI(), nil)
	case "POST":
		body, _ := ioutil.ReadAll(r.Body)

		// write post request body to fs
		switch r.Header.Get("Content-Type") {
		case "application/json":
			ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".json", body, 0644)
		case "application/octet-stream":
			ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".pcm", body, 0644)
		default:
			ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".bin", body, 0644)
		}

		request, _ = http.NewRequest("POST", "https://"+livingio_api_server+r.URL.RequestURI(), bytes.NewBuffer(body))

		request.Header.Add("Content-Type", r.Header.Get("Content-Type"))
		request.Header.Add("Content-Length", r.Header.Get("Content-Length"))
	default:
	}

	val, exists := r.Header["Authorization"]
	if exists {
		request.Header.Add("Authorization", val[0])
	}

	val, exists = r.Header["Secret"]
	if exists {
		request.Header.Add("Secret", val[0])
	}

	request.Header.Del("User-Agent")

	httpclient := &http.Client{}
	response, err := httpclient.Do(request)

	if err != nil {
		log.Fatalf("An Error Occured %v", err)
	}
	defer response.Body.Close()

	// read response
	body, _ := ioutil.ReadAll(response.Body)
	log.Println("Server response: ", string(body))

	logResponse(response)
	return string(body)
}

func makeTtsRequest(r *http.Request) string {
	request, _ := http.NewRequest("GET", "http://"+livingio_tts_server+r.URL.RequestURI(), nil)

	val, exists := r.Header["Authorization"]
	if exists {
		request.Header.Add("Authorization", val[0])
	}

	val, exists = r.Header["Secret"]
	if exists {
		request.Header.Add("Secret", val[0])
	}

	request.Header.Del("User-Agent")

	httpclient := &http.Client{}
	response, err := httpclient.Do(request)

	if err != nil {
		log.Fatalf("An Error Occured %v", err)
	}
	defer response.Body.Close()

	// read response
	body, _ := ioutil.ReadAll(response.Body)

	// write post request body to fs
	switch response.Header.Get("Content-Type") {
	case "application/json":
		ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".json", body, 0644)
	case "application/octet-stream":
		ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".wav", body, 0644)
	default:
		ioutil.WriteFile(postFS+time.Now().Format("20060102/")+"emo_"+fmt.Sprint(time.Now().Unix())+".bin", body, 0644)
	}

	logResponse(response)
	return string(body)
}
