#include <glib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

#include "parsing.h"
#include "mach3.h"

char configini[PATH_MAX];//путь к конфигу

//модуль парсить файл config.ini и меняет поведение пограммы в зависимости от парсинга

//структура ini файла
typedef struct {
    gchar *section;
    gchar *key;
    gchar *value;
} IniEntry;


//функция отвечает за парсинг INI-файла и заполнение массива entries 
void parseIniFile(char *filename, GArray *entries) {
    GKeyFile *key_file;
    GKeyFileFlags flags;
    GError *error = NULL;

    key_file = g_key_file_new();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    if (!g_key_file_load_from_file(key_file, filename, flags, &error)) {
        g_error("Ошибка загрузки файла: %s", error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return;
    }

    gsize num_sections;
    gchar **sections = g_key_file_get_groups(key_file, &num_sections);

    for (gsize i = 0; i < num_sections; i++) {
        const gchar *section = sections[i];
        gsize num_keys;
        gchar **keys = g_key_file_get_keys(key_file, section, &num_keys, NULL);

        for (gsize j = 0; j < num_keys; j++) {
            const gchar *key = keys[j];
            gchar *value = g_key_file_get_string(key_file, section, key, NULL);

            IniEntry entry = {g_strdup(section), g_strdup(key), g_strdup(value)};
            g_array_append_val(entries, entry);

            g_free(value);
        }

        g_strfreev(keys);
    }

    g_strfreev(sections);
    g_key_file_free(key_file);
}


//Эта функция освобождает выделенную память для элементов массива entries
void freeIniEntries(GArray *entries) {
    for (guint i = 0; i < entries->len; i++) {
        IniEntry *entry = &g_array_index(entries, IniEntry, i);
        g_free(entry->section);
        g_free(entry->key);
        g_free(entry->value);
    }

    g_array_free(entries, TRUE);
}

gpointer parsing_thread(gpointer data) {
	GThread *thread2 = (GThread *)data;
	// Ждем завершения распаковки
	g_thread_join(thread2);
	GtkWidget *input = GTK_WIDGET(gtk_builder_get_object(builder, "input"));
	const gchar *name_config = gtk_entry_get_text(GTK_ENTRY(input));
	snprintf(configini, sizeof(configini), "%s/config.ini", name_config);
    GArray *entries = g_array_new(FALSE, FALSE, sizeof(IniEntry));
    parseIniFile(configini, entries);

   
    for (guint i = 0; i < entries->len; i++) {
        IniEntry *entry = &g_array_index(entries, IniEntry, i);

        if (g_strcmp0(entry->section, "Controller") == 0 && g_strcmp0(entry->key, "controller_name") == 0) {
    if (g_strcmp0(entry->value, "mach3") == 0) {
		// код для соответствующего поведения для Mach3
        GtkWidget *load_stat = GTK_WIDGET(gtk_builder_get_object(builder,"loadStatusBar"));
		gtk_statusbar_push(GTK_STATUSBAR(load_stat), 1, "Скачивание необходимого ПО");
        GThread *thread = g_thread_new("download_mach3_thread", download_mach3_thread, NULL); //mach3.c
        g_thread_join(thread);
        
    } else if (g_strcmp0(entry->value, "smc4") == 0) {
        g_print("Поведение для SMC4\n");
        // од для соответствующего поведения для SMC4
    } else {
        g_print("Другое поведение\n");
        // код для других значений entry->value
    }
}
    }

    // Очистка памяти
    freeIniEntries(entries);
	g_thread_exit(NULL);
}
