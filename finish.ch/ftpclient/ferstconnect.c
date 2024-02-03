#include <gtk/gtk.h>
#include <glib.h>
#include <curl/curl.h>
#include "ferstconnect.h"




void reconnect_clicked_cb(GtkWidget *widget, gpointer data){
	(void)widget; // Подавление предупреждения о неиспользуемом параметре
	(void)data;   // Подавление предупреждения о неиспользуемом параметре
	g_thread_new("ferstconnect_thread", ferstconnect_thread, NULL);
	
}
	
void reconnect_to_server(){
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "reconnect")), "clicked", G_CALLBACK(reconnect_clicked_cb), NULL);
}

gpointer ferstconnect_thread(gpointer data) {
	(void)data; 
	GtkWidget *setup_button = GTK_WIDGET(gtk_builder_get_object(builder, "setup"));
	gtk_widget_set_sensitive(setup_button, FALSE);
	GtkWidget *reconnect_button = GTK_WIDGET(gtk_builder_get_object(builder, "reconnect"));
    gtk_widget_set_sensitive(reconnect_button, FALSE);
	GtkWidget *info_stat = GTK_WIDGET(gtk_builder_get_object(builder,"info"));
	gtk_statusbar_push(GTK_STATUSBAR(info_stat), 1, "Попытка соединения с сервером");
	
	CURL *curl = curl_easy_init();
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "ftp://ip");
        curl_easy_setopt(curl, CURLOPT_USERPWD, "login:password");
        // Включение использования SSL/TLS и отключение проверки сертификата
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
			gtk_statusbar_push(GTK_STATUSBAR(info_stat), 1, "Нет связи с сервером");
            fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            gtk_widget_set_sensitive(reconnect_button, TRUE);

        }else{
        gtk_statusbar_push(GTK_STATUSBAR(info_stat), 1, "Связь установленна");
        curl_easy_cleanup(curl);
        gtk_widget_set_sensitive(reconnect_button, FALSE);
        gtk_widget_set_sensitive(setup_button, TRUE);
	}
    }
    g_thread_exit(NULL);
}

