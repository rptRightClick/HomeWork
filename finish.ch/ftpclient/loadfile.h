#include <stdio.h>
#include <glib.h>


#ifndef loadfile_h
#define loadfile_h


extern GtkBuilder* builder;

void exitButton_clicked_cb(GtkWidget *widget, gpointer data);
void cancelExitButton_clicked_cb(GtkWidget *widget, gpointer data);
void destroy_loadWindow_cb(GtkWidget *widget, gpointer data);
void NoSearch_button_clicked_cb(GtkWidget *widget, gpointer data);
void showError();
size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream);
int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
int xferinfo_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
gpointer unzip_thread(gpointer data);
gpointer load_thread(gpointer data);
int checkfile();
void setup_clicked_cb(GtkWidget *widget, gpointer data);
void loadfile();

#endif// loadfile_h
