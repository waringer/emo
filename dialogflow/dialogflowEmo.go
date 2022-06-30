package main

import (
	"context"
	"flag"
	"fmt"
	"io/ioutil"
	"log"

	dfClient "cloud.google.com/go/dialogflow/apiv2"
	"google.golang.org/api/option"
	dfApi "google.golang.org/genproto/googleapis/cloud/dialogflow/v2"
)

func DetectIntentAudio(projectID, sessionID, audioFile, languageCode string, service_account_credentials string) (string, error) {
	ctx := context.Background()

	sessionClient, err := dfClient.NewSessionsClient(ctx, option.WithCredentialsFile(service_account_credentials))
	if err != nil {
		return "", err
	}
	defer sessionClient.Close()

	if projectID == "" || sessionID == "" {
		return "", fmt.Errorf("received empty project (%s) or session (%s)", projectID, sessionID)
	}

	sessionPath := fmt.Sprintf("projects/%s/agent/sessions/%s", projectID, sessionID)

	// In this example, we hard code the encoding and sample rate for simplicity.
	audioConfig := dfApi.InputAudioConfig{
		AudioEncoding:   dfApi.AudioEncoding_AUDIO_ENCODING_LINEAR_16,
		SampleRateHertz: 8000,
		LanguageCode:    languageCode}

	queryAudioInput := dfApi.QueryInput_AudioConfig{
		AudioConfig: &audioConfig}

	audioBytes, err := ioutil.ReadFile(audioFile)
	if err != nil {
		return "", err
	}

	queryInput := dfApi.QueryInput{
		Input: &queryAudioInput}
	request := dfApi.DetectIntentRequest{
		Session:    sessionPath,
		QueryInput: &queryInput,
		InputAudio: audioBytes}

	response, err := sessionClient.DetectIntent(ctx, &request)
	if err != nil {
		return "", err
	}

	queryResult := response.GetQueryResult()
	log.Println("queryResult:", queryResult)
	fulfillmentText := queryResult.GetFulfillmentText()
	return fulfillmentText, nil
}

func main() {
	// parse flags
	projectID := flag.String("projectID", "", "projectID")
	languageCode := flag.String("language", "en-US", "languageCode")
	service_account_credentials := flag.String("account", "account.json", "service account credential file")
	wavFile := flag.String("wav", "output.wav", "wav file to read")
	flag.Parse()
	log.Println("projectID: ", *projectID)
	log.Println("languageCode: ", *languageCode)
	log.Println("account credential file: ", *service_account_credentials)
	log.Println("WAV file: ", *wavFile)

	text, err := DetectIntentAudio(*projectID, "sessionid-1234567", *wavFile, *languageCode, *service_account_credentials)

	if err != nil {
		log.Println("error:", err)
	}

	log.Println("text:", text)
}
