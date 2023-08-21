#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


typedef struct {
	uint8_t cp1251;
	uint8_t koi8r;
	uint8_t iso885;
	uint16_t unicode;
}ConversionEntry;

ConversionEntry Table[] = {
	{0xC0, 0xE1, 0xB0, 0x0410},
    {0xC1, 0xE2, 0xB1, 0x0411},
    {0xC2, 0xF7, 0xB2, 0x0412},
    {0xC3, 0xE7, 0xB3, 0x0413},
    {0xC4, 0xE4, 0xB4, 0x0414},
    {0xC5, 0xE5, 0xB5, 0x0415},
    {0xC6, 0xF6, 0xB6, 0x0416},
    {0xC7, 0xFA, 0xB7, 0x0417},
    {0xC8, 0xE9, 0xB8, 0x0418},
    {0xC9, 0xEA, 0xB9, 0x0419},
    {0xCA, 0xEB, 0xBA, 0x041A},
    {0xCB, 0xEC, 0xBB, 0x041B},
    {0xCC, 0xED, 0xBC, 0x041C},
    {0xCD, 0xEE, 0xBD, 0x041D},
    {0xCE, 0xEF, 0xBE, 0x041E},
    {0xCF, 0xF0, 0xBF, 0x041F},
    {0xD0, 0xF2, 0xC0, 0x0420},
    {0xD1, 0xF3, 0xC1, 0x0421},
    {0xD2, 0xF4, 0xC2, 0x0422},
    {0xD3, 0xF5, 0xC3, 0x0423},
    {0xD4, 0xE6, 0xC4, 0x0424},
    {0xD5, 0xE8, 0xC5, 0x0425},
    {0xD6, 0xE3, 0xC6, 0x0426},
    {0xD7, 0xFE, 0xC7, 0x0427},
    {0xD8, 0xFB, 0xC8, 0x0428},
    {0xD9, 0xFD, 0xC9, 0x0429},
    {0xDA, 0xFF, 0xCA, 0x042A},
    {0xDB, 0xF9, 0xCB, 0x042B},
    {0xDC, 0xF8, 0xCC, 0x042C},
    {0xDD, 0xFC, 0xCD, 0x042D},
    {0xDE, 0xE0, 0xCE, 0x042E},
    {0xDF, 0xF1, 0xCF, 0x042F},
    {0xE0, 0xC1, 0xD0, 0x0430},
    {0xE1, 0xC2, 0xD1, 0x0431},
    {0xE2, 0xD7, 0xD2, 0x0432},
    {0xE3, 0xC7, 0xD3, 0x0433},
    {0xE4, 0xC4, 0xD4, 0x0434},
    {0xE5, 0xC5, 0xD5, 0x0435},
    {0xE6, 0xD6, 0xD6, 0x0436},
    {0xE7, 0xDA, 0xD7, 0x0437},
    {0xE8, 0xC9, 0xD8, 0x0438},
    {0xE9, 0xCA, 0xD9, 0x0439},
    {0xEA, 0xCB, 0xDA, 0x043A},
    {0xEB, 0xCC, 0xDB, 0x043B},
    {0xEC, 0xCD, 0xDC, 0x043C},
    {0xED, 0xCE, 0xDD, 0x043D},
    {0xEE, 0xCF, 0xDE, 0x043E},
    {0xEF, 0xD0, 0xDF, 0x043F},
    {0xF0, 0xD2, 0xE0, 0x0440},
    {0xF1, 0xD3, 0xE1, 0x0441},
    {0xF2, 0xD4, 0xE2, 0x0442},
    {0xF3, 0xD5, 0xE3, 0x0443},
    {0xF4, 0xC6, 0xE4, 0x0444},
    {0xF5, 0xC8, 0xE5, 0x0445},
    {0xF6, 0xC3, 0xE6, 0x0446},
    {0xF7, 0xDE, 0xE7, 0x0447},
    {0xF8, 0xDB, 0xE8, 0x0448},
    {0xF9, 0xDD, 0xE9, 0x0449},
    {0xFA, 0xDF, 0xEA, 0x044A},
    {0xFB, 0xD9, 0xEB, 0x044B},
    {0xFC, 0xD8, 0xEC, 0x044C},
    {0xFD, 0xDC, 0xED, 0x044D},
    {0xFE, 0xC0, 0xEE, 0x044E},
    {0xFF, 0xD1, 0xEF, 0x044F},
    {0x2C, 0x2C, 0x2C, 0x002C},
    {0x2E, 0x2E, 0x2E, 0x002E},
    {0x5B, 0x5B, 0x5B, 0x005B},
    {0x5D, 0x5D, 0x5D, 0x005D},
    {0x28, 0x28, 0x28, 0x0028},
    {0x29, 0x29, 0x29, 0x0029},
    {0x30, 0x30, 0x30, 0x0030},
    {0x31, 0x31, 0x31, 0x0031},
    {0x32, 0x32, 0x32, 0x0032},
    {0x33, 0x33, 0x33, 0x0033},
    {0x34, 0x34, 0x34, 0x0034},
    {0x35, 0x35, 0x35, 0x0035},
    {0x36, 0x36, 0x36, 0x0036},
    {0x37, 0x37, 0x37, 0x0037},
    {0x38, 0x38, 0x38, 0x0038},
    {0x39, 0x39, 0x39, 0x0039},
    {0x20, 0x20, 0x20, 0x0020}
};

// Преобразует символ Unicode в последовательность байтов UTF-8.
// Параметры:
//   - unicode: символ Unicode, который необходимо преобразовать.
//   - utf8_buffer: указатель на буфер, в который будут записаны байты UTF-8.
//                  Должен быть достаточно большим, чтобы вместить все байты.(максимальная длинна ютф-8  4 байта) 
//   - utf8_size: указатель на переменную, в которую будет записан размер
//                последовательности байтов UTF-8.
void unicode_to_utf8(uint16_t unicode, uint8_t *utf8_buffer, size_t *utf8_size) {
    if (unicode <= 0x7F) {
		// Если символ входит в диапазон ASCII (0x00-0x7F), он представляется
        // одним байтом в UTF-8.
        utf8_buffer[0] = (uint8_t)unicode;
        *utf8_size = 1;		// Размер байта UTF-8		
    } else if (unicode <= 0x7FF) {
		// Если символ не входит в диапазон ASCII, но вмещается в 11 бит,
        // он представляется двумя байтами в UTF-8.
        utf8_buffer[0] = 0xC0 | (unicode >> 6);// Первый байт: 110xxxxx
        utf8_buffer[1] = 0x80 | (unicode & 0x3F);// Второй байт: 10xxxxxx
        *utf8_size = 2;// Размер байтов UTF-8
    } else {
		// Если символ занимает более 11 бит, он представляется тремя байтами
        // в UTF-8.
        utf8_buffer[0] = 0xE0 | (unicode >> 12);
        utf8_buffer[1] = 0x80 | ((unicode >> 6) & 0x3F);
        utf8_buffer[2] = 0x80 | (unicode & 0x3F);
        *utf8_size = 3;// Размер байтов UTF-8
    }
}


void cp1251_to_unicode(char *input_file,char *output_file){
	
	FILE *cp1251 = fopen(input_file,"rb");
	if (!cp1251){
		perror("ошибка при открытии входного файла");
		return;
	}
	
	size_t numEntries = sizeof(Table)/sizeof(Table[0]);//количество элементов в таблице
	
	// вычисление размера файла
	fseek(cp1251,0,SEEK_END);
	size_t fileSize = ftell(cp1251);
	fseek(cp1251,0,SEEK_SET);
	
	
	// выделение буфера для чтения и конвертации
	uint8_t *input_buffer = (uint8_t *)calloc(fileSize, sizeof(uint8_t));
    if (input_buffer == NULL) {
        perror("Не удалось выделить память для буфера");
        fclose(cp1251);
        return;
    }
	
	// Читаем весь файл в буфер
    fread(input_buffer, sizeof(uint8_t), fileSize, cp1251);
    if (ferror(cp1251)){
		printf("Произошла ошибка чтения файла.\n");
		fclose(cp1251);
		return;
	}
    fclose(cp1251);
	
	 //Подсчитываем количество символов Unicode, которые могут быть созданы
	 //это количество символов, которые мы создадим в результате конвертации. 
	 //Это значение нам нужно знать, чтобы правильно выделить память под буфер unicode_buffer, 
	 //который будет хранить символы Unicode после конвертации.
    size_t numUnicodeChars = 0;
    for (size_t i = 0; i < fileSize; i++) {// двигаемся по таблице 
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].cp1251) {
                numUnicodeChars++;//именно в этой переменной и будет храниться количество символов
                break;
            }
        }
    }
    
    // Выделяем буфер для Unicode символов
    uint16_t *unicode_buffer = (uint16_t *)calloc(numUnicodeChars, sizeof(uint16_t));
    if (unicode_buffer == NULL) {
        perror("Не удалось выделить память для буфера Unicode");
        free(input_buffer);
        return;
    }
    
    // Конвертируем CP1251 в Unicode
    //после завершения цикла, буфер unicode_buffer будет содержать сконвертированные символы Unicode в правильной последовательности.
    size_t unicodeIndex = 0;//используется для отслеживания текущей позиции в буфере unicode_buffer
    for (size_t i = 0; i < fileSize; i++) {
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].cp1251) {
                unicode_buffer[unicodeIndex++] = Table[j].unicode;
                break;
            }
        }
    }
    
    
    //Здесь мы используем временный буфер temp_buffer,
    // чтобы передать его в функцию unicode_to_utf8 для вычисления размера UTF-8 символа без фактической записи его в буфер.
    // Поскольку нам нужен только размер каждого символа
    size_t utf8_index = 0;
// Подсчитываем общий размер буфера UTF-8
for (size_t i = 0; i < numUnicodeChars; i++) {//numUnicodeChars количество символов в юникод
    size_t utf8_size;
    uint8_t temp_buffer[4]; // временный буфер, не используется для записи
    unicode_to_utf8(unicode_buffer[i], temp_buffer, &utf8_size); // Подсчитываем размер UTF-8 символа
    utf8_index += utf8_size;
}


// Создаем буфер для хранения UTF-8 символов
uint8_t *utf8_buffer = (uint8_t *)malloc(utf8_index * sizeof(uint8_t));// тут мы как раз используем utf8_index для выделения места под массив 
if (utf8_buffer == NULL) {
    perror("Не удалось выделить память для буфера UTF-8");
    free(input_buffer);
    free(unicode_buffer);
    return;
}

size_t utf8_offset = 0;//переменная будет использоваться для отслеживания текущей позиции в буфере utf8_buffer
size_t utf8_size;
// Преобразуем Unicode в UTF-8 и записываем в буфер
for (size_t i = 0; i < numUnicodeChars; i++) {
    unicode_to_utf8(unicode_buffer[i], utf8_buffer + utf8_offset, &utf8_size);//преобразует текущий символ Unicode в его представление в формате UTF-8
    utf8_offset += utf8_size;// utf8_offset обеспечивает правильное размещение символов в буфере
}



    // Открываем выходной файл для записи
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Ошибка при открытии выходного файла");
        free(utf8_buffer);
        free(input_buffer);
        free(unicode_buffer);
        return;
    }

   
// Выводим результат (байты UTF-8) в выходной файл
fwrite(utf8_buffer, sizeof(uint8_t), utf8_index, output);

    // Закрываем выходной файл
    fclose(output);

    // Освобождаем память
    free(utf8_buffer);
    free(input_buffer);
    free(unicode_buffer);
    
	return;
}	

void iso885_to_unicode(char *input_file,char *output_file){
	FILE *iso885 = fopen(input_file,"rb");
	if (!iso885){
		perror("ошибка при открытии входного файла");
		return;
	}
	
	size_t numEntries = sizeof(Table)/sizeof(Table[0]);//количество элементов в таблице
	
	// вычисление размера файла
	fseek(iso885,0,SEEK_END);
	size_t fileSize = ftell(iso885);
	fseek(iso885,0,SEEK_SET);
	
	
	// выделение буфера для чтения и конвертации
	uint8_t *input_buffer = (uint8_t *)calloc(fileSize, sizeof(uint8_t));
    if (input_buffer == NULL) {
        perror("Не удалось выделить память для буфера");
        fclose(iso885);
        return;
    }
	
	// Читаем весь файл в буфер
    fread(input_buffer, sizeof(uint8_t), fileSize, iso885);
    if (ferror(iso885)){
		printf("Произошла ошибка чтения файла.\n");
		fclose(iso885);
		return;
	}
    fclose(iso885);
	
	 //Подсчитываем количество символов Unicode, которые могут быть созданы
	 //это количество символов, которые мы создадим в результате конвертации. 
	 //Это значение нам нужно знать, чтобы правильно выделить память под буфер unicode_buffer, 
	 //который будет хранить символы Unicode после конвертации.
    size_t numUnicodeChars = 0;
    for (size_t i = 0; i < fileSize; i++) {// двигаемся по таблице 
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].iso885) {
                numUnicodeChars++;//именно в этой переменной и будет храниться количество символов
                break;
            }
        }
    }
    
    // Выделяем буфер для Unicode символов
    uint16_t *unicode_buffer = (uint16_t *)calloc(numUnicodeChars, sizeof(uint16_t));
    if (unicode_buffer == NULL) {
        perror("Не удалось выделить память для буфера Unicode");
        free(input_buffer);
        return;
    }
    
    // Конвертируем iso885 в Unicode
    //после завершения цикла, буфер unicode_buffer будет содержать сконвертированные символы Unicode в правильной последовательности.
    size_t unicodeIndex = 0;//используется для отслеживания текущей позиции в буфере unicode_buffer
    for (size_t i = 0; i < fileSize; i++) {
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].iso885) {
                unicode_buffer[unicodeIndex++] = Table[j].unicode;
                break;
            }
        }
    }
    
    
    //Здесь мы используем временный буфер temp_buffer,
    // чтобы передать его в функцию unicode_to_utf8 для вычисления размера UTF-8 символа без фактической записи его в буфер.
    // Поскольку нам нужен только размер каждого символа
    size_t utf8_index = 0;
// Подсчитываем общий размер буфера UTF-8
for (size_t i = 0; i < numUnicodeChars; i++) {//numUnicodeChars количество символов в юникод
    size_t utf8_size;
    uint8_t temp_buffer[4]; // временный буфер, не используется для записи
    unicode_to_utf8(unicode_buffer[i], temp_buffer, &utf8_size); // Подсчитываем размер UTF-8 символа
    utf8_index += utf8_size;
}


// Создаем буфер для хранения UTF-8 символов
uint8_t *utf8_buffer = (uint8_t *)malloc(utf8_index * sizeof(uint8_t));// тут мы как раз используем utf8_index для выделения места под массив 
if (utf8_buffer == NULL) {
    perror("Не удалось выделить память для буфера UTF-8");
    free(input_buffer);
    free(unicode_buffer);
    return;
}

size_t utf8_offset = 0;//переменная будет использоваться для отслеживания текущей позиции в буфере utf8_buffer
size_t utf8_size;
// Преобразуем Unicode в UTF-8 и записываем в буфер
for (size_t i = 0; i < numUnicodeChars; i++) {
    unicode_to_utf8(unicode_buffer[i], utf8_buffer + utf8_offset, &utf8_size);//преобразует текущий символ Unicode в его представление в формате UTF-8
    utf8_offset += utf8_size;// utf8_offset обеспечивает правильное размещение символов в буфере
}



    // Открываем выходной файл для записи
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Ошибка при открытии выходного файла");
        free(utf8_buffer);
        free(input_buffer);
        free(unicode_buffer);
        return;
    }

   
// Выводим результат (байты UTF-8) в выходной файл
fwrite(utf8_buffer, sizeof(uint8_t), utf8_index, output);

    // Закрываем выходной файл
    fclose(output);

    // Освобождаем память
    free(utf8_buffer);
    free(input_buffer);
    free(unicode_buffer);
    
	return;
}	

void koi8r_to_unicode(char *input_file,char *output_file){
	FILE *koi8r = fopen(input_file,"rb");
	if (!koi8r){
		perror("ошибка при открытии входного файла");
		return;
	}
	
	size_t numEntries = sizeof(Table)/sizeof(Table[0]);//количество
	
	// вычисление размера файла
	fseek(koi8r,0,SEEK_END);
	size_t fileSize = ftell(koi8r);
	fseek(koi8r,0,SEEK_SET);
	
	// выделение буфера для чтения и конвертации
	uint8_t *input_buffer = (uint8_t *)calloc(fileSize, sizeof(uint8_t));
    if (input_buffer == NULL) {
        perror("Не удалось выделить память для буфера");
        fclose(koi8r);
        return;
    }

// Читаем весь файл в буфер
    fread(input_buffer, sizeof(uint8_t), fileSize, koi8r);
    if (ferror(koi8r)){
		printf("Произошла ошибка чтения файла.\n");
		fclose(koi8r);
		return;
	}
    fclose(koi8r);
    
    
     //Подсчитываем количество символов Unicode, которые могут быть созданы
	 //это количество символов, которые мы создадим в результате конвертации. 
	 //Это значение нам нужно знать, чтобы правильно выделить память под буфер unicode_buffer, 
	 //который будет хранить символы Unicode после конвертации.
    size_t numUnicodeChars = 0;
    for (size_t i = 0; i < fileSize; i++) {// двигаемся по таблице 
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].koi8r) {
                numUnicodeChars++;//именно в этой переменной и будет храниться количество символов
                break;
            }
        }
    }
    
    // Выделяем буфер для Unicode символов
    uint16_t *unicode_buffer = (uint16_t *)calloc(numUnicodeChars, sizeof(uint16_t));
    if (unicode_buffer == NULL) {
        perror("Не удалось выделить память для буфера Unicode");
        free(input_buffer);
        return;
    }
    
   // Конвертируем koi8r в Unicode
    //после завершения цикла, буфер unicode_buffer будет содержать сконвертированные символы Unicode в правильной последовательности.
    size_t unicodeIndex = 0;//используется для отслеживания текущей позиции в буфере unicode_buffer
    for (size_t i = 0; i < fileSize; i++) {
        for (size_t j = 0; j < numEntries; j++) {
            if (input_buffer[i] == Table[j].koi8r) {
                unicode_buffer[unicodeIndex++] = Table[j].unicode;
                break;
            }
        }
    } 
    
    
    //Здесь мы используем временный буфер temp_buffer,
    // чтобы передать его в функцию unicode_to_utf8 для вычисления размера UTF-8 символа без фактической записи его в буфер.
    // Поскольку нам нужен только размер каждого символа
    size_t utf8_index = 0;
// Подсчитываем общий размер буфера UTF-8
for (size_t i = 0; i < numUnicodeChars; i++) {//numUnicodeChars количество символов в юникод
    size_t utf8_size;
    uint8_t temp_buffer[4]; // временный буфер, не используется для записи
    unicode_to_utf8(unicode_buffer[i], temp_buffer, &utf8_size); // Подсчитываем размер UTF-8 символа
    utf8_index += utf8_size;
}
    
    // Создаем буфер для хранения UTF-8 символов
uint8_t *utf8_buffer = (uint8_t *)malloc(utf8_index * sizeof(uint8_t));// тут мы как раз используем utf8_index для выделения места под массив 
if (utf8_buffer == NULL) {
    perror("Не удалось выделить память для буфера UTF-8");
    free(input_buffer);
    free(unicode_buffer);
    return;
}
    
    
    size_t utf8_offset = 0;//переменная будет использоваться для отслеживания текущей позиции в буфере utf8_buffer
size_t utf8_size;
// Преобразуем Unicode в UTF-8 и записываем в буфер
for (size_t i = 0; i < numUnicodeChars; i++) {
    unicode_to_utf8(unicode_buffer[i], utf8_buffer + utf8_offset, &utf8_size);//преобразует текущий символ Unicode в его представление в формате UTF-8
    utf8_offset += utf8_size;// utf8_offset обеспечивает правильное размещение символов в буфере
}



    // Открываем выходной файл для записи
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Ошибка при открытии выходного файла");
        free(utf8_buffer);
        free(input_buffer);
        free(unicode_buffer);
        return;
    }

   
// Выводим результат (байты UTF-8) в выходной файл
fwrite(utf8_buffer, sizeof(uint8_t), utf8_index, output);

    // Закрываем выходной файл
    fclose(output);

    // Освобождаем память
    free(utf8_buffer);
    free(input_buffer);
    free(unicode_buffer);
    
	return;
}	
    
    
    

int main(int argc, char *argv[]){
	if(argc != 4){
		printf("Использование: %s <входной файл> <кодировка> <выходной файл>\n",argv[0]);
		printf("кодировка должна быть:одна из CP-1251, KOI8-R, ISO-8859-5\n");
		return 1;
	}
	
	char *input_file_name = argv[1];
	const char *input_encoding = argv[2];
	char *output_file_name = argv[3];
	
	
	FILE *input_file = fopen(input_file_name,"rb");
	if (!input_file){
		perror("ошибка при открытии входного файла\n");
		return 1;
	}
	
	FILE *output_file = fopen(output_file_name,"wb");
	if (!output_file){
		perror("ошибка при открытии выходного файла\n");
		return 1;
	}
	
	
	if (strcmp(input_encoding, "CP-1251")==0){
		cp1251_to_unicode(input_file_name,output_file_name);
		printf("готово CP-1251 конвертирована в UTF-8\n");
		fclose(input_file);
		fclose(output_file);
		return 0;
	}
	else if (strcmp(input_encoding, "KOI8-R")==0){
		koi8r_to_unicode(input_file_name,output_file_name);
		printf("готово KOI8-R конвертирована в UTF-8\n");
		fclose(input_file);
		fclose(output_file);
		return 0;
	}
	else if (strcmp(input_encoding,"ISO-8859-5")==0){
		iso885_to_unicode(input_file_name,output_file_name);
		printf("готово  (ISO-8859-5 конвертирована в UTF-8)\n");
		fclose(input_file);
		fclose(output_file);
		return 0;
	}
	printf("неверная кодировка\n");
	fclose(input_file);
	fclose(output_file);
	return 0;
}



		
	

































