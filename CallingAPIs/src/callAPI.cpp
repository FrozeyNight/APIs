#include <iostream>
#include <string>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>
#include "CallAPI.h"

size_t CallAPI::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::vector<std::string> CallAPI::RunMyWeather(int argc, char* argv[]){

    std::string userOptions[7];
    std::string userOptionsLiteral[7];

    std::string apiAddress = "";

    apiAddress = "https://api.open-meteo.com/v1/forecast?timezone=Europe%2FBerlin&latitude=";
    size_t userOptionsIndex = 0;

    double latitude = 0;
    double longitude = 0;
    std::string arguments[argc];

    for(int i = 1; i < argc; i++){
        arguments[i - 1] = argv[i];
    }

    if(arguments[0] == "-gc"){
        std::string geolocationData;
        if(callAPI("http://ip-api.com/json/", &geolocationData)){
            nlohmann::json data = nlohmann::json::parse(geolocationData);
            latitude = data["lat"];
            userOptionsLiteral[userOptionsIndex] = std::to_string(latitude);
            userOptionsIndex = userOptionsIndex + 1;

            longitude = data["lon"];
            userOptionsLiteral[userOptionsIndex] = std::to_string(longitude);
            userOptionsIndex = userOptionsIndex + 1;
        }
    }
    else if(arguments[0][0] == '-' && arguments[0][1] == 'c'){
        std::string argument = arguments[0];
        latitude = std::stod(argument.substr(2, argument.find(',') - 2));
        userOptionsLiteral[userOptionsIndex] = std::to_string(latitude);
        userOptionsIndex = userOptionsIndex + 1;

        longitude = std::stod(argument.substr(argument.find(',') + 1, argument.length() - argument.find(',') - 1));
        userOptionsLiteral[userOptionsIndex] = std::to_string(longitude);
        userOptionsIndex = userOptionsIndex + 1;
    }
    else{
        getUserCoordinates(&latitude, &longitude, userOptionsLiteral, &userOptionsIndex);
    }

    apiAddress = apiAddress + std::to_string(latitude) + "&longitude=" + std::to_string(longitude) + "&current=";

    if(arguments[1] == "-ao" || arguments[0] == "-ao"){
        for (size_t i = 1; i < 6; i++)
        {
            saveUserInput(i, userOptionsLiteral, userOptions, &userOptionsIndex, &apiAddress);
        }
    }
    else{
        getUserOptions(userOptionsLiteral, userOptions, &userOptionsIndex, &apiAddress);
    }

    std::string weatherData = "";
    std::vector<std::string> formattedWeatherData;
    if(callAPI(apiAddress, &weatherData)){
        //std::cout << "The chosen data for " << userOptionsLiteral[0].substr(0, userOptionsLiteral[0].find('.') + 3) << ", " << userOptionsLiteral[1].substr(0, userOptionsLiteral[1].find('.') + 3) << ":\n";

        nlohmann::json data = nlohmann::json::parse(weatherData);
        const auto& current = data["current"];
        for (size_t i = 2; i < sizeof(userOptionsLiteral)/sizeof(std::string) && !userOptionsLiteral[i].empty(); ++i) {
            if (current.contains(userOptionsLiteral[i])) {
                formattedWeatherData.push_back(userOptions[i] + current[userOptionsLiteral[i]].dump());
                //std::cout << userOptions[i] << current[userOptionsLiteral[i]] << '\n';
            } else {
                //std::cout << userOptions[i] << "Not available\n";
                formattedWeatherData.push_back(userOptions[i] + "Not available");
            }
        }
    }
    
    return formattedWeatherData;
}

double CallAPI::getValidatedCoordinate(const std::string& name, double min, double max) {
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

void CallAPI::saveUserInput(int input, std::string userOptionsLiteral[], std::string userOptions[], size_t *userOptionsIndex, std::string *apiAddress){
    switch (input)
    {
        case 1:
            *apiAddress += ",temperature_2m";
            userOptionsLiteral[*userOptionsIndex] = "temperature_2m";
            userOptions[*userOptionsIndex] = "Temperature (Celsius): ";
            *userOptionsIndex = *userOptionsIndex + 1;
            break;
        case 2:
            *apiAddress += ",apparent_temperature";
            userOptionsLiteral[*userOptionsIndex] = "apparent_temperature";
            userOptions[*userOptionsIndex] = "Apparent Temperature (Celsius): ";
            *userOptionsIndex = *userOptionsIndex + 1;
            break;
        case 3:
            *apiAddress += ",relative_humidity_2m";
            userOptionsLiteral[*userOptionsIndex] = "relative_humidity_2m";
            userOptions[*userOptionsIndex] = "Relative Humidity (%): ";
            *userOptionsIndex = *userOptionsIndex + 1;
            break;
        case 4:
            *apiAddress += ",wind_speed_10m";
            userOptionsLiteral[*userOptionsIndex] = "wind_speed_10m";
            userOptions[*userOptionsIndex] = "Wind Speed (km/h): ";
            *userOptionsIndex = *userOptionsIndex + 1;
            break;
        case 5:
            *apiAddress += ",cloud_cover";
            userOptionsLiteral[*userOptionsIndex] = "cloud_cover";
            userOptions[*userOptionsIndex] = "Cloud Cover (%): ";
            *userOptionsIndex = *userOptionsIndex + 1;
            break;
        default:
            std::cout << "Please enter a number from 0-5" << "\n";
    }
}

void CallAPI::getUserOptions(std::string userOptionsLiteral[], std::string userOptions[], size_t *userOptionsIndex, std::string *apiAddress){
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

        saveUserInput(input, userOptionsLiteral, userOptions, userOptionsIndex, apiAddress);
    }

}

void CallAPI::getUserCoordinates(double *latitude, double *longitude, std::string userOptionsLiteral[], size_t *userOptionsIndex){
    *latitude = getValidatedCoordinate("Latitude", -90, 90);
    userOptionsLiteral[*userOptionsIndex] = std::to_string(*latitude);
    *userOptionsIndex = *userOptionsIndex + 1;

    *longitude = getValidatedCoordinate("Longitude", -180, 180);
    userOptionsLiteral[*userOptionsIndex] = std::to_string(*longitude);
    *userOptionsIndex = *userOptionsIndex + 1;
}

bool CallAPI::callAPI(std::string apiAddress, std::string *output){
    CURL* curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, apiAddress.c_str()); // curl is more for C, so it doesn't know what a std::string is. You have to convert it into a char*
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        response = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if(response == CURLE_OK){
            *output = readBuffer;
            return true;
        }
        else{
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(response) << "\n";
            return false;
        }

    }

    return false;
}