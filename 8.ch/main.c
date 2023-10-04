#include <gtk/gtk.h>
#include <dirent.h>


GtkWidget *window;
GtkListStore *liststore;

void load_files(GtkWidget *widget, gpointer data) {
    gtk_list_store_clear(liststore);

    const char *current_directory = ".";
    DIR *dir;
    struct dirent *entry;

    dir = opendir(current_directory);
    if (dir == NULL) {
        g_print("не удалось открыть\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        GtkTreeIter iter;
        gboolean is_dir = entry->d_type == DT_DIR;

        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter, 0, entry->d_name, 1, is_dir, -1);
    }

    closedir(dir);
}

void on_activate(GApplication *app, gpointer user_data) {
    window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "список файлов");    //главное окно
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6); // вертикальный контейнер
    gtk_container_add(GTK_CONTAINER(window), vbox);

    liststore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);
    load_files(NULL, NULL);

    GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(liststore));

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Имя файла", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);



//виджет для скрола
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), treeview);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *refresh_button = gtk_button_new_with_label("Обновить");
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(load_files), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), refresh_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.file_viewer", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
