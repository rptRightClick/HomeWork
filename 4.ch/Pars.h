// здесь обьявленны все функции для парсинга json
#include "AllStruct.h"
#ifndef PARS
#define PARS

void CurrentJson(struct memory *mem, struct currentWeatherData *weatherData);//функция для парсинга текущей погоды

void OtherJson(struct memory *mem, struct OtherInformation *weatherData);// обработка прочих значений

#endif // PARS
