#ifndef parsing_h
#define parsing_h

#include <glib.h>

extern GtkBuilder* builder;
extern GThread *thread2;
void parseIniFile(char *filename, GArray *entries);
void freeIniEntries(GArray *entries);
gpointer parsing_thread(gpointer data);



#endif// parsing ini file
