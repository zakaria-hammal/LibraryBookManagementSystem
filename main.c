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
GtkWidget *submit[5];
GtkWidget *homeLabel[4];
GtkWidget *entry[3];
GtkEntryBuffer *buffer[3];
GtkWidget *insertLabel[7];

EBook *Inventory;
EBorrowedBook *BorrowedBooks;
Stack ReturnedBooks;

static void Borrow(GtkWidget *widget, gpointer user_data)
{

}

static void Add(GtkWidget *widget, gpointer user_data)
{
    char *string_Id = g_strdup(gtk_entry_buffer_get_text(buffer[0]));
    char id[13] = "";

    strcpy(id, string_Id);
    g_print("Id : %s \n", id);

    char *string_title = g_strdup(gtk_entry_buffer_get_text(buffer[1]));
    char title[51] = "";

    strcpy(title, string_title);
    g_print("Title : %s \n", title);

    char *string_author = g_strdup(gtk_entry_buffer_get_text(buffer[2]));
    char author[51] = "";

    strcpy(author, string_author);
    g_print("Author : %s \n", author);

    Book newBook;
    strcpy(newBook.Id, id);
    strcpy(newBook.Title, title);
    strcpy(newBook.Author, author);

    if(AddBook(&Inventory, newBook))
    {
        gtk_label_set_label(GTK_LABEL(insertLabel[0]), "Book Id Already exists !!!");
        gtk_widget_add_css_class(GTK_WIDGET(insertLabel[0]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(insertLabel[0]), "correct");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(insertLabel[0]), "Book added successfully");
        gtk_widget_add_css_class(GTK_WIDGET(insertLabel[0]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(insertLabel[0]), "exist");
    }

}

static void GoToBorrow(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_borrow");
}

static void GoToAdd(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_add");
}

static void Home(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_home");

    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer[1]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer[2]), "", 0);

    gtk_label_set_text(GTK_LABEL(insertLabel[0]), "");
}

static void on_activate(GtkApplication *app)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title (GTK_WINDOW (window), "Library Book Management System");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 800);

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
    go[2] = gtk_button_new_with_mnemonic("Return a Book");
    go[3] = gtk_button_new_with_mnemonic("Display Recent Books");
    go[4] = gtk_button_new_with_mnemonic("Display Users Borrow Queue");

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

    retour[0] = gtk_button_new_with_mnemonic("Return");
    g_signal_connect (retour[0], "clicked", G_CALLBACK(Home), NULL);
    retour[1] = gtk_button_new_with_mnemonic("Return");
    g_signal_connect (retour[1], "clicked", G_CALLBACK(Home), NULL);
    retour[2] = gtk_button_new_with_mnemonic("Return");
    g_signal_connect (retour[2], "clicked", G_CALLBACK(Home), NULL);
    retour[3] = gtk_button_new_with_mnemonic("Return");
    g_signal_connect (retour[3], "clicked", G_CALLBACK(Home), NULL);
    retour[4] = gtk_button_new_with_mnemonic("Return");
    g_signal_connect (retour[4], "clicked", G_CALLBACK(Home), NULL);

    grid[1] = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(grid[1]), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid[1]), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid[1]), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid[1]), TRUE);

    submit[0] = gtk_button_new_with_mnemonic("Add");
    g_signal_connect (submit[0], "clicked", G_CALLBACK(Add), NULL);

    buffer[0] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(buffer[0]), 12);

    entry[0] = gtk_entry_new_with_buffer(buffer[0]);

    buffer[1] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(buffer[1]), 50);

    entry[1] = gtk_entry_new_with_buffer(buffer[1]);

    buffer[2] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(buffer[2]), 50);

    entry[2] = gtk_entry_new_with_buffer(buffer[2]);

    insertLabel[0] = gtk_label_new("");
    insertLabel[1] = gtk_label_new("ID : ");
    gtk_label_set_xalign(GTK_LABEL(insertLabel[1]), 0.0);
    insertLabel[2] = gtk_label_new("Title : ");
    gtk_label_set_xalign(GTK_LABEL(insertLabel[2]), 0.0);
    insertLabel[3] = gtk_label_new("Author : ");
    gtk_label_set_xalign(GTK_LABEL(insertLabel[3]), 0.0);
    insertLabel[4] = gtk_label_new("");

    gtk_widget_add_css_class(GTK_WIDGET(insertLabel[0]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(insertLabel[1]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(insertLabel[2]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(insertLabel[3]), "bold-label");

    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(insertLabel[1]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(entry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(insertLabel[2]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(entry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(insertLabel[3]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(entry[2]), 1, 6, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(insertLabel[0]), 1, 11, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(submit[0]), 1, 12, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(retour[0]), 6, 12, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(insertLabel[4]), 0, 13, 12, 1);

    g_signal_connect (go[0], "clicked", G_CALLBACK(GoToAdd), NULL);

    grid[2] = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(grid[2]), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid[2]), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid[2]), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid[2]), TRUE);

    gtk_stack_add_titled(GTK_STACK(stack), grid[0], "grid_home", "Home");
    gtk_stack_add_titled(GTK_STACK(stack), grid[1], "grid_add", "Add");
    gtk_stack_add_titled(GTK_STACK(stack), grid[2], "grid_borrow", "Borrow");

    g_signal_connect (go[1], "clicked", G_CALLBACK(GoToBorrow), NULL);

    submit[1] = gtk_button_new_with_mnemonic("Borrow");
    g_signal_connect (submit[1], "clicked", G_CALLBACK(Borrow), NULL);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    Inventory = NULL;
    BorrowedBooks = NULL;
    InitStack(&ReturnedBooks);

    app = gtk_application_new ("stackof.holger.entry", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}