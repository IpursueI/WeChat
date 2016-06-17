#ifndef GET_WEATHER_H_
#define GET_WEATHER_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>

using namespace std;

string getWeather(const string &city);

#endif

