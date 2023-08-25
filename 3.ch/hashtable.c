#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //для работы с нижним регистром

#define TABLE_SIZE 20//размер таблицы, специально малый размер, что бы продемонстрировать изменение размера
#define MAX_WORD_LENGTH 100

const float porog = 0.75; // коэффициент заполненности таблицы

//структура для хранения данных в хэш таблице
typedef struct {
	char *key;
	unsigned int value;
} KeyValuePair;

typedef struct {
	KeyValuePair *data;
	size_t sizeTable;
	size_t count; // переменная для слежения заполненности таблицы
} HashTable;

// простая хэш функция
unsigned int hash(const char *key, size_t size){
	unsigned int hash = 5381;
	for(size_t i = 0; key[i] != '\0';i++){
		if(isalnum(key[i])){//исключаем знаки препинания (не работает, не знаю почему) , думаю из-за кодировки , но не уверен
			hash = hash * 33 << key[i];
		}
	}
	
	
	return hash%size;
}

// Вставка пары ключ-значение в хэш-таблицу
void insert(HashTable *table, const char *key, unsigned int value){
	unsigned int index = hash(key, table->sizeTable);
	
	while (table->data[index].key != NULL){
		if (strcmp(table->data[index].key,key)==0){//проверяем занята ли ячейка этим же словом
			table->data[index].value+=value;//если да,то добавляем одно  повторение
			return;
		}
		index = (index + 1)%table->sizeTable;
	}
	table->data[index].key = strdup(key);
	table->data[index].value = value;
	table->count++;
}


// данная функция отличается от insert тем, что не происходит добавления количество повторений, а просто идет заполнение таблицы (копирование)
void insertInternal(HashTable *table,const char *key, int value){
	unsigned int index = hash(key, table->sizeTable);
	
	while(table->data[index].key != NULL){
		index = (index+1)%table->sizeTable;
	}
	
	table->data[index].key = strdup(key);
	table->data[index].value = value;
	table->count++;
}
	
	

// функция проверяет процент заполненности таблицы
void checkResize(HashTable *table, float loadFactor){
	float a = (float)table->count/(float)table->sizeTable;// не больше 0.75
	
	if(a>=loadFactor){
		size_t newSize = table->sizeTable*2;// если таблица заполнена на 75%, то мы ее увеличиваем в два раза
		
		HashTable newTable;//создание новой таблицы
		newTable.sizeTable = newSize;
		newTable.data = (KeyValuePair*)calloc(newSize, sizeof(KeyValuePair)); //выделение места под новую таблицу
		newTable.count = 0;
		
		for (size_t i = 0; i<table->sizeTable;i++){//для каждого непустого слота в старой таблице table, вызывается функция insertInternal для вставки его данных в новую таблицу
			if(table->data[i].key != NULL){
				insertInternal(&newTable, table->data[i].key,table->data[i].value);
			}
		}
		
		//освобождаем память старую
		free(table->data);
		
		// Обновляем данные в исходной таблице
        table->sizeTable = newTable.sizeTable;
        table->data = newTable.data;
        table->count = newTable.count; // Обновляем счетчик элементов
	}
}
		
		
			




int main (int argc, char *argv[]){
	if (argc !=2){
		printf("usage: %s <input_file>\n", argv[0]);
		return 1;
	}
	
	FILE *file = fopen (argv[1],"r");
	if (file == NULL){
		perror("error open file\n");
		return 1;
	}
	
	// выделение памяти и инициализация хэш таблицы
	HashTable *wordeFrequency = (HashTable*)malloc(sizeof(HashTable));
	wordeFrequency->sizeTable = TABLE_SIZE;
	wordeFrequency->data = (KeyValuePair*)calloc(TABLE_SIZE,sizeof(KeyValuePair));
	wordeFrequency->count = 0;
	
	
	// Приведение слова к нижнему регистру
	char word[MAX_WORD_LENGTH];
	while (fscanf(file,"%s",word )== 1){
		for (size_t i=0; word[i];i++){
			word[i] = tolower(word[i]);
		}
		checkResize(wordeFrequency, porog);//проверка заполненности таблицы
		insert(wordeFrequency, word,1);// добавляем значение в таблицу
	}
	
	fclose(file);
	
	printf("частота слов в файле:\n");
	for (size_t i =0;i<wordeFrequency->sizeTable;i++){
		if(wordeFrequency->data[i].key != NULL){
			printf("%s: %d\n", wordeFrequency->data[i].key, wordeFrequency->data[i].value);
		}
	}
	printf("_________________________________________\n");
	printf("всего уникалиных слов в файле:%zu\n",wordeFrequency->count);
	
	for (size_t i = 0; i < wordeFrequency->sizeTable; i++) {
    free(wordeFrequency->data[i].key);
}
	free(wordeFrequency);
	return 0;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
