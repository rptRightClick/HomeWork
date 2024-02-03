#include <glib.h>

#ifndef ferstconnect_h
#define ferstconnect_h

extern GtkBuilder* builder;
void reconnect_clicked_cb();
void reconnect_to_server();
gpointer ferstconnect_thread(gpointer data);

#endif// pervii connect to server
