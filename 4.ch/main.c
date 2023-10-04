#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>
#include <stdlib.h>
#include "AllStruct.h"
#include "Pars.h"



// Функция для обработки ответа ,которая сохраняет весь json в структуру
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



int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <city>\n", argv[0]);
        return 1;
    }

    char url[100];
    snprintf(url, sizeof(url), "https://wttr.in/%s?format=j1", argv[1]); // Формирование URL для запроса 

    CURL* curl;
    CURLcode res;
 //____________________________________________________________________________________________________________________

    struct currentWeatherData *ptrWeatherData = (struct currentWeatherData *)malloc(sizeof(struct currentWeatherData));// структура для текущей погоды
		if (ptrWeatherData == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			return 1;
		}
	struct memory chunk = {0}; // Создание структуры для хранения данных
	
	struct OtherInformation *ptrOInformation = (struct OtherInformation*)malloc(sizeof(struct OtherInformation));// указатель на структуру для прочей информации
		if (ptrOInformation==NULL){
			fprintf(stderr,"Memory allocation failed\n");
			return 1;
		}
		
//____________________________________________________________________________________________________________________
	

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Error initializing libcurl\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url); // Установка URL
   // printf("Запрос %s\n\n", url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Установка функции обратного вызова
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk); // Пользовательские данные для функции обратного вызова

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        CurrentJson(&chunk, ptrWeatherData); // Обработка текущей погоды
        OtherJson(&chunk, ptrOInformation);// обработка прочих значений
        printf("┌──────────────────────────────┐\n");
        printf("│          %-20s│\n",ptrOInformation->city);
        printf("├──────────────────────────────┤\n");
        printf("│       Current Weather        │\n");
        printf("├──────────────────────────────┤\n");
        printf("│          %-20s│\n", ptrWeatherData->weather);
        printf("│       Temperature:%4.1f°C     │\n", ptrWeatherData->temp_c);
        printf("│       Wind Direction: %-7s│\n", ptrWeatherData->direction);
        printf("│       Wind Speed:%2d km/h     │\n",ptrWeatherData->speed);
        printf("├──────────────────────────────┤\n");
        printf("│          %-20s│\n", ptrOInformation->currentData);
        printf("└──────────────────────────────┘\n");
    }
             
    
	free(ptrOInformation);
	free(ptrWeatherData);
    free(chunk.response); // Освобождение памяти для данных

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}








