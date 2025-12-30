#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>

class CallAPI{
public:
    static std::vector<std::string> RunMyWeather(int argc, char* argv[]);
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    static double getValidatedCoordinate(const std::string& name, double min, double max);
    static void saveUserInput(int input, std::string userOptionsLiteral[], std::string userOptions[], size_t *userOptionsIndex, std::string *apiAddress);
    static void getUserOptions(std::string userOptionsLiteral[], std::string userOptions[], size_t *userOptionsIndex, std::string *apiAddress);
    static void getUserCoordinates(double *latitude, double *longitude, std::string userOptionsLiteral[], size_t *userOptionsIndex);
    static bool callAPI(std::string apiAddress, std::string *output);
};
