#include <iostream>
#include <string>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <nlohmann/json.hpp>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string ReadUserInput(std::string userOptions[], std::string userOptionsLiteral[]);
double getValidatedCoordinate(const std::string& name, double min, double max);

int main(){

    CURL* curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        std::string userOptions[7];
        std::string userOptionsLiteral[7];
        std::string apiAddress = ReadUserInput(userOptions, userOptionsLiteral);

        curl_easy_setopt(curl, CURLOPT_URL, apiAddress.c_str()); // curl is more for C, so it doesn't know what a std::string is. You have to convert it into a char*
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        response = curl_easy_perform(curl);

        if(response == CURLE_OK){
            std::cout << "The chosen data for " << userOptionsLiteral[0].substr(0, userOptionsLiteral[0].find('.') + 3) << ", " << userOptionsLiteral[1].substr(0, userOptionsLiteral[1].find('.') + 3) << ":\n";

            nlohmann::json data = nlohmann::json::parse(readBuffer);
            const auto& current = data["current"];
            for (size_t i = 2; i < sizeof(userOptionsLiteral)/sizeof(std::string) && !userOptionsLiteral[i].empty(); ++i) {
                if (current.contains(userOptionsLiteral[i])) {
                    std::cout << userOptions[i] << current[userOptionsLiteral[i]] << '\n';
                } else {
                    std::cout << userOptions[i] << "Not available\n";
                }
            }

            
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

std::string ReadUserInput(std::string userOptions[], std::string userOptionsLiteral[]){
    
    std::string apiAddress = "https://api.open-meteo.com/v1/forecast?timezone=Europe%2FBerlin&latitude=";
    
    size_t userOptionsIndex = 0;

    double latitude = getValidatedCoordinate("Latitude", -90, 90);
    userOptionsLiteral[userOptionsIndex] = std::to_string(latitude);
    userOptionsIndex++;

    double longitude = getValidatedCoordinate("Longitude", -180, 180);
    userOptionsLiteral[userOptionsIndex] = std::to_string(longitude);
    userOptionsIndex++;

    apiAddress = apiAddress + std::to_string(latitude) + "&longitude=" + std::to_string(longitude) + "&current=";

    int input = 0;
    std::set<int> chosenOptions = {};
    while(true){
        std::cout << "Which data would you like to display? (0 to quit):" << "\n"
        "1. temperature" << "\n"
        "2. aparent temperature" << "\n"
        "3. humidity" << "\n"
        "4. wind speed" << "\n"
        "5. cloud cover" << "\n";

        std::string line;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        
        if(ss >> input && ss.eof()){
            std::cout << "You have chosen option " << input << "\n";
        }
        else{
            std::cout << "Please enter a valid number\n";
            continue;
        }

        if(input == 0)
            break;

        if(!chosenOptions.insert(input).second){
            std::cout << "You have already chosen option " << input << "\n";
            continue;
        }

        switch (input)
        {
        case 1:
            apiAddress += ",temperature_2m";
            userOptionsLiteral[userOptionsIndex] = "temperature_2m";
            userOptions[userOptionsIndex] = "Temperature (Celsius): ";
            userOptionsIndex++;
            break;
        case 2:
            apiAddress += ",apparent_temperature";
            userOptionsLiteral[userOptionsIndex] = "apparent_temperature";
            userOptions[userOptionsIndex] = "Apparent Temperature (Celsius): ";
            userOptionsIndex++;
            break;
        case 3:
            apiAddress += ",relative_humidity_2m";
            userOptionsLiteral[userOptionsIndex] = "relative_humidity_2m";
            userOptions[userOptionsIndex] = "Relative Humidity (%): ";
            userOptionsIndex++;
            break;
        case 4:
            apiAddress += ",wind_speed_10m";
            userOptionsLiteral[userOptionsIndex] = "wind_speed_10m";
            userOptions[userOptionsIndex] = "Wind Speed (km/h): ";
            userOptionsIndex++;
            break;
        case 5:
            apiAddress += ",cloud_cover";
            userOptionsLiteral[userOptionsIndex] = "cloud_cover";
            userOptions[userOptionsIndex] = "Cloud Cover (%): ";
            userOptionsIndex++;
            break;
        default:
            std::cout << "Please enter a number from 0-5" << "\n";
            continue;
        }

    }

    return apiAddress;
}

double getValidatedCoordinate(const std::string& name, double min, double max) {
    double value;
    std::string line;
    while (true) {
        std::cout << name << ": ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (ss >> value && ss.eof()){
            if (value >= min && value <= max) return value;
            std::cout << name << " must be between " << min << " and " << max << "\n";
        }
        else
            std::cout << "Please enter a valid number\n";
    }
}
