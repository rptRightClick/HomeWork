#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <glib.h>
#include <curl/curl.h>
#include <windows.h>
#include <stdio.h>
#include <glib.h>
#include <Shlwapi.h>

#include "install_manual.h"
#include "loadfile.h"

extern CURL* curl;
extern char remove_server[PATH_MAX];
wchar_t save[PATH_MAX];


int individual_manual(const char *section, wchar_t* folderPath, char *configini){
	
	FILE *file = fopen("code","rb");
    if (file == NULL){  // успешно ли открыт файл
		puts ("Faild to open the file.\n");
		return 0;
    }
    
    
	unsigned char code[25] = {0};//размер пользовательского кода
	fread(code, sizeof(unsigned char),25, file);
	printf("code = %s\n",code);
	fclose (file);



    const char *key = "ind_manual";

    GKeyFile *keyfile;
    GError *error = NULL;

    // Создаем и загружаем GKeyFile из INI-файла
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, configini, G_KEY_FILE_NONE, &error)) {
        g_error("Error loading file: %s", error->message);
        g_error_free(error);
        g_key_file_free(keyfile);
        return 1;
    }
char path_ind_manual[PATH_MAX];
    // Получаем значение ключа из секции
    const char *value = g_key_file_get_string(keyfile, section, key, &error);
    if (value != NULL) {
		if(strcmp(value, "false")!=0){
        // Ваш код для обработки значения
        printf("Значение ключа %s\n",value);
        
        snprintf(path_ind_manual,sizeof(path_ind_manual), "%s\\%s",code,value);
        printf("fffffffff====%s\n",path_ind_manual);
        gchar *downloaded_file_path = g_build_filename(g_get_current_dir(), path_ind_manual, NULL);
        
                size_t sourceSize = mbstowcs(NULL, downloaded_file_path, 0);
				wchar_t* downloaded_file_pathW = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
				mbstowcs(downloaded_file_pathW, downloaded_file_path, sourceSize + 1);
				g_free(downloaded_file_path);
				wprintf(L"источник инструкций: %ls\n", downloaded_file_pathW);
				
        
        
		swprintf(save, sizeof(save) / sizeof(save[0]), L"%ls\\%s", folderPath, value);
		 copy_to_desktop(downloaded_file_pathW, save);


        // Не забудьте освободить память, когда она больше не нужна
        g_free((char *)value);
	}
    } else {
        g_error("Error reading key: %s", key);
    }

    // Освобождаем GKeyFile
    g_key_file_free(keyfile);
	return 0;
}

//копирует файл
int copy_to_desktop(wchar_t *source_path, wchar_t *destination_path) {
// Попытка копирования файла
    if (CopyFileW(source_path, destination_path, FALSE)) {
        wprintf(L"Файл успешно скопирован.\n");
    } else {
        wprintf(L"Ошибка при копировании файла. Код ошибки: %lu\n", GetLastError());
    }
    return 0;
}






//скачевает инструкции
int load_manual(const char *save_path){
	 curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize libcurl\n");
        return 1;
    }
        curl_easy_setopt(curl, CURLOPT_URL, remove_server);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "login:password");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
        
        FILE* file = fopen(save_path, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", save_path);
        curl_easy_cleanup(curl);
        return 1;
    }
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to download manual: %s\n", curl_easy_strerror(res));
        }

	fclose(file);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
	return (res == CURLE_OK) ? 0 : 1;
}

//функция скачивает отмеченные инструкции и помещает их на рабочий стол
int finish_manual(const char *section, wchar_t* folderPath, char *configini){

	//начинаем парсить config.ini
	GKeyFile *keyfile;
    GError *error = NULL;

    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, configini, G_KEY_FILE_NONE, &error)) {
        g_error("Error loading file: %s", error->message);
        g_error_free(error);
        return 0;
    }
	if(strcmp(section, "All_Manual") == 0) {
		const char *keys[] = {"firstcnc", "manual2", "manual3"};
		char *values[G_N_ELEMENTS(keys)]; // Массив для хранения значений
		
		for (guint i = 0; i < G_N_ELEMENTS(keys); i++) {
			const char *value = g_key_file_get_string(keyfile, section, keys[i], &error);
			if (value != NULL) {
				values[i] = g_strdup(value);
				g_free((char *)value);
			} else {
				g_error("Error reading key: %s", keys[i]);
				}
		}

    g_key_file_free(keyfile);
    
		for (guint i = 0; i < G_N_ELEMENTS(keys); i++) {
			if (strcmp(values[i], "true") == 0) {
				printf("%s: %s\n", keys[i], values[i]);
				swprintf(save, sizeof(save) / sizeof(save[0]), L"%s.7z", keys[i]);
				snprintf(remove_server, sizeof(remove_server), "ftp://ip/%s/%s.7z",section, keys[i]);
				
				// Вычисляем размер буфера, необходимого для многобайтовой строки
				size_t bufferSize = wcstombs(NULL, save, 0);
				// Выделяем буфер для многобайтовой строки
				char* multiByteString = (char*)malloc(bufferSize + 1);
				// Преобразуем строку wchar_t в многобайтовую строку
				wcstombs(multiByteString, save, bufferSize + 1);
				char downloaded_file_path[PATH_MAX];
					if (load_manual(multiByteString) == 0) {
						// Путь к скачанному файлу
						snprintf( downloaded_file_path,sizeof(downloaded_file_path),"%s\\%ls",g_get_current_dir(), save);
						size_t sourceSize = mbstowcs(NULL, downloaded_file_path, 0);
						wchar_t* downloaded_file_pathW = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
						mbstowcs(downloaded_file_pathW, downloaded_file_path, sourceSize + 1);
						//g_free(downloaded_file_path);
						swprintf(save, sizeof(save) / sizeof(save[0]), L"%ls\\%s.7z", folderPath, keys[i]);
						wprintf(L"назначения инструкций: %ls\n", save);
						// Копируем файл на рабочий стол
					   copy_to_desktop(downloaded_file_pathW, save);
							//удаляем файлы
							if (DeleteFileW(downloaded_file_pathW)) {
								wprintf(L"Файл успешно удален.\n");
							} else {
								wprintf(L"Ошибка при удалении файла. Код ошибки: %lu\n", GetLastError());
							}
					}
					free(multiByteString);
			}
			
			g_free(values[i]);
			values[i] = NULL;
		}
return 0;
	}else if(strcmp(section, "All_Manual_for_mach3") == 0) {
		const char *keys[] = {"manual_mach", "manual_mach2", "manual_mach3"};
		char *values[G_N_ELEMENTS(keys)]; // Массив для хранения значений
		for (guint i = 0; i < G_N_ELEMENTS(keys); i++) {
			const char *value = g_key_file_get_string(keyfile, section, keys[i], &error);
				if (value != NULL) {
					values[i] = g_strdup(value);
					g_free((char *)value);
				} else {
					g_error("Error reading key: %s", keys[i]);
				}
		}
	g_key_file_free(keyfile);
					for (guint i = 0; i < G_N_ELEMENTS(keys); i++) {
						if (strcmp(values[i], "true") == 0) {
							swprintf(save, sizeof(save) / sizeof(save[0]), L"%s.7z", keys[i]);
							snprintf(remove_server, sizeof(remove_server), "ftp://ip/%s/%s.7z",section, keys[i]);
							// Вычисляем размер буфера, необходимого для многобайтовой строки
				size_t bufferSize = wcstombs(NULL, save, 0);
				// Выделяем буфер для многобайтовой строки
				char* multiByteString = (char*)malloc(bufferSize + 1);
				// Преобразуем строку wchar_t в многобайтовую строку
				wcstombs(multiByteString, save, bufferSize + 1);
				char downloaded_file_path[PATH_MAX];
							if (load_manual(multiByteString) == 0) {
								snprintf( downloaded_file_path,sizeof(downloaded_file_path),"%s\\%ls",g_get_current_dir(), save);
								// Путь к скачанному файлу
								size_t sourceSize = mbstowcs(NULL, downloaded_file_path, 0);
								wchar_t* downloaded_file_pathW = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
								mbstowcs(downloaded_file_pathW, downloaded_file_path, sourceSize + 1);
								//g_free(downloaded_file_path);
								wprintf(L"источник инструкций: %ls\n", downloaded_file_pathW);
								
								swprintf(save, sizeof(save) / sizeof(save[0]), L"%ls\\%s.7z", folderPath, keys[i]);
								wprintf(L"Путь2: %ls\n", folderPath);
								wprintf(L"назначения инструкций: %ls\n", save);
								// Копируем файл на рабочий стол
							   copy_to_desktop(downloaded_file_pathW, save);
								//удаляем файлы
									if (DeleteFileW(downloaded_file_pathW)) {
										wprintf(L"Файл успешно удален.\n");
									} else {
										wprintf(L"Ошибка при удалении файла. Код ошибки: %lu\n", GetLastError());
										}
							}
							free(multiByteString);
						}
					
					g_free(values[i]);
					values[i] = NULL;
					}
	return 0;
	} else {
	printf("error parsing section\n"); 
	}
return 0;
}
	
//функция для установки инструкций
gpointer  install_all_manual(){
	GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
	gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Установка инструкций");
	
	GtkProgressBar *progbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "LoadProgres"));
	gtk_progress_bar_set_fraction(progbar, 0.1);
	
	FILE *file = fopen("code","rb");
    if (file == NULL){  // успешно ли открыт файл
		puts ("Faild to open the file.\n");
		return 0;
    }
    
	unsigned char code[25] = {0};//размер пользовательского кода
	fread(code, sizeof(unsigned char),25, file);
	printf("code = %s\n",code);
	fclose (file);
	
	char configini[PATH_MAX];
	snprintf(configini, sizeof(configini), "%s/config.ini", code);
	printf("configini = %s\n",configini);
	
	// создаем папку на рабочем столе , куда будут скопираванны все инструкции
	const gchar *desktop_path = g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP);
    char PathManual[PATH_MAX];
    snprintf(PathManual, sizeof(PathManual), "%s\\%s", desktop_path, "manual");

    // Преобразование в широкую строку
     size_t sourceSize = mbstowcs(NULL, PathManual, 0);
     wchar_t* folderPath = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
     mbstowcs(folderPath, PathManual, sourceSize + 1);
	 wprintf(L"Путь: %ls\n", folderPath);

     if (CreateDirectoryW(folderPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        wprintf(L"Папка успешно создана или уже существует.\n");
	} else {
        wprintf(L"Ошибка при создании папки. Код ошибки: %lu\n", GetLastError());
    }
	
	finish_manual("All_Manual",folderPath, configini);
	gtk_progress_bar_set_fraction(progbar, 0.5);
	finish_manual("All_Manual_for_mach3",folderPath, configini);
	gtk_progress_bar_set_fraction(progbar, 0.8);
	individual_manual("Individual_Manual", folderPath,configini);
	gtk_progress_bar_set_fraction(progbar, 1);
	free(folderPath);
	delit_trash(code);
	GtkWidget *f_window = GTK_WIDGET(gtk_builder_get_object(builder, "finish_window"));
	gtk_widget_show(f_window);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "finish_button")), "clicked", G_CALLBACK(exitButton_clicked_cb), NULL);
	

    // Явное завершение потока
	g_thread_exit(NULL);
}

//функция для удаления временных файлов
int delit_trash(unsigned char *code){
	wchar_t currentDir[MAX_PATH];
	
	//удаление пользовательской папки
	// Получение текущей рабочей папки
	DWORD length_cur = GetCurrentDirectoryW(MAX_PATH, currentDir);
	if (length_cur == 0) {
		// Обработка ошибки
		wprintf(L"Ошибка при получении текущей рабочей папки\n");
		}
	wprintf(L"Текущий путь: %ls\n", currentDir);
	// Создание относительного пути к папке
	wchar_t fullPath[MAX_PATH];
	swprintf(fullPath, MAX_PATH, L"%ls\\%s", currentDir, code);
	wchar_t command[PATH_MAX];
	swprintf(command, sizeof(command) / sizeof(command[0]), L"cmd.exe /c rmdir /s /q \"%ls\"", fullPath);
	
	
	
	
	// Запуск процесса
STARTUPINFOW si;
ZeroMemory(&si, sizeof(STARTUPINFOW));
si.cb = sizeof(STARTUPINFOW);
si.dwFlags |= STARTF_USESHOWWINDOW;
si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    if (CreateProcessW(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Ожидание завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Закрытие дескрипторов процесса и потока
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        wprintf(L"Ошибка при запуске процесса. Код ошибки: %lu\n", GetLastError());
    }
    

	//удаление папки mach3

	// Создание относительного пути к папке
	swprintf(fullPath, MAX_PATH, L"%ls\\Mach3Version3.043", currentDir);
	// Вывод полного пути (для проверки)
	wprintf(L"Полный путь: %ls\n", fullPath);
	swprintf(command, sizeof(command) / sizeof(command[0]), L"cmd.exe /c rmdir /s /q \"%ls\"", fullPath);
	// Выполнение команды с помощью system
	
	if (CreateProcessW(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Ожидание завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Закрытие дескрипторов процесса и потока
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        wprintf(L"Ошибка при запуске процесса. Код ошибки: %lu\n", GetLastError());
    }
	
	
	swprintf(fullPath, MAX_PATH, L"%ls\\%s.7z", currentDir, code);
	if (DeleteFileW(fullPath)) {
			wprintf(L"Файл успешно удален.\n");
	} else {
		wprintf(L"Ошибка при удалении файла. Код ошибки: %lu\n", GetLastError());
		}
		
			swprintf(fullPath, MAX_PATH, L"%ls\\Mach3Version3.043.7z", currentDir);
	if (DeleteFileW(fullPath)) {
			wprintf(L"Файл успешно удален.\n");
	} else {
		wprintf(L"Ошибка при удалении файла. Код ошибки: %lu\n", GetLastError());
		}
		
		
	return 0;
}
