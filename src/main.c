#include "./header.h"

GtkWidget *calc_window(void)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1.0);
    gtk_entry_set_text(GTK_ENTRY(entry), "");
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
    gtk_widget_set_name(entry, "calc-entry");
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 5);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    const char *button_labels[17] = {
        "C", "+", "7", "8", "9", "*", "4", "5", "6", "-", "1", "2", "3", "/", ".", "0", "="};

    int button_index = 0;
    for (int row = 0; row < 5 && button_index < 17; row++)
    {
        int cal = 0;
        for (int col = 0; col < 4 && button_index < 17; col++)
        {
            if (row == 0 && button_index > 1)
                break;

            GtkWidget *button = gtk_button_new_with_label(button_labels[button_index]);
            g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), entry);
            if (g_strcmp0(button_labels[button_index], "C") == 0)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(button), "function-button");
            }
            else if (g_strcmp0(button_labels[button_index], "+") == 0 ||
                     g_strcmp0(button_labels[button_index], "-") == 0 ||
                     g_strcmp0(button_labels[button_index], "*") == 0 ||
                     g_strcmp0(button_labels[button_index], "/") == 0 ||
                     g_strcmp0(button_labels[button_index], "=") == 0)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(button), "operator-button");
            }
            else
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(button), "number-button");
            }
            if (button_index == 0 || button_index == 1 || button_index == 16)
            {
                if (button_index == 16)
                    cal = 2;
                gtk_grid_attach(GTK_GRID(grid), button, cal, row, 2, 1);
                cal += 2;
            }
            else
            {
                gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
            }
            button_index++;
        }
    }
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    return box;
}

static void load_css(void)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    GError *error = NULL;
    if (!gtk_css_provider_load_from_path(provider, "./resource/style.css", &error))
    {
        g_print("Failed to load CSS: %s\n", error->message);
        g_error_free(error);
    }

    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}

static void button_clicked(GtkWidget *widget, gpointer data)
{
    GtkEntry *entry = GTK_ENTRY(data);
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    if (g_strcmp0(button_label, "C") == 0)
    {
        gtk_entry_set_text(entry, "");
        g_print("Cleared entry\n");
    }
    else if (g_strcmp0(button_label, "=") == 0)
    {
        const char *expression = gtk_entry_get_text(entry);
        g_print("Evaluating: %s\n", expression);
        char *result = calculate((char *)expression);
        g_print("Result: %s\n", result);
        gtk_entry_set_text(entry, result);
        free(result);
    }
    else
    {
        const char *current = gtk_entry_get_text(entry);
        char *new_text = g_strconcat(current[0] == '0' && button_label[0] != '.' ? "" : current, button_label, NULL);
        gtk_entry_set_text(entry, new_text);
        g_print("Appended: %s\n", button_label);
        g_free(new_text);
    }
}

char *calculate(char *data)
{
    char *result = (char *)malloc(20 * sizeof(char));
    snprintf(result, 20, "Error: Unknown");

    if (!data || data[0] == '\0')
    {
        snprintf(result, 20, "Error: Empty input");
        return result;
    }

    char *a = (char *)malloc(20 * sizeof(char));
    char *b = (char *)malloc(20 * sizeof(char));
    a[0] = '\0';
    b[0] = '\0';
    char ch = '\0';
    int flag = 1;
    size_t a_idx = 0, b_idx = 0;
    int decimal_count = 0;

    for (size_t i = 0; data[i] != '\0'; i++)
    {
        if (isdigit(data[i]))
        {
            if (flag)
            {
                if (a_idx < 19)
                {
                    a[a_idx++] = data[i];
                    a[a_idx] = '\0';
                }
                else
                {
                    snprintf(result, 20, "Error: Number too long");
                    free(a);
                    free(b);
                    return result;
                }
            }
            else
            {
                if (b_idx < 19)
                {
                    b[b_idx++] = data[i];
                    b[b_idx] = '\0';
                }
                else
                {
                    snprintf(result, 20, "Error: Number too long");
                    free(a);
                    free(b);
                    return result;
                }
            }
        }
        else if (data[i] == '.')
        {
            if (decimal_count >= 1)
            {
                snprintf(result, 20, "Error: Invalid number");
                free(a);
                free(b);
                return result;
            }
            decimal_count++;
            if (flag)
            {
                if (a_idx < 19)
                {
                    a[a_idx++] = data[i];
                    a[a_idx] = '\0';
                }
            }
            else
            {
                if (b_idx < 19)
                {
                    b[b_idx++] = data[i];
                    b[b_idx] = '\0';
                }
            }
        }
        else if (data[i] == '+' || data[i] == '-' || data[i] == '*' || data[i] == '/')
        {
            if (ch != '\0' || a_idx == 0)
            {
                snprintf(result, 20, "Error: Invalid op");
                free(a);
                free(b);
                return result;
            }
            ch = data[i];
            flag = 0;
            decimal_count = 0;
        }
        else
        {
            snprintf(result, 20, "Error: Invalid char");
            free(a);
            free(b);
            return result;
        }
    }

    if (a_idx == 0)
    {
        snprintf(result, 20, "Error: No number");
        free(a);
        free(b);
        return result;
    }
    if (ch != '\0' && b_idx == 0)
    {
        snprintf(result, 20, "Error: Incomplete");
        free(a);
        free(b);
        return result;
    }

    double num_a = atof(a);
    double num_b = ch ? atof(b) : 0.0;
    double res = 0.0;

    switch (ch)
    {
    case '+':
        res = num_a + num_b;
        snprintf(result, 20, "%.1f", res);
        break;
    case '-':
        res = num_a - num_b;
        snprintf(result, 20, "%.1f", res);
        break;
    case '*':
        res = num_a * num_b;
        snprintf(result, 20, "%.1f", res);
        break;
    case '/':
        if (num_b == 0.0)
        {
            snprintf(result, 20, "Error: Div by 0");
            break;
        }
        res = num_a / num_b;
        snprintf(result, 20, "%.1f", res);
        break;
    default:
        snprintf(result, 20, "%.1f", num_a);
        break;
    }

    free(a);
    free(b);
    return result;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CALCULATOR");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 700);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    load_css();

    GtkWidget *calc = calc_window();
    gtk_container_add(GTK_CONTAINER(window), calc);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}