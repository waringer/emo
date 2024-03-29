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
	Time   int64 `json:"time"`
	Offset int   `jason:"offset"`
}

type emo_code struct {
	Code       int64  `json:"code"`
	Errmessage string `json:"errmessage"`
}

const (
	livingio_api_server    = "api.living.ai"    // my dns point to own server, local hosts on server point to lai server
	livingio_tts_server    = "eu-tts.living.ai" // [server name must be used]
	livingio_res_eu_server = "res.living.ai"    // [server name must be used]
	postFS                 = "/tmp/"
	logFileName            = "/var/log/emoProxy.log"
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
		_, dtsDiff := time.Now().Zone()
		resp := emo_time{time.Now().Unix(), dtsDiff} // get offset from tz in query

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusOK)
		json.NewEncoder(w).Encode(resp)
	})

	// handle token requests
	http.HandleFunc("/token/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusOK)

		body := makeApiRequest(r)
		fmt.Fprint(w, body)
	})

	// handle emo requests
	http.HandleFunc("/emo/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusOK)

		body := makeApiRequest(r)
		fmt.Fprint(w, body)
	})

	// handle home station requests
	http.HandleFunc("/home/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusOK)

		body := makeApiRequest(r)
		fmt.Fprint(w, body)
	})

	http.HandleFunc("/app/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)
		resp := emo_code{200, "OK"}

		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.WriteHeader(http.StatusOK)
		json.NewEncoder(w).Encode(resp)
	})

	// handle downloads
	http.HandleFunc("/download/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		w.Header().Set("Content-Type", "application/octet-stream")
		w.WriteHeader(http.StatusOK)

		body := makeTtsRequest(r)
		fmt.Fprint(w, body)
	})

	//handle res requests (fw update)
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		logRequest(r)

		body := makeResRequest(r, w)
		w.WriteHeader(http.StatusOK)

		fmt.Fprint(w, body)
	})

	log.Fatal(http.ListenAndServe(":"+strconv.Itoa(*Port), nil))
}

func logRequest(r *http.Request) {
	log.Println("request call: ", r)

	for k, v := range r.Header {
		log.Printf("Request-Header field %q, Value %q\n", k, v)
	}
}

func logResponse(r *http.Response) {
	log.Println("responce call: ", r)

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
		dir := postFS + time.Now().Format("20060102/")
		os.MkdirAll(dir, os.ModePerm)
		switch r.Header.Get("Content-Type") {
		case "application/json":
			ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".json", body, 0644)
		case "application/octet-stream":
			ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".pcm", body, 0644)
		default:
			ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".bin", body, 0644)
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
	dir := postFS + time.Now().Format("20060102/")
	os.MkdirAll(dir, os.ModePerm)
	switch response.Header.Get("Content-Type") {
	case "application/json":
		ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".json", body, 0644)
	case "application/octet-stream":
		ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".wav", body, 0644)
	default:
		ioutil.WriteFile(dir+"emo_"+fmt.Sprint(time.Now().Unix())+".bin", body, 0644)
	}

	logResponse(response)
	return string(body)
}

func makeResRequest(r *http.Request, w http.ResponseWriter) string {
	request, _ := http.NewRequest("GET", "https://"+livingio_res_eu_server+r.URL.RequestURI(), nil)

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
	dir := postFS + time.Now().Format("20060102/")
	os.MkdirAll(dir, os.ModePerm)
	switch response.Header.Get("Content-Type") {
	case "application/json":
		ioutil.WriteFile(dir+"emo_res_"+fmt.Sprint(time.Now().Unix())+".json", body, 0644)
	case "application/octet-stream":
		ioutil.WriteFile(dir+"emo_res_"+fmt.Sprint(time.Now().Unix())+".wav", body, 0644)
	default:
		ioutil.WriteFile(dir+"emo_res_"+fmt.Sprint(time.Now().Unix())+".bin", body, 0644)
	}

	for k, _ := range response.Header {
		w.Header().Set(k, response.Header.Get(k))
	}

	logResponse(response)
	return string(body)
}
