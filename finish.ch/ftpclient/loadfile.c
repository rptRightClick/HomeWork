#include <gtk/gtk.h>
#include <glib.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <unistd.h>

#include "loadfile.h"
#include "parsing.h"

/* данный модуль отвечает за скачивания с сервера нужного профиля,
а также проверяет если данный профиль на сервере, после успешного скачивания распаковывает его в папку
с исполняемым файлом*/

char localzip[PATH_MAX]; // массив в котором хранится названия архива с профилем 
char remove_server[PATH_MAX]; // url сервера 
CURL* curl;
GThread *thread1, *thread2;// первый поток отвечает за скачивания файла , второй за распоковку


// кнопка отмены установки
void exitButton_clicked_cb(GtkWidget *widget, gpointer data){
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
	FILE *save = fopen("save.txt","r+");
		if (save == NULL){  // успешно ли открыт файл
			puts ("Faild to open the file.\n");
		}
    // Записываем слово "delete"
    fprintf(save, "defoultstart");
    // Закрываем файл
    fclose(save);
    gtk_main_quit();
}

// кнопка отмены завершение установки
void cancelExitButton_clicked_cb(GtkWidget *widget, gpointer data){
		(void)widget; // Подавление предупреждения о неиспользуемом параметре
		(void)data;   // Подавление предупреждения о неиспользуемом параметре
		GtkWidget *ExitWindow = GTK_WIDGET(gtk_builder_get_object(builder, "exitWindow"));
		GtkWidget *loadWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoadWindow"));
		gtk_widget_set_sensitive(loadWindow, TRUE);
		gtk_widget_hide(ExitWindow);
}

// функция выводит окно подтверждения перед закрытием
void destroy_loadWindow_cb(GtkWidget *widget, gpointer data) {
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
	
	GtkWidget *ExitWindow = GTK_WIDGET(gtk_builder_get_object(builder, "exitWindow"));
	GtkWidget *loadWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoadWindow"));
	gtk_widget_set_sensitive(loadWindow, FALSE);
	gtk_widget_show(ExitWindow);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancelExitButton")), "clicked", G_CALLBACK(cancelExitButton_clicked_cb), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "exitButton")), "clicked", G_CALLBACK(exitButton_clicked_cb), NULL);
}

// функция скрывает предупреждение и возвращает управление пользователю
void NoSearch_button_clicked_cb(GtkWidget *widget, gpointer data){
	(void)widget;
	(void)data;
	GtkWidget *ErrorWindow = GTK_WIDGET(gtk_builder_get_object(builder, "NoSearch"));
	GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
	gtk_widget_set_sensitive(mainWindow, TRUE);
	gtk_widget_hide(ErrorWindow);
}

//функция отбирает управление у пользователя и выводит ошибку
void showError(){
	GtkWidget *ErrorWindow = GTK_WIDGET(gtk_builder_get_object(builder, "NoSearch"));
	GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
	gtk_widget_set_sensitive(mainWindow, FALSE);
	gtk_widget_show(ErrorWindow);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "NoSearch_button")), "clicked", G_CALLBACK(NoSearch_button_clicked_cb), NULL);
}



// Функция обратного вызова для сохранения скачанного файла
size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	return fwrite(ptr, size, nmemb, stream);
}



// Функция обратного вызова для отслеживания прогресса скачивания
int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	(void)clientp;
	(void)ultotal;
	(void)ulnow;
	// Вычисляем процент завершенности скачивания
	GtkProgressBar *progbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "LoadProgres"));
	double show = (dltotal > 0) ? (double)dlnow / (double)dltotal : 0.0;
	//printf("Загружено: %f\n", show);
	// Обновляем прогресс бар
	gtk_progress_bar_set_fraction(progbar, show);
	return 0;
}

// Функция обратного вызова для получения информации о прогрессе
int xferinfo_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	// Используем ту же функцию обратного вызова для прогресса
	return progress_callback(clientp, dltotal, dlnow, ultotal, ulnow);
}


// функция для распаковки скачанного архива
gpointer unzip_thread(gpointer data) {
		GThread *thread1 = (GThread *)data;

		// Ждем завершения первого потока
		g_thread_join(thread1);
		
		const char *sevenZipPath = "7zr.exe";

		// Формируем команду для распаковки
		char command[256];
		sprintf(command, "%s x -aoa %s", sevenZipPath, localzip);

		// Выполняем команду через CreateProcess
		// STARTUPINFO -- эта структура, используемая для настройки нового процесса
		//Она содержит информацию, такую как размер структуры, стандартные дескрипторы ввода/вывода и другие параметры.
		STARTUPINFO si;
		
		//PROCESS_INFORMATION - это структура, которая будет заполнена функцией CreateProcess и содержит информацию о созданном процессе,
		// такую как идентификатор процесса и дескрипторы процесса и потока.
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));// Заполняем структуру нулями для избежания мусорных значений
		si.cb = sizeof(si);// Устанавливаем размер структуры STARTUPINFO
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
			// Дожидаемся завершения процесса
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Закрываем дескрипторы процесса и потока
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			printf("athiv s profilem raspakovan. %s\n", localzip);
		} else {
			printf("error arhiv s profilem .\n");
		}
	// Явное завершение потока
	g_thread_exit(NULL);
}




// функция загружает архив с сервера
gpointer load_thread(gpointer data){
	(void)data;
	GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
	gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Скачивание конфигурационных файлов");
	 curl = curl_easy_init();
	 curl_global_init(CURL_GLOBAL_DEFAULT);
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, remove_server);
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
		FILE* file = fopen(localzip, "wb");
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
		// Явное завершение потока
		g_thread_exit(NULL);
}

//функция проверяет существует ли файл на сервере
int checkfile(){
	CURL* curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Failed to initialize curl.\n");
		return -1;
	}
	// Устанавливаем URL для HEAD-запроса
	curl_easy_setopt(curl, CURLOPT_URL, remove_server);
	curl_easy_setopt(curl, CURLOPT_USERPWD, "login:password");
	// Включение использования SSL/TLS и отключение проверки сертификата
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	// Указываем, что нужен HEAD-запрос
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

	// Выполняем запрос
	CURLcode res = curl_easy_perform(curl);
	// Проверяем результат выполнения
	if (res == CURLE_OK) {
		long response_code;
		// Получаем HTTP-код ответа
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		// Если файл существует, сервер вернет код 350
		if (response_code == 350) {
			printf("Файл существует на сервере.\n");
			curl_easy_cleanup(curl);
			return 1;  // Вернуть код успеха
		} else {
			printf("Файл не существует. HTTP-код ответа: %ld\n", response_code);
		}
		} else {
		fprintf(stderr, "Ошибка при выполнении HEAD-запроса: %s\n", curl_easy_strerror(res));
		}

		// Освобождаем ресурсы
		curl_easy_cleanup(curl);
		return 0;
}
    


// сигнал кнопки "установить", функция запускает два потока 1 для скачивания 2 для распаковки 
void setup_clicked_cb(GtkWidget *widget, gpointer data) {
	(void)widget;
	(void)data;
	GtkWidget *input = GTK_WIDGET(gtk_builder_get_object(builder, "input"));
	const gchar *name_config = gtk_entry_get_text(GTK_ENTRY(input));
	if (name_config != NULL && strlen(name_config) > 0) {
		snprintf(localzip, sizeof(localzip), "%s.7z", name_config);
		snprintf(remove_server, sizeof(remove_server), "ftp://ip/%s.7z", name_config);
		int res = checkfile();
		if (res == 1){
				//сохроняем введеный номер в файл
				FILE *file = fopen("code","rb+");
				if (file == NULL){  // успешно ли открыт файл
					perror("Faild to open the file.");
					}
				fprintf(file, name_config);
				// Закрываем файл
				fclose(file);
			GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
			gtk_widget_hide(mainWindow);
			GtkWidget *loadWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoadWindow"));
			gtk_widget_show(loadWindow);
			
			//не знаю почему работает(но без этого ошибка дискритпора окна номер 1400)
			//____________________________________________________________________
			GtkWidget *install_mach3_w = GTK_WIDGET(gtk_builder_get_object(builder, "install_mach3_window"));
			gtk_widget_show(install_mach3_w);
			gtk_widget_hide(install_mach3_w);
			GtkWidget *hint_win = GTK_WIDGET(gtk_builder_get_object(builder, "manual_window"));
			gtk_window_move(GTK_WINDOW(hint_win), 1920 - 440, 290);
			gtk_widget_show(hint_win);
			gtk_widget_hide(hint_win);
			GtkWidget *ddd = GTK_WIDGET(gtk_builder_get_object(builder, "select_install_window"));
			gtk_widget_show(ddd);
			gtk_widget_hide(ddd);
			GtkWidget *rebutW = GTK_WIDGET(gtk_builder_get_object(builder, "rebutWindow"));
			gtk_widget_show(rebutW);
			gtk_widget_hide(rebutW);
			GtkWidget *finishW = GTK_WIDGET(gtk_builder_get_object(builder, "compile_window"));
			gtk_widget_show(finishW);
			gtk_widget_hide(finishW);
			GtkWidget *f_window = GTK_WIDGET(gtk_builder_get_object(builder, "finish_window"));
			gtk_widget_show(f_window);
			gtk_widget_hide(f_window);
			//____________________________________________________________________
			
			//выводит подтверждение об отмене скачивания 
			g_signal_connect(loadWindow, "delete-event", G_CALLBACK(destroy_loadWindow_cb), NULL);
			g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "cancelLoad")), "clicked", G_CALLBACK(destroy_loadWindow_cb), NULL);
			
			//первый поток для скачивания файла
			thread1 = g_thread_new("load_thread", load_thread, NULL);
			// Создаем второй поток, передавая ему идентификатор первого потока
			thread2 = g_thread_new("unzip_thread", unzip_thread, thread1);
			// запускаем процесс парсинга
			g_thread_new("parsing_thread", parsing_thread, thread2);//функция в новом потоке для парсинга ini файла parsing.c
		}else{
			showError();
			}
	}else{
		showError();
		}
}



void loadfile() {
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "setup")), "clicked", G_CALLBACK(setup_clicked_cb), NULL);
}

