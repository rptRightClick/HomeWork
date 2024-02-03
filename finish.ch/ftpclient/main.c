#include <gtk/gtk.h>
#include <stdio.h> // Стандартная библиотека ввода/вывода
#include <stdlib.h> // Стандартная библиотека общих утилит
#include <string.h> // Стандартная библиотека для операций с символьными строками
#include <glib.h>
#include <curl/curl.h>
#include <unistd.h> // Стандартная библиотека для обработки системных вызовов

#include "loadfile.h"
#include "mach3.h"
#include "ferstconnect.h"
#include "install_manual.h"


GtkBuilder* builder;
// строки для контроля поведением программы (файл save.txt)
const char install_mach3[] = {"installmach3"};
const char defoult_start[] = {"defoultstart"};
const char after_installation_mach3[] = {"afterinstallationmach3"};


void exit_button_clicked_cb(GtkWidget *widget, gpointer data){
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
	gtk_main_quit();
}

void finish(GtkWidget *widget, gpointer data){
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
		GtkWidget *finishW = GTK_WIDGET(gtk_builder_get_object(builder, "compile_window"));
		gtk_widget_show(finishW);
		g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "exit_button")), "clicked", G_CALLBACK(exit_button_clicked_cb), NULL);
}


//певрое что делает программа проверяет файл save и дальнейшие поведение зависит от него
int read_save(){
	FILE *file = fopen("save.txt","rb");
    if (file == NULL){  // успешно ли открыт файл
		perror("Error (save.txt open)");
    }
	unsigned char header[22];
	fread(header, sizeof (unsigned char),22, file);
	fclose (file);
	if (memcmp (header,defoult_start,12)==0){
		return 5;
	}else if (memcmp (header,install_mach3,12)==0){
		instal_mach3_window();
	}else if (memcmp (header,after_installation_mach3,22)==0){
		install_profile_mach3_and_manual();//mach3.c
	}else{
		perror("Error (incorrect program behavior)");
		GtkWidget *error_save_w = GTK_WIDGET(gtk_builder_get_object(builder, "error_save_window"));
		gtk_widget_show(error_save_w);
		g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "exit_button_save")), "clicked", G_CALLBACK(exit_button_clicked_cb), NULL);
	}
	 return 0;
}

// Callback-функция для обработки события "destroy"
void destroy_main_cb(GtkWidget *widget, gpointer data) {
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
    // Сохранение состояния в файл перед завершением
    gtk_main_quit();
}

//____________________________________________________
int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	//Загрузка интерфейса из файла ui.glade
	builder = gtk_builder_new();
	GError *error = NULL;

    if (gtk_builder_add_from_file(builder, "ftpclientui.glade", &error) == 0) {
        g_printerr("error glade .glade: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    
    GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "Mainwindow"));
    //в зависимости от результата функции read_save отоброжается нужное окно
	int result = read_save();
	if (result == 5 ){
		gtk_widget_show_all(mainWindow);
	}
	
	
	g_thread_new("ferstconnect_thread", ferstconnect_thread, NULL);//testovoe soedinenie s serverom v otdelnom potoke
	reconnect_to_server();// knopka reconnect
	loadfile();//zagruzka filelov s servera



	g_signal_connect(mainWindow, "destroy", G_CALLBACK(destroy_main_cb), NULL);

	
	gtk_main();
    g_object_unref(G_OBJECT(builder));

    return 0;
}
    
