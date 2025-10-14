#include <iostream>
#include <string>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string ReadUserInput();

int main(){

    CURL* curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        std::string apiAddress = ReadUserInput();

        curl_easy_setopt(curl, CURLOPT_URL, apiAddress.c_str()); // curl expects is more for C, so it doesn't know what a std::string is. You have to convert it into a char*
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        response = curl_easy_perform(curl);

        if(response == CURLE_OK){
            std::cout << readBuffer << "\n";
            readBuffer.erase(0, readBuffer.find_last_of("l") + 24);
            //readBuffer.erase(readBuffer.length() - 2, readBuffer.length());

            //std::cout << "The temperature at " << latitude << " " << longitude << " is " << readBuffer << "c" << "\n";
            std::cout << "The temperature is " << readBuffer << "c" << "\n";
        }
        else{
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(response) << "\n";
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

std::string ReadUserInput(){
    
    std::string apiAddress = "https://api.open-meteo.com/v1/forecast?timezone=Europe%2FBerlin&latitude=";

        double latitude = 0;
        double longitude = 0;

        do{
            std::cout << "Latitude: ";
            std::cin >> latitude;
            if(latitude <= 90 && latitude >= -90)
                break;
            std::cout << "The latitude must be between -90 and 90\n";
        }while(true);

        do{
            std::cout << "Longitude: ";
            std::cin >> longitude;
            if(longitude <= 180 && longitude >= -180)
                break;
            std::cout << "The longitude must be between -180 and 180\n";
        }while(true);

        apiAddress = apiAddress + std::to_string(latitude) + "&longitude=" + std::to_string(longitude) + "&current=temperature_2m";

        int input = 0;
        //Maybe a hashmap to keep the selected options
        while(true){
            std::cout << "Which data would you like to display? (0 to quit):" << "\n";
            std::cout << "1. temperature" << "\n";
            std::cout << "2. aparent temperature" << "\n";
            std::cout << "3. humidity" << "\n";
            std::cout << "4. wind speed" << "\n";
            std::cout << "5. cloud cover" << "\n";

            std::cin >> input;

            if(input == 0)
                break;

            switch (input)
            {
            case 1:
                //apiAddress += ",temperature_2m";
                break;
            case 2:
                apiAddress += ",apparent_temperature";
                break;
            case 3:
                apiAddress += ",relative_humidity_2m";
                break;
            case 4:
                apiAddress += ",wind_speed_10m";
                break;
            case 5:
                apiAddress += ",cloud_cover";
                break;
            default:
                std::cout << "Please enter a number from 0-5" << "\n";
                break;
            }
        }

        return apiAddress;
}