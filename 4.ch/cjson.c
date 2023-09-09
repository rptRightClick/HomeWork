#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>
#include <stdlib.h>

struct WeatherData {
    double temp_c;
    char* weather;
};

struct memory {
  char *response;
  size_t size;
};

// Функция для обработки ответа 
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL) {
        return 0;  // Ошибка выделения памяти!
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

// Функция для обработки JSON
void handlerJson(struct memory *mem, struct WeatherData *weatherData) {
    json_error_t error;
    json_t* json = json_loads(mem->response, 0, &error); // Заменено на json_loads

    printf("весь массив %s\n", mem->response);

    if (!json) {
        fprintf(stderr, "Error parsing JSON data: %s\n", error.text);
        return;
    }

    json_t* current = json_object_get(json, "current_condition");
    if (!current) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing 'current_condition'\n");
        return;
    }

    json_t* tempC = json_object_get(current, "temp_C");
    json_t* weatherDesc = json_object_get(current, "weatherDesc");
    if (!tempC || !weatherDesc) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing required fields\n");
        return;
    }

    weatherData->temp_c = json_real_value(tempC);
    const char* weatherStr = json_string_value(json_array_get(weatherDesc, 0));
    weatherData->weather = strdup(weatherStr);

    json_decref(json);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <city>\n", argv[0]);
        return 1;
    }

    char url[100];
    snprintf(url, sizeof(url), "https://wttr.in/%s?format=j1", argv[1]); // Формирование URL для запроса 

    CURL* curl;
    CURLcode res;
    struct WeatherData weatherData;
    weatherData.weather = NULL;

    struct memory chunk = {0}; // Создание структуры для хранения данных

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Error initializing libcurl\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url); // Установка URL
    printf("Запрос %s\n\n", url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Установка функции обратного вызова
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk); // Пользовательские данные для функции обратного вызова

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        handlerJson(&chunk, &weatherData); // Обработка JSON
        printf("Received data:\n");
        printf("Temperature: %.1f°C\n", weatherData.temp_c);
        printf("Weather: %s\n", weatherData.weather);
    }

    free(weatherData.weather);
    free(chunk.response); // Освобождение памяти для данных

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}








