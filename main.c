#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "books.h"
#include "users.h"
#include "library.h"

GtkWidget *window;
GtkWidget *stack;
GtkWidget *grid[4];
GtkWidget *box[2];
GtkWidget *go[5];
GtkWidget *retour[5];
GtkWidget *homeLabel[4];

EBook *Books;
EBorrowedBook *BorrowedBooks;
Stack ReturnedBooks;

static void on_activate(GtkApplication *app)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title (GTK_WINDOW (window), "Library Book Management System");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 900);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 300);
    gtk_window_set_child(GTK_WINDOW(window), stack);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, 
                                        "button { font-size: 20px; font-weight: normal; border-radius: 10px;} "
                                        "button:hover { font-size: 25px; font-weight: bold;} "
                                        ".bold-label { font-size: 25px; font-weight: bold;}"
                                        "entry { min-height: 50px; font-size: 20px; font-weight: 600; padding: 10px; border-radius: 10px;}"
                                        ".exist { color: red;}"
                                        ".correct { color: green;}"
                                        ".title-label { color: blue;font-size: 25px; font-weight: bold;}");
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    grid[0] = gtk_grid_new ();
    
    gtk_grid_set_column_spacing(GTK_GRID(grid[0]), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid[0]), 20);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid[0]), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid[0]), TRUE);

    go[0] = gtk_button_new_with_mnemonic("Add a Book");
    go[1] = gtk_button_new_with_mnemonic("Borrow a Book");
    go[2] = gtk_button_new_with_mnemonic("Update a Task Status");
    go[3] = gtk_button_new_with_mnemonic("Display Tasks");
    go[4] = gtk_button_new_with_mnemonic("Search by Priority");

    homeLabel[0] = gtk_label_new ("");
    homeLabel[1] = gtk_label_new ("");
    homeLabel[2] = gtk_label_new ("");
    homeLabel[3] = gtk_label_new ("");

    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(homeLabel[0]), 0, 0, 22, 1);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(homeLabel[1]), 0, 0, 1, 14);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(homeLabel[2]), 0, 13, 22, 1);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(homeLabel[3]), 19, 0, 1, 14);

    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(go[0]), 1, 1, 10, 4);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(go[1]), 11, 1, 10, 4);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(go[2]), 1, 5, 10, 4);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(go[3]), 11, 5, 10, 4);
    gtk_grid_attach (GTK_GRID (grid[0]), GTK_WIDGET(go[4]), 1, 9, 20, 4);

    gtk_stack_add_titled(GTK_STACK(stack), grid[0], "grid_home", "Home");

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    Books = NULL;
    BorrowedBooks = NULL;
    InitStack(&ReturnedBooks);

    app = gtk_application_new ("stackof.holger.entry", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}