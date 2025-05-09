#ifndef GTK_HELPERS_H
#define GTK_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
GtkWidget* calc_window(void);
static void load_css(void);
static void button_clicked(GtkWidget *widget, gpointer data);
char *calculate(char *);

#endif