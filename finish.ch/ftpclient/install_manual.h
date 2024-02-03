#include <glib.h>
#include <gtk/gtk.h>

#ifndef install_manual_h
#define install_manual_h


int individual_manual(const char *section, wchar_t* folderPath, char *configini);
int copy_to_desktop(wchar_t *source_path, wchar_t *destination_path);
int load_manual(const char *save_path);
int finish_manual(const char *section, wchar_t* folderPath, char *configini);
gpointer  install_all_manual();
int delit_trash(unsigned char *code);
extern GtkBuilder* builder;


#endif// install_manual_h

