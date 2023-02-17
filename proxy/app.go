package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"mime/multipart"
	"net/http"
	"os"
	"path/filepath"
)

func main() {

	http.HandleFunc("/loading", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodPost {
			w.WriteHeader(http.StatusMethodNotAllowed)
			return
		}
		type reqModel struct {
			MediaServerDest string `json:"mediaServerDest"`
			Token           string `json:"token"`
			FilePath        string `json:"filePath"`
		}
		var requestBody reqModel
		if err := json.NewDecoder(r.Body).Decode(&requestBody); err != nil {
			w.WriteHeader(http.StatusBadRequest)
			log.Println(err)
			return
		}
		log.Println(requestBody.Token)
		file, err := os.Open(requestBody.FilePath)
		if err != nil {
			w.WriteHeader(http.StatusNotFound)
			log.Println(err)
			fmt.Fprintln(w, "{\"error\": \"File not exist\"}")
			return
		}
		defer file.Close()

		body := &bytes.Buffer{}
		writer := multipart.NewWriter(body)
		part, err := writer.CreateFormFile("file", filepath.Base(file.Name()))
		if err != nil {
			log.Println(err)
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
		log.Println("File created")
		io.Copy(part, file)
		writer.Close()

		uplaodingRequest, _ := http.NewRequest("POST", requestBody.MediaServerDest, body)
		uplaodingRequest.Header.Add("Content-Type", writer.FormDataContentType())
		uplaodingRequest.Header.Add("Token", requestBody.Token)
		client := &http.Client{}
		log.Println("Proxy request")
		uploadingResponse, err := client.Do(uplaodingRequest)
		if err != nil {
			log.Println(err)
			w.WriteHeader(http.StatusInternalServerError)
			return
		}

		responseData, _ := ioutil.ReadAll(uploadingResponse.Body)
		w.WriteHeader(uploadingResponse.StatusCode)
		fmt.Fprintln(w, string(responseData))
	})
	http.ListenAndServe(":5555", nil)
}
