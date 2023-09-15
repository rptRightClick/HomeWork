#include "Pars.h"
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <stdlib.h>
#include "AllStruct.h"
#include "Pars.h"


// Функция для обработки текущей погоды 
void CurrentJson(struct memory *mem, struct currentWeatherData *weatherData) {
	
    json_error_t error;
    json_t* json = json_loads(mem->response, 0, &error); 
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
    
    json_t* current0 = json_array_get(current, 0);
    if (!current0){
		json_decref(json);
		fprintf(stderr, "error current0\n");
		return;
	}
	
	// извлечение направления ветра
	json_t* ptrDirection = json_object_get(current0, "winddir16Point");
	// извлечение скорости ветра
	json_t* ptrSpeed = json_object_get(current0, "windspeedKmph");
	//извлечение текущей температуры
    json_t* tempC = json_object_get(current0, "temp_C");
    //извлечение текущей погоды
    json_t* weatherDesc = json_object_get(current0, "weatherDesc");
    json_t* arryDesc = json_array_get(weatherDesc,0);
    json_t* ptrValue = json_object_get(arryDesc, "value");

    
    if (!tempC || !ptrValue || !ptrDirection || !ptrSpeed || !weatherDesc || !arryDesc) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing required fields\n");
        return;
    }
	
	
	//назначение в структуру, состояние текущей температуры
	const char* tempCString = json_string_value(tempC);
	double temperatureC = atof(tempCString);
	weatherData->temp_c = temperatureC;
	
	//назначение в структуру, скорости ветра
	const char* speedStr = json_string_value(ptrSpeed);
	int speed_int = atoi(speedStr);
	weatherData->speed = speed_int;
	
	//назначение в структуру, направления ветра
	const char* directionStr = json_string_value(ptrDirection);
	weatherData->direction = strdup(directionStr);
	if (weatherData->direction == NULL) {
		fprintf(stderr, "Memory allocation failed for weatherData->weather\n");
		free(weatherData->direction);
		return;
}
	//назначение в структуру, состояние текущей погоды
    const char* weatherStr = json_string_value(ptrValue);
    weatherData->weather = strdup(weatherStr);
	if (weatherData->weather == NULL) {
		fprintf(stderr, "Memory allocation failed for weatherData->weather\n");
		free(weatherData->weather);
		return;
}


    json_decref(json);
}


//функция для обработки прочих значений
// функция аналогична CurrentJson

void OtherJson(struct memory *mem, struct OtherInformation *OtherData){
	
	json_error_t error;
    json_t* json = json_loads(mem->response, 0, &error); 
	if (!json) {
        fprintf(stderr, "Error parsing JSON data: %s\n", error.text);
        return;
    }
// в данном куске мы парсим название города
    json_t* area = json_object_get(json, "nearest_area");
    if (!area) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing 'nearest_area'\n");
        return;
    }
    
    json_t* areaArrow = json_array_get(area, 0);
    if (!areaArrow){
		json_decref(json);
		fprintf(stderr, "error areaArrow\n");
		return;
	}
	
	json_t* areaName = json_object_get(areaArrow, "areaName");
    if (!areaName) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing 'areaName'\n");
        return;
    }
    
    json_t* areaNameArrow = json_array_get(areaName, 0);
    if (!areaNameArrow){
		json_decref(json);
		fprintf(stderr, "error areaNameArrow\n");
		return;
	}
	
	json_t* ptrCity = json_object_get(areaNameArrow, "value");
	if (!ptrCity){
		json_decref(json);
		fprintf(stderr,"JSON data is missing required fields\n");
	}
	
	const char* cityStr = json_string_value(ptrCity);
	OtherData->city = strdup(cityStr);
	if (OtherData->city == NULL){
		fprintf(stderr, "memery error");
		free(OtherData->city);
		return;
	}
	
	
	json_t* weather = json_object_get(json, "weather");
    if (!weather) {
        json_decref(json);
        fprintf(stderr, "JSON data is missing 'nearest_area'\n");
        return;
    }
    
    json_t* weatherArrow = json_array_get(weather, 0);
    if (!weatherArrow){
		json_decref(json);
		fprintf(stderr, "error areaArrow\n");
		return;
	}
	
	json_t* ptrData = json_object_get(weatherArrow, "date");
	if (!ptrData){
		json_decref(json);
		fprintf(stderr,"JSON data is missing required fields\n");
	}
	
	const char* dataStr = json_string_value(ptrData);
	OtherData->currentData = strdup(dataStr);
	if (OtherData->currentData == NULL){
		fprintf(stderr, "memery error");
		free(OtherData->currentData);
		return;
	}
	
	json_decref(json);
}
	
	
	
