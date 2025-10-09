#include <iostream>
#include <string>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

int main(){

    CURL* curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.chucknorris.io/jokes/random");       

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        response = curl_easy_perform(curl);

        if(response == CURLE_OK){
            std::cout << "Response: " << readBuffer << "\n";
        }
        else{
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(response) << "\n";
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}