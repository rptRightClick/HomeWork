#include <glib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#ifndef mach3_h
#define mach3_h


extern GtkBuilder* builder;
void ExecuteCommand(const wchar_t* command);
gpointer profile_f(gpointer data);
int install_profile_mach3_and_manual();
int patch_mach3(wchar_t* sourcePath, wchar_t* destinationPath);
void instal_mach3_window();
void cancel_install_mach3_button_clicked_cb(GtkWidget *widget, gpointer data);
void next_button_clicked_cb(GtkWidget *widget, gpointer data);
int remove_autorun();
int add_autorun();
BOOL EnablePrivilege(LPCTSTR lpszPrivilege);
void rebutButton_clicked_cb();
void laterButton_clicked_cb();
void full_install_mach3_clicked_cb();
void instal_only_config_mach3_clicked_cb();
void instal_manual_and_profile_clicked_cb();
void select_f();
void check_install_mach3();
void unzip_mach3();
gpointer download_mach3_thread();




#endif// download_and_install_mach3
