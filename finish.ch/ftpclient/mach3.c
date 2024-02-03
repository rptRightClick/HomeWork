#include <gtk/gtk.h>
#include <glib.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h>
#include <tchar.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <lmcons.h>
#include <Shlwapi.h>
#include <ShlObj.h>

#include "install_manual.h"
#include "loadfile.h"
#include "mach3.h"

/*модуль который скачивает mach3  и устанавливает профиль 
 , а так же запускает установку инструкций*/
 
 
//тестовая функция для выполнения консольных команд в широких символах
void ExecuteCommand(const wchar_t* command) {
	//явная инициализация структуры
    STARTUPINFOW si = { sizeof(STARTUPINFOW), NULL, 0, NULL, 0, 0, 0, 0, 0, 0, 0, SW_HIDE, 0, 0, NULL, 0, 0, 0 };
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Это скроет консольное окно

    if (CreateProcessW(NULL, (LPWSTR)command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Ждем завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Закрываем дескрипторы процесса и потока
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        // В случае ошибки
        perror("Failed to execute command");
    }
}

//функция для установки профиля
gpointer profile_f(gpointer data){
	(void)data;
	
	//считываем из файла пользовательский код
	FILE *file = fopen("code","rb");
    if (file == NULL){  // успешно ли открыт файл
		puts ("Faild to open the file.\n");
		return 0;
    }
	unsigned char code[25] = {0};//размер пользовательского кода
	fread(code, sizeof(unsigned char),25, file);
	printf("code = %s\n",code);
	fclose (file);
	
	GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
	gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Установка профиля");
	
	GtkProgressBar *progbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "LoadProgres"));
	gtk_progress_bar_set_fraction(progbar, 0.1);
	
	
	char configini[PATH_MAX];
	snprintf(configini, sizeof(configini), "%s/config.ini", code);
	//printf("configini = %s\n",configini);
	
    GKeyFile *keyfile;
    GError *error = NULL;

	//начинаем парсить config.ini
	keyfile = g_key_file_new();
	if (!g_key_file_load_from_file(keyfile, configini, G_KEY_FILE_NONE, &error)) {
		g_error("Error loading file: %s", error->message);
		g_error_free(error);
		return 0;
	}
	
	gtk_progress_bar_set_fraction(progbar, 0.3);// изменение прогрессбара
	
    const char *section = "Profile_Mach3";
    const char *keys[] = {"skin", "profile", "macros"};
    char *values[3]; // Массив для хранения значений

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
    
     //// Вывод значений на экран
    //for (int i = 0; i < 3; i++) {
        //printf("%s: %s\n", keys[i], values[i]);
       //// g_free(values[i]);  // Освобождаем память, выделенную для каждого значения
    //}

    gtk_progress_bar_set_fraction(progbar, 0.5);
    //подготовка строк для копирования файлов
    char sourceFile[PATH_MAX];
    char destinationFile[PATH_MAX];
    for (int i = 0; i<2; i++){
		snprintf(sourceFile,sizeof(sourceFile),"%s\\%s",code, values[i]);
		//printf("sourceFile: %s\n", sourceFile);
		snprintf(destinationFile,sizeof(destinationFile),"C:\\Mach3\\%s", values[i]);
		//printf("destinationFile: %s\n", destinationFile);
		
		// Преобразование строк в формат wchar_t* 
		size_t sourceSize = mbstowcs(NULL, sourceFile, 0);
		size_t destSize = mbstowcs(NULL, destinationFile, 0);

		wchar_t* wSourceFile = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
		wchar_t* wDestinationFile = (wchar_t*)malloc((destSize + 1) * sizeof(wchar_t));

		mbstowcs(wSourceFile, sourceFile, sourceSize + 1);
		mbstowcs(wDestinationFile, destinationFile, destSize + 1);

		// Вызов функции patch_mach3 с преобразованными строками
		patch_mach3(wSourceFile, wDestinationFile);

		// Освобождение памяти, выделенной для преобразованных строк
		free(wSourceFile);
		free(wDestinationFile);
		g_free(values[i]);
	}
		
		//копируем папку с макросами
		gtk_progress_bar_set_fraction(progbar, 0.7);
		snprintf(destinationFile, sizeof(destinationFile), "C:\\Mach3\\macros\\%s", values[2]);
		snprintf(sourceFile, sizeof(sourceFile), "%s\\%s", code, values[2]);
		// Преобразование строк в формат wchar_t* 
		size_t sourceSize = mbstowcs(NULL, sourceFile, 0);
		size_t destSize = mbstowcs(NULL, destinationFile, 0);

		wchar_t* wSourceFile = (wchar_t*)malloc((sourceSize + 1) * sizeof(wchar_t));
		wchar_t* wDestinationFile = (wchar_t*)malloc((destSize + 1) * sizeof(wchar_t));

		mbstowcs(wSourceFile, sourceFile, sourceSize + 1);
		mbstowcs(wDestinationFile, destinationFile, destSize + 1);

		// Подготавливаем команду
		wchar_t command[256];
		swprintf(command, sizeof(command) / sizeof(command[0]), L"xcopy \"%ls\" \"%ls\" /e /y /i", wSourceFile, wDestinationFile);

		// Выполняем команду
		ExecuteCommand(command);

		gtk_progress_bar_set_fraction(progbar, 1);

		// Освобождаем память
		g_free(values[2]);
		free(wSourceFile);
		free(wDestinationFile);


		//записываем в файл сохранения дефолтное поведение
		FILE *save = fopen("save.txt","r+");
		if (save == NULL){  // успешно ли открыт файл
			puts ("Faild to open the file.\n");
		}
		// Записываем слово "delete"
		fprintf(save, "defoultstart");
		// Закрываем файл
		fclose(save);
		//удаляем программу из автозапуска
		remove_autorun();
		// Явное завершение потока
		g_thread_exit(NULL);
	}

//функция запускает установку профиля и инструкций
int install_profile_mach3_and_manual(){
		GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
		gtk_widget_hide(mainWindow);
		GtkWidget *loadWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoadWindow"));
		g_signal_connect(loadWindow, "delete-event", G_CALLBACK(destroy_loadWindow_cb), NULL);
		g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancelLoad")), "clicked", G_CALLBACK(destroy_loadWindow_cb), NULL);
		gtk_widget_show(loadWindow);
		GtkWidget *ddd = GTK_WIDGET(gtk_builder_get_object(builder, "select_install_window"));
		gtk_widget_hide(ddd);
		//________________________
		GtkWidget *f_window = GTK_WIDGET(gtk_builder_get_object(builder, "finish_window"));
		gtk_widget_show(f_window);
		gtk_widget_hide(f_window);
		//__________________________
		
		
		
		GThread *profile_idol;
		profile_idol = g_thread_new("profile_thread", profile_f, NULL);
		g_thread_join(profile_idol);
		g_thread_new("install_manual_thread", install_all_manual, NULL);
		return 0;
}

//копирование файлов
int patch_mach3(wchar_t* sourcePath, wchar_t* destinationPath){
    LPVOID errorMsg = NULL; // Инициализация переменной
    // Копирование файла
    if (CopyFileW(sourcePath, destinationPath, FALSE)) {
        wprintf(L"Файл успешно скопирован.\n");
    } else {
        DWORD error = GetLastError();
        wprintf(L"Ошибка при копировании файла. Код ошибки: %lu\n", error);
        
        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            0, // Default language
            (LPWSTR)&errorMsg,
            0, // Don't need size when using FORMAT_MESSAGE_ALLOCATE_BUFFER
            NULL
        );

        // Проверка, что errorMsg не равен NULL
        if (errorMsg != NULL) {
            wprintf(L"Текст ошибки: %s\n", errorMsg);
            
            // Освобождение выделенной памяти для сообщения об ошибке
            LocalFree(errorMsg);
        } else {
            wprintf(L"Не удалось получить текст ошибки.\n");
        }
    }
    return 0;
}


void instal_mach3_window(){
	GtkWidget *hint_win = GTK_WIDGET(gtk_builder_get_object(builder, "manual_window"));

    // Получаем текущий экран
    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(hint_win));

    // Получаем дисплей экрана
    GdkDisplay *display = gdk_screen_get_display(screen);

    // Получаем размер экрана
    GdkMonitor *primary_monitor = gdk_display_get_primary_monitor(display);
    GdkRectangle screen_rect;
    gdk_monitor_get_geometry(primary_monitor, &screen_rect);

    // Рассчитываем положение окна
    int x = screen_rect.x + (screen_rect.width - gtk_widget_get_allocated_width(hint_win)) / 1.4;
    int y = screen_rect.y + (screen_rect.height - gtk_widget_get_allocated_height(hint_win)) / 2.8;
	gtk_window_move(GTK_WINDOW(hint_win), x, y);

	gtk_widget_show(hint_win);
	GtkWidget *install_mach3_w = GTK_WIDGET(gtk_builder_get_object(builder, "install_mach3_window"));
	gtk_widget_show(install_mach3_w);
	GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
	gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Установка Mach3");
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "next_button")), "clicked", G_CALLBACK(next_button_clicked_cb), NULL);
}


void cancel_install_mach3_button_clicked_cb(GtkWidget *widget, gpointer data) {
	(void)widget;
	(void)data;
	remove_autorun();
	FILE *file = fopen("save.txt","r+");
		if (file == NULL){  // успешно ли открыт файл
			puts ("Faild to open the file.\n");
		}
    fprintf(file, "defoultstart");
    // Закрываем файл
    fclose(file);
	gtk_main_quit();
}


void next_button_clicked_cb(GtkWidget *widget, gpointer data) {
	(void)widget;
	(void)data;
	GtkWidget *install_mach3_w = GTK_WIDGET(gtk_builder_get_object(builder, "install_mach3_window"));
	gtk_widget_hide(install_mach3_w);
	
	//запускаем установку mach3


// Запуск процесса с правами администратора
SHELLEXECUTEINFOW shExecInfo = {0};
shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
shExecInfo.lpVerb = L"runas";  // Запуск от имени администратора
shExecInfo.lpFile = L"Mach3Version3.043\\Mach3Version3.043.exe";
shExecInfo.nShow = SW_HIDE;

if (ShellExecuteExW(&shExecInfo)) {
    // Ожидание завершения процесса
    WaitForSingleObject(shExecInfo.hProcess, INFINITE);

    // Закрытие дескриптора процесса
    CloseHandle(shExecInfo.hProcess);
} else {
    wprintf(L"Ошибка при запуске процесса. Код ошибки: %lu\n", GetLastError());
}
	
	

	
	
	
	GtkWidget *hint_win = GTK_WIDGET(gtk_builder_get_object(builder, "manual_window"));
	gtk_widget_hide(hint_win);
	
	const char* path = "C:\\Mach3\\Mach3.exe";
    DWORD fileAttributes = GetFileAttributes(path);
	//проверяем установилась ли mach3
   if (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        // Файл существует, программа установлена
        printf("yes Программа установлена.\n");
        printf("na4inaem patch.\n");
	//патчим мач3
	wchar_t* sourcePath = L"Mach3Version3.043\\Mach1Lic.dat";
	wchar_t* destinationPath = L"C:\\Mach3\\Mach1Lic.dat";
	patch_mach3(sourcePath,destinationPath);
	
	sourcePath = L"Mach3Version3.043\\Mach3.exe";
	destinationPath = L"C:\\Mach3\\Mach3.exe";
	patch_mach3(sourcePath,destinationPath);
	
	printf("patch norm.\n");
	//удаляем папку с установщиком
	wchar_t currentDir[MAX_PATH];
	// Получение текущей рабочей папки
	DWORD length_cur = GetCurrentDirectoryW(MAX_PATH, currentDir);
	if (length_cur == 0) {
		// Обработка ошибки
		wprintf(L"Ошибка при получении текущей рабочей папки\n");
		}
		wprintf(L"Текущий путь: %ls\n", currentDir);

		// Создание относительного пути к папке
		wchar_t fullPath[MAX_PATH];
		swprintf(fullPath, MAX_PATH, L"%ls\\Mach3Version3.043", currentDir);

		// Вывод полного пути (для проверки)
		wprintf(L"Полный путь: %ls\n", fullPath);
		wchar_t command[256];
		swprintf(command, sizeof(command) / sizeof(command[0]), L"rmdir /s /q \"%ls\"", fullPath);
	// Выполнение команды с помощью system
    int result = _wsystem(command);

    // Проверка результата выполнения команды
    if (result == 0) {
        wprintf(L"Директория успешно удалена.\n");
    } else {
        wprintf(L"Ошибка при удалении директории.\n");
    }
	//сохроняем состояние перед ребутом
	FILE *file = fopen("save.txt","r+");
		if (file == NULL){  // успешно ли открыт файл
			perror("Faild to open the file.\n");
		}
    fprintf(file, "afterinstallationmach3");
    // Закрываем файл
    fclose(file);
    add_autorun();
    //перезагрузка
   GtkWidget *rebutW = GTK_WIDGET(gtk_builder_get_object(builder, "rebutWindow"));
   gtk_widget_show(rebutW);
   g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "rebutButton")), "clicked", G_CALLBACK(rebutButton_clicked_cb), NULL);
   g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "laterButton")), "clicked", G_CALLBACK(laterButton_clicked_cb), NULL);
	}else{
			GtkWidget *hint_win = GTK_WIDGET(gtk_builder_get_object(builder, "manual_window"));
			gtk_widget_hide(hint_win);
			GtkWidget *cancel_w = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_install_mach3_window"));
			gtk_widget_show(cancel_w);
			g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancel_install_mach3_button")), "clicked", G_CALLBACK(cancel_install_mach3_button_clicked_cb), NULL);
		}
}

//удаление из автозапуска
int remove_autorun(){
	wchar_t username[UNLEN + 1]; //имя пользователя
	wchar_t path[MAX_PATH]; // путь к ярлыку
	 DWORD size = sizeof(username) / sizeof(username[0]);
	if (GetUserNameW(username, &size)) {
        // Теперь переменная 'username' содержит имя текущего пользователя
        wprintf(L"Имя пользователя: %ls\n", username);
        // Теперь вы можете использовать 'username' для создания пути
        swprintf(path, sizeof(path) / sizeof(path[0]), L"C:\\Users\\%ls\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\ModelistCentr.lnk", username);
        wprintf(L"Путь: %ls\n", path);
	}
	// Удаление файла
    if (DeleteFileW(path)) {
        wprintf(L"Файл успешно удален.\n");
    } else {
        wprintf(L"Ошибка при удалении файла. Код ошибки: %lu\n", GetLastError());
    }
    return 0;
}

//добавление в автозапуск
int add_autorun(){
     // Инициализация COM
    CoInitialize(NULL);
	wchar_t exePath[MAX_PATH];//путь к exe файлу
	wchar_t path[MAX_PATH]; // путь к ярлыку
	wchar_t username[UNLEN + 1]; //имя пользователя
	wchar_t currentDir[MAX_PATH];// путь к рабочей папке
	 // Получение текущей рабочей папки
	DWORD length_cur = GetCurrentDirectoryW(MAX_PATH, currentDir);
	if (length_cur == 0) {
    // Обработка ошибки
		wprintf(L"Ошибка при получении текущей рабочей папки\n");
		return 1;
	}
    // Получение полного пути к исполняемому файлу текущего процесса
    DWORD length = GetModuleFileNameW(NULL, exePath, MAX_PATH);
    if (length == 0) {
        // Обработка ошибки
        wprintf(L"Ошибка при получении пути к исполняемому файлу\n");
        return 1;
    }
    // Создание объекта IShellLink
    IShellLinkW* pShellLink; // Используем IShellLinkW для работы с широкими символами
    CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, (LPVOID*)&pShellLink);
    // Установка свойств ярлыка
    pShellLink->lpVtbl->SetPath(pShellLink, exePath);
    pShellLink->lpVtbl->SetDescription(pShellLink, L"Ярлык для автозапуска");
    // Устанавливаем рабочую папку
    pShellLink->lpVtbl->SetWorkingDirectory(pShellLink, currentDir);


    // Получение интерфейса IPersistFile
    IPersistFile* pPersistFile;
    pShellLink->lpVtbl->QueryInterface(pShellLink, &IID_IPersistFile, (LPVOID*)&pPersistFile);
    
    DWORD size = sizeof(username) / sizeof(username[0]);
    if (GetUserNameW(username, &size)) {
        // Теперь переменная 'username' содержит имя текущего пользователя
        wprintf(L"Имя пользователя: %ls\n", username);

        swprintf(path, sizeof(path) / sizeof(path[0]), L"C:\\Users\\%ls\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\ModelistCentr.lnk", username);
        wprintf(L"Путь: %ls\n", path);

    // Сохранение ярлыка на диск
    pPersistFile->lpVtbl->Save(pPersistFile, path, TRUE);

    
    
}

    // Освобождение ресурсов
    pPersistFile->lpVtbl->Release(pPersistFile);
    pShellLink->lpVtbl->Release(pShellLink);
    // Завершение COM
    CoUninitialize();
    return 0;
}
	

/*функция предназначена для включения (включения) определенной привилегии для текущего процесса. 
 *принимает параметр lpszPrivilege,который представляет собой строку (LPCTSTR), содержащую имя привилегии, которую мы хотим включить.
 */
BOOL EnablePrivilege(LPCTSTR lpszPrivilege) {
    HANDLE hToken;//Создается переменная hToken типа HANDLE, которая будет использоваться для хранения дескриптора токена (token) текущего процесса.
    TOKEN_PRIVILEGES tkp;
    //открывает токен текущего процесса для возможности регулирования привилегий (TOKEN_ADJUST_PRIVILEGES)
    //(TOKEN_QUERY) запрос информации о привилегиях 
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        _tprintf(_T("OpenProcessToken failed: %lu\n"), GetLastError());
        return FALSE;
    }

    //LookupPrivilegeValue  получает LUID (Locally Unique Identifier) для указанного имени привилегии (lpszPrivilege)
    if (!LookupPrivilegeValue(NULL, lpszPrivilege, &tkp.Privileges[0].Luid)) {
        _tprintf(_T("LookupPrivilegeValue failed: %lu\n"), GetLastError());
        CloseHandle(hToken);
        return FALSE;
    }

    tkp.PrivilegeCount = 1; // Устанавливаем количество привилегий
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // Включаем привилегию

    // Функция AdjustTokenPrivileges применяет новые привилегии к токену процесса.
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        _tprintf(_T("AdjustTokenPrivileges failed: %lu\n"), GetLastError());
        CloseHandle(hToken);
        return FALSE;
    }

    CloseHandle(hToken);
    return TRUE;
}

//функция для перезагрузки пк
void rebutButton_clicked_cb(){
	GtkWidget *rebutW = GTK_WIDGET(gtk_builder_get_object(builder, "rebutWindow"));
	gtk_widget_hide(rebutW);
	// Вызов функции для перезагрузки компьютера
    if (EnablePrivilege(SE_SHUTDOWN_NAME)) {
		if (!ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_INSTALLATION)) {
			_tprintf(_T("Reboot error: %lu\n"), GetLastError());
		}
	} else {
    _tprintf(_T("Failed to enable SE_SHUTDOWN_NAME privilege\n"));
	}
}

//каллбек функция для кнопки перезагрузить позже
void laterButton_clicked_cb(){
	gtk_main_quit();
}


void full_install_mach3_clicked_cb(){
	//удаление программы
	GtkWidget *ddd = GTK_WIDGET(gtk_builder_get_object(builder, "select_install_window"));
	gtk_widget_hide(ddd);
	
	
	
	
	// Запуск процесса с правами администратора
SHELLEXECUTEINFOW shExecInfo = {0};
shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
shExecInfo.lpVerb = L"runas";  // Запуск от имени администратора
shExecInfo.lpFile = L"C:\\Windows\\Mach3\\uninstall.exe";
shExecInfo.lpParameters = L"/U:C:\\Mach3\\Uninstall\\uninstall.xml";
shExecInfo.nShow = SW_HIDE;

if (ShellExecuteExW(&shExecInfo)) {
    // Ожидание завершения процесса
    WaitForSingleObject(shExecInfo.hProcess, INFINITE);

    // Закрытие дескриптора процесса
    CloseHandle(shExecInfo.hProcess);
} else {
    wprintf(L"Ошибка при запуске процесса. Код ошибки: %lu\n", GetLastError());
}
	


	
	//повторная проверка установленна ли mach3 , если установленна , то удаление было отмена
	const char* path = "C:\\Mach3\\Uninstall\\uninstall.xml";
    DWORD fileAttributes = GetFileAttributes(path);
    
    if (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			GtkWidget *cancel_w = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_install_mach3_window"));
			gtk_widget_show(cancel_w);
			g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancel_install_mach3_button")), "clicked", G_CALLBACK(cancel_install_mach3_button_clicked_cb), NULL);
			} else {
				FILE *file = fopen("save.txt","r+");
					if (file == NULL){  // успешно ли открыт файл
						puts ("Faild to open the file.\n");
						}
				//записываем в файл сохранения поведение для установки mach3
				fprintf(file, "installmach3");
				// Закрываем файл
				fclose(file);
				//добовляем в автозапуск
				add_autorun();
				g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "laterButton")), "clicked", G_CALLBACK(laterButton_clicked_cb), NULL);
				GtkWidget *rebutW = GTK_WIDGET(gtk_builder_get_object(builder, "rebutWindow"));
				g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "rebutButton")), "clicked", G_CALLBACK(rebutButton_clicked_cb), NULL);
				gtk_widget_show(rebutW);
			}
}

//функция установки только профиля
void instal_only_config_mach3_clicked_cb(){
	GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
	gtk_widget_hide(mainWindow);
	GtkWidget *loadWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoadWindow"));
	g_signal_connect(loadWindow, "delete-event", G_CALLBACK(destroy_loadWindow_cb), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancelLoad")), "clicked", G_CALLBACK(destroy_loadWindow_cb), NULL);
	gtk_widget_show(loadWindow);
	GtkWidget *ddd = GTK_WIDGET(gtk_builder_get_object(builder, "select_install_window"));
	gtk_widget_hide(ddd);
	
	GThread *thread_profile;
	thread_profile = g_thread_new("profile_thread", profile_f, NULL);
	g_thread_join(thread_profile);
	
	gtk_widget_hide(loadWindow);
	GtkWidget *finish = GTK_WIDGET(gtk_builder_get_object(builder, "compile_window"));
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "exit_button")), "clicked", G_CALLBACK(exitButton_clicked_cb), NULL);
	gtk_widget_show(finish);
}

//функция запускает установку профиля и инструкций
void instal_manual_and_profile_clicked_cb(){
	install_profile_mach3_and_manual();
}
	
//окно выбора переустановить mach3 или установить только когфиг или конфиг и инструкции
void select_f(){
	GtkWidget *ddd = GTK_WIDGET(gtk_builder_get_object(builder, "select_install_window"));
	gtk_widget_show(ddd);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "full_install_mach3")), "clicked", G_CALLBACK(full_install_mach3_clicked_cb), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "instal_manual_and_profile")), "clicked", G_CALLBACK(instal_manual_and_profile_clicked_cb), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "instal_only_config_mach3")), "clicked", G_CALLBACK(instal_only_config_mach3_clicked_cb), NULL);
	
}

void check_install_mach3(){
	
	GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
	gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Проверка системы");
	const char* path = "C:\\Mach3\\Uninstall\\uninstall.xml";
	DWORD fileAttributes = GetFileAttributes(path);

	//проверка установленна ли уже mach3
    if (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		// Файл существует, программа установлена
		printf("yes Программа установлена.\n");
		select_f();
		} else {
		// Файл не существует, программа не установлена
			printf(" no Программа не установлена.\n");
			instal_mach3_window();
			}
}
	
//распаковка архива с mach3
void unzip_mach3(){
		const char *sevenZipPath = "7zr.exe";
		const char *archivePath = "Mach3Version3.043.7z";

		// Формируем команду для распаковки
		char command[256];
		sprintf(command, "%s x -aoa %s", sevenZipPath, archivePath);

		// Выполняем команду через CreateProcess
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
			// Дожидаемся завершения процесса
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Закрываем дескрипторы процесса и потока
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			printf("athiv s mach3 raspakovan. %s\n", archivePath);
		} else {
			printf("error arhiv s mach3 .\n");
		}
}



//скачивание mach3
gpointer download_mach3_thread() {
	CURL* curl;
	curl = curl_easy_init();
	 curl_global_init(CURL_GLOBAL_DEFAULT);
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "ftp://ip/SOFT/Mach3Version3.043.7z");
		curl_easy_setopt(curl, CURLOPT_USERPWD, "login:password");
		// Включение использования SSL/TLS и отключение проверки сертификата
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		// Устанавливаем функцию обратного вызова для прогресса
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
		// Включаем обработку прогресса
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		// Устанавливаем функцию обратного вызова для получения информации о прогрессе
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo_callback);

		// Включаем вывод заголовков, чтобы получать информацию о размере файла
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

		// Устанавливаем функцию обратного вызова для сохранения файла
		FILE* file = fopen("Mach3Version3.043.7z", "wb");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		// Выполняем запрос
		CURLcode res = curl_easy_perform(curl);


		// Проверяем результат выполнения
		if (res != CURLE_OK) {
			fprintf(stderr, "error curl: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
		}

		// Закрываем файл
		fclose(file);

		// Освобождаем ресурсы
		curl_easy_cleanup(curl);
		}
		unzip_mach3();
		check_install_mach3();
		// Явное завершение потока
		g_thread_exit(NULL);
	}
