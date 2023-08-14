#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define MAX_BYTES 1024 // максимальное количество байт для хранения имени файла


int is_png(const char *inputFilename){
	unsigned char png_magic[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
	FILE *file = fopen(inputFilename,"rb");
	 if (file == NULL){  // успешно ли открыт файл
      puts ("Faild to open the file.\n");
      return 0;}
      
    unsigned char header[8];
	fread(header, sizeof (unsigned char), 8, file);
  fclose (file);
  return memcmp (header,png_magic,8)==0;} // сравниваем 3 байта с магическим числом
  
  
// функция проверяет является ли файл джепегом
int is_jpeg(const char *inputFilename){
  unsigned char jpeg_magic[]={0xFF,0xD8,0xFF};
  FILE *file = fopen(inputFilename,"rb");
    if (file == NULL){  // успешно ли открыт файл
      puts ("Faild to open the file.\n");
      return 0;}
  unsigned char header[3];// массив в котором хранится первые 3 байта файла
  fread(header, sizeof (unsigned char), 3, file);
  fclose (file);
  return memcmp (header,jpeg_magic,3)==0;} // сравниваем 3 байта с магическим числом
//________________________________________
// поиск центрального заголовка зип



 
 
//____________________________________________
//функция преобразовывает массив в число (самая сложная функция в программе)
uint32_t bytes_to_int(uint8_t *bytes, size_t length){
	
    if (length != 2 && length != 4) {
        puts("error function bytes_to_int");
        return 0;
    }
    
    uint32_t result = 0;
    
    for (size_t i = 0; i < length; i++) {
        result |= ((uint32_t)bytes[i] << (i * 8));
    }
    
    return result;
}
//____________________________________________
void list_zip_contents(const char *zip_path){
  FILE *file = fopen(zip_path, "rb");
  if(!file){
    return;
  }
  fseek (file,-4,SEEK_END);
  long position = ftell(file);// текущая позиция каретки
  long off;
  unsigned char end_signature[4]={0x50,0x4B,0x05,0x06};
  unsigned char signature[4];// массив для поиска сигнатур
  
  // цикл для нахождения конца центрального каталога
  while(1){
	
    fread (signature,sizeof (unsigned char),4,file);//прочитываем 4 байта в массив
    if (memcmp (signature, end_signature, 4)==0){
      break;// сравнение сигнатур, если сравнение совпадает,цикл завершается
    }
    position = ftell(file);
    off= (position-sizeof (signature))-1;
    fseek(file, off, SEEK_SET); 
    position = ftell(file);
    
    if ((unsigned long)ftell(file) < sizeof(signature)) {//если мы прошли весь файл и не нашли сигнатуру зип
		puts("Данный файл не является zip архивом.");
		puts("_____________________________________________________________________\n");
		return;
		}
    }
    
  puts("Файл zip архив.");
  puts("_____________________________________________________________________\n");
  unsigned char size_central_catalog[4];// массив в котором хранится размер центрального каталога
  fseek (file,8,SEEK_CUR);//двигаем каретку на байты, которые отвечают за размер центрального каталога
  fread (size_central_catalog, sizeof (unsigned char),4,file);// считываем размер центрального каталога
  uint32_t number = bytes_to_int(size_central_catalog,4);//переменная number принимает преобразованное в инт значение размера центрального каталога   
  fseek (file,-16,SEEK_CUR);// выравнивание каретки
  
  
  position = ftell(file);
  off = (position-sizeof (size_central_catalog))-number; // переменная оффсет это текущая позиция каретки минус размер массива(4) и минус значение размера центрального каталога 
  fseek (file,off,SEEK_SET);// сам прыжок
  /* после прыжка каретка находится перед первым центральным каталогом 
   * файла*/


  uint8_t byteArray[MAX_BYTES]={0};// Массив для хранения считанных байтов имени файлов в архиве
  unsigned char size_name[2];//длина имени файла
  unsigned char zip_magic[]={0x50, 0x4B, 0x01, 0x02};//сигнатура центрального каталога
  int count_file=0;
  
  /*следующий цикл ищет сигнатуру центрального каталога, а также считывает размер
   * длины имени файла в архиве*/
 M:  while(1){
	  fread (signature,sizeof (unsigned char),4,file);//прочитываем 4 байта в массив
    if (memcmp (signature, zip_magic, 4)==0){
		fseek (file,24,SEEK_CUR);
		fread (size_name,sizeof (unsigned char),2,file);// считываем размер длинны имени файла
		number = bytes_to_int(size_name,2);//перевод массива в инт
		fseek (file,16,SEEK_CUR); // перевод каретки на имя файла в архиве
      break;// сравнение сигнатур, если сравнение совпадает,цикл завершается
    } 
    if (memcmp (signature, end_signature, 4)==0){// если сигнатура равна концу центрального каталога,заначит файлы закончились
		fclose(file);
		puts("_____________________________________________________________________\n");
		printf("Файлов в архиве:%d\n",count_file);
		puts("_____________________________________________________________________\n");
		return;
		}	
    position = ftell(file);
    off= (position-sizeof (signature))+1;
    fseek (file,off,SEEK_SET); 
    }
    
    /*считываем и выводим полное имя файлов*/
    fread (byteArray,sizeof (unsigned char),number,file);
    printf("%d. ",count_file+1);
    for (size_t i = 0; i <= number; i++){
    printf ("%c",byteArray[i]);
  }
	memset(byteArray, 0, sizeof(byteArray));
	count_file++;
    puts("\n");
	goto M;
}


int main(int argc, char *argv[]) {
  if (argc !=2) {
    printf ("Usage: %s <input_file>\n",argv[0]);// проверка на количество аргументов
    return 0;
  }
   const char *inputFilename = argv[1];
  puts("Данная программа, определяет, является ли заданный файл изображением,\nв конец которого дописан архив ,либо обычным архивом,\nи выводит список файлов в архиве, если заданный файл таковым является");
  puts("_____________________________________________________________________\n");
   
  if (is_jpeg(inputFilename)){//если файл джепег
    puts("Данный файл является jpeg изображением.\n");
    if(is_png(inputFilename)){// если пнг
	  puts("Данный файл является png изображением.");}
	  }
  else {
    puts ("Файл не является изображением.(png,jpeg)\n");
  }
  list_zip_contents (inputFilename);
   return 0;
  }
   //________________________
 

