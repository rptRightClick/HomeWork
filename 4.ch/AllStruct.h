#include <stdlib.h>
//здесь объявлены все структуры, которые нужны нам
#ifndef ALL_STRUCT
#define ALL_STRUCT

struct OtherInformation{
	char* city;
	char* currentData;
};

struct currentWeatherData {
    double temp_c;
    char* weather;
    char* direction;
    int speed;
};


struct memory {
  char *response;
  size_t size;
};

#endif // ALL_STRUCT
