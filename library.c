#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "library.h"

GtkWidget *window;
GtkWidget *stack;
GtkWidget *grid[4];
GtkWidget *box[2];
GtkWidget *go[5];
GtkWidget *retour[5];
GtkWidget *submit[5];
GtkWidget *homeLabel[4];
GtkWidget *addEntry[3];
GtkEntryBuffer *addBuffer[3];
GtkWidget *borrowEntry[3];
GtkEntryBuffer *borrowBuffer[3];
GtkWidget *returnEntry[2];
GtkEntryBuffer *returnBuffer[2];
GtkWidget *addLabel[6];
GtkWidget *borrowLabel[6];
GtkWidget *returnLabel[5];
GtkWidget *scrolled_window[2];
GtkWidget **stackLabel;
GtkWidget **queueLabel;
GtkWidget *headLabel[2];
GtkWidget *bottomLabel[2];
GtkWidget **tempLabel;
GtkWidget *queueEntry;
GtkEntryBuffer *queueBuffer;

EBook *Inventory;
EBorrowedBook *BorrowedBooks;
Stack ReturnedBooks;

int stackSize;
int queueSize;

int AddBook(EBook **L, Book book)
{
    if (L == NULL)
    {
        return 1;
    }

    EBook* B = *L;
    while (B != NULL)
    {
        if (!strcmp(B->Book.Id, book.Id)) {
            return 1;
        }

        B = B->next;
    }
    
    
    EBook* newBook = malloc(sizeof(EBook));
    if(newBook == NULL)
    {
        perror("Memory allocation failed");
        return 1;    
    }

    book.available = 1;
    CopyBook(&(newBook->Book), book);

    newBook->next = *L;
    *L = newBook;

    return 0;
}

int SearchBook(EBook *L, char Id[13])
{
    while(L != NULL && strcmp(L->Book.Id, Id) != 0)
    {
        L = L->next;
    }

    if(L == NULL)
    {
        return -1;
    }

    return L->Book.available;
}

int BorrowBook(EBook **L1, EBorrowedBook **L2, User user)
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, user.RequestedBookId);
    if(BookStatus == -1)
    {
        return -1;
    }

    EBook *Q = *L1;
    while (Q !=NULL && strcmp(Q->Book.Id, user.RequestedBookId) != 0)
    {
        Q = Q->next;
    }

    EBorrowedBook *P;

    if(BookStatus)
    {
        P = malloc(sizeof(EBorrowedBook));
        CopyBook(&(P->Book), Q->Book);
        InitQueue(&(P->UserQueue));
        P->next = *L2;
        CopyUser(&(P->Borrower), user);
        Q->Book.available = 0;
        *L2 = P;
    }
    else
    {
        P = *L2;
        while (P != NULL && strcmp(P->Book.Id, user.RequestedBookId) != 0)
        {
            P = P->next;
        }

        if(!strcmp(P->Borrower.Id, user.Id))
        {
            return 2;
        }

        Queue Q;
        User usr;
        InitQueue(&Q);
        int c = 0;

        while (!isQEmpty(P->UserQueue))
        {
            Dequeue(&(P->UserQueue), &usr);
            Enqueue(&Q, usr);

            if(!strcmp(user.Id, usr.Id))
            {
                c++;
            }
        }
        
        P->UserQueue = Q;

        if(c > 0)
        {
            return 3;
        }

        Enqueue(&(P->UserQueue), user);
    }

    return !BookStatus;
}

int ReturnBook(EBook **L1, EBorrowedBook **L2, char userId[13], char bookId[13], Stack *RBooks, int *sSize)
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, bookId);
    if(BookStatus == -1)
    {
        return -1;
    }

    if(BookStatus)
    {
        return 2;
    }

    if(!strcmp((*L2)->Book.Id, bookId))
    {
        if(strcmp((*L2)->Borrower.Id, userId) != 0)
        {
            return 1;
        }

        if(isQEmpty((*L2)->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Id, bookId) != 0)
            {
                Q = Q->next;
            }

            Q->Book.available = 1;

            EBorrowedBook *P = *L2;
            *L2 = (*L2)->next;
            free(P);

            Stack S;
            InitStack(&S);

            Book book;

            while(!isSEmpty(*RBooks))
            {
                Pop(RBooks, &book);

                if(strcmp(bookId, book.Id) != 0)
                {
                    Push(&S, book);
                }
            }

            (*sSize) = 0;
            while(!isSEmpty(S))
            {
                Pop(&S, &book);
                Push(RBooks, book);
                (*sSize)++;
            }

            Push(RBooks, Q->Book);
            (*sSize) += 1;

            return 0;
        }

        Dequeue(&((*L2)->UserQueue), &((*L2)->Borrower));

        return 0;
    }

    EBorrowedBook *P = *L2;

    while (P->next != NULL && strcmp(P->next->Book.Id, bookId) != 0)
    {
        P = P->next;
    }

    if(P->next == NULL)
    {
        return 1;
    }

    if(!strcmp(P->next->Book.Id, bookId))
    {
        if(strcmp(P->Borrower.Id, userId) != 0)
        {
            return 1;
        }

        if(isQEmpty(P->next->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Id, bookId) != 0)
            {
                Q = Q->next;
            }

            Q->Book.available = 1;

            Stack S;
            InitStack(&S);

            Book book;

            while(!isSEmpty(*RBooks))
            {
                Pop(RBooks, &book);

                if(strcmp(bookId, book.Id) != 0)
                {
                    Push(&S, book);
                }
            }

            (*sSize) = 0;
            while(!isSEmpty(S))
            {
                Pop(&S, &book);
                Push(RBooks, book);
                (*sSize)++;
            }

            Push(RBooks, Q->Book);

            EBorrowedBook *K =P->next;
            P->next= K->next;
            free(K);

            (*sSize)++;
            return 0;
        }

        Dequeue(&(P->next->UserQueue), &(P->next->Borrower));

        return 0;
    }
}

static void Display(GtkWidget *widget, gpointer user_data)
{
    if(tempLabel != NULL)
    {
        gtk_box_remove(GTK_BOX(box[1]), *tempLabel);

        free(tempLabel);
        tempLabel = NULL;

        gtk_box_remove(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
    }

    if(queueLabel != NULL)
    {
        for (int i = 0; i < queueSize; i++)
        {
            gtk_box_remove(GTK_BOX(box[1]), queueLabel[i]);
        }
        
        free(queueLabel);
        queueLabel = NULL;

        gtk_box_remove(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
    }
    
    queueSize = 0;
    char *string_book_id = g_strdup(gtk_entry_buffer_get_text(queueBuffer));
    char book_id[13] = "";

    strcpy(book_id, string_book_id);

    EBook *P = Inventory;

    while (P != NULL && strcmp(P->Book.Id, book_id) != 0)
    {
        P = P->next;
    }

    bottomLabel[1] = gtk_label_new_with_mnemonic("\n");
    
    if(P == NULL)
    {
        tempLabel = malloc(sizeof(GtkWidget*));
        *tempLabel = gtk_label_new_with_mnemonic("This Book does not exist !!!\n");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "bold-label");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "exist");
        gtk_label_set_xalign(GTK_LABEL(*tempLabel), 0.0);
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(*tempLabel));
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
        return;
    }

    if(P->Book.available)
    {
        tempLabel = malloc(sizeof(GtkWidget*));
        *tempLabel = gtk_label_new_with_mnemonic("This Book is available\n");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "bold-label");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "correct");
        gtk_label_set_xalign(GTK_LABEL(*tempLabel), 0.0);
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(*tempLabel));
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
        return;
    }

    EBorrowedBook *borrowed = BorrowedBooks;

    while (borrowed != NULL && strcmp(borrowed->Book.Id, book_id) != 0)
    {
        borrowed = borrowed->next;
    }

    if(borrowed == NULL)
    {
        return;
    }

    Queue T;
    InitQueue(&T);

    User user;

    if(isQEmpty(borrowed->UserQueue))
    {
        tempLabel = malloc(sizeof(GtkWidget*));
        *tempLabel = gtk_label_new_with_mnemonic("This Book is borrowed but it's queue is empty\n");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "bold-label");
        gtk_widget_add_css_class(GTK_WIDGET(*tempLabel), "notExist");
        gtk_label_set_xalign(GTK_LABEL(*tempLabel), 0.0);
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(*tempLabel));
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
        return;
    }

    while (!isQEmpty(borrowed->UserQueue))
    {
        Dequeue(&(borrowed->UserQueue), &user);
        Enqueue(&T, user);
        queueSize += 1;
    }

    queueLabel = malloc(queueSize * sizeof(GtkWidget*));
    char stat[100] = "";

    for (int i = 0; i < queueSize; i++)
    {
        Dequeue(&T, &user);
        Enqueue(&(borrowed->UserQueue), user);
        strcpy(stat, "\nUser Id :");
        strcat(stat, user.Id);
        strcat(stat, "\nUser name :");
        strcat(stat, user.Name);
        queueLabel[i] = gtk_label_new_with_mnemonic(stat);
        gtk_widget_add_css_class(GTK_WIDGET(queueLabel[i]), "bold-label");
        gtk_label_set_xalign(GTK_LABEL(queueLabel[i]), 0.0);
        gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(queueLabel[i]));
    }

    gtk_box_append(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
}

static void Return(GtkWidget *widget, gpointer user_data)
{
    char *string_user_id = g_strdup(gtk_entry_buffer_get_text(returnBuffer[0]));
    char user_id[13] = "";

    strcpy(user_id, string_user_id);

    char *string_book_id = g_strdup(gtk_entry_buffer_get_text(returnBuffer[1]));
    char book_id[13] = "";

    strcpy(book_id, string_book_id);

    int status = ReturnBook(&Inventory, &BorrowedBooks, user_id, book_id, &ReturnedBooks, &stackSize);

    EBook *P = Inventory;

    while (P != NULL)
    {
        P = P->next;
    }
    
    if(status == -1)
    {
        gtk_label_set_label(GTK_LABEL(returnLabel[3]), "Book Doesn't Exist !!!");
        gtk_widget_add_css_class(GTK_WIDGET(returnLabel[3]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "notExist");
    }
    else if(status == 2)
    {
        gtk_label_set_label(GTK_LABEL(returnLabel[3]), "Book is not even borrowed !!!");
        gtk_widget_add_css_class(GTK_WIDGET(returnLabel[3]), "notExist");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "exist");
    }
    else if(status)
    {
        gtk_label_set_label(GTK_LABEL(returnLabel[3]), "You can't return a book that you don't have !!!");
        gtk_widget_add_css_class(GTK_WIDGET(returnLabel[3]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "notExist");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(returnLabel[3]), "Book Returned Successfully !!!");
        gtk_widget_add_css_class(GTK_WIDGET(returnLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(returnLabel[3]), "notExist");
    }
    
}

static void Borrow(GtkWidget *widget, gpointer user_data)
{
    char *string_user_id = g_strdup(gtk_entry_buffer_get_text(borrowBuffer[0]));
    char user_id[13] = "";

    strcpy(user_id, string_user_id);

    char *string_name = g_strdup(gtk_entry_buffer_get_text(borrowBuffer[1]));
    char name[51] = "";

    strcpy(name, string_name);

    char *string_book_id = g_strdup(gtk_entry_buffer_get_text(borrowBuffer[2]));
    char book_id[13] = "";

    strcpy(book_id, string_book_id);

    User user;
    strcpy(user.Id, user_id);
    strcpy(user.Name, name);
    strcpy(user.RequestedBookId, book_id);

    int status = BorrowBook(&Inventory, &BorrowedBooks, user);

    if(status == -1)
    {
        gtk_label_set_label(GTK_LABEL(borrowLabel[4]), "Book Doesn't Exist !!!");
        gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "notExist");
    }
    else if(status == 2)
    {
        gtk_label_set_label(GTK_LABEL(borrowLabel[4]), "You already have the book !!!");
        gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "notExist");
    }
    else if(status == 3)
    {
        gtk_label_set_label(GTK_LABEL(borrowLabel[4]), "Book has not been returned yet\nYou are still in the queue");
        gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "notExist");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "exist");
    }
    else if(status)
    {
        gtk_label_set_label(GTK_LABEL(borrowLabel[4]), "Book Already Taken\nYou are added to the Queue");
        gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "notExist");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "exist");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(borrowLabel[4]), "You can take the book !!!");
        gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "notExist");
        gtk_widget_remove_css_class(GTK_WIDGET(borrowLabel[4]), "exist");
    }
}

static void Add(GtkWidget *widget, gpointer user_data)
{
    char *string_Id = g_strdup(gtk_entry_buffer_get_text(addBuffer[0]));
    char id[13] = "";

    strcpy(id, string_Id);
    g_print("Id : %s \n", id);

    char *string_title = g_strdup(gtk_entry_buffer_get_text(addBuffer[1]));
    char title[51] = "";

    strcpy(title, string_title);
    g_print("Title : %s \n", title);

    char *string_author = g_strdup(gtk_entry_buffer_get_text(addBuffer[2]));
    char author[51] = "";

    strcpy(author, string_author);
    g_print("Author : %s \n", author);

    Book newBook;
    strcpy(newBook.Id, id);
    strcpy(newBook.Title, title);
    strcpy(newBook.Author, author);

    if(AddBook(&Inventory, newBook))
    {
        gtk_label_set_label(GTK_LABEL(addLabel[4]), "Book Id Already exists !!!");
        gtk_widget_add_css_class(GTK_WIDGET(addLabel[4]), "exist");
        gtk_widget_remove_css_class(GTK_WIDGET(addLabel[4]), "correct");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(addLabel[4]), "Book added successfully");
        gtk_widget_add_css_class(GTK_WIDGET(addLabel[4]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(addLabel[4]), "exist");
    }

}

static void GoToQueue(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_queue");
}

static void GoToStack(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_stack");

    bottomLabel[0] = gtk_label_new_with_mnemonic("\n");

    if(isSEmpty(ReturnedBooks))
    {
        stackLabel = malloc(sizeof(GtkWidget*));
        *stackLabel = gtk_label_new_with_mnemonic("\nNo Returned Books Yet");
        gtk_widget_add_css_class(GTK_WIDGET(*stackLabel), "bold-label");
        gtk_label_set_xalign(GTK_LABEL(*stackLabel), 0.0);
        gtk_box_append(GTK_BOX(box[0]), GTK_WIDGET(*stackLabel));
        gtk_box_append(GTK_BOX(box[0]), GTK_WIDGET(bottomLabel[0]));
        g_print("Zero\n");
        return;
    }

    stackLabel = malloc(stackSize * sizeof(GtkWidget*));
    char stat[200] = "";

    printf("%d\n", stackSize);

    Stack T;
    InitStack(&T);

    Book book;

    for (int i = 0; i < stackSize; i++)
    {
        g_print("%d\n", i);
        Pop(&ReturnedBooks, &book);
        Push(&T, book);
        strcpy(stat, "\nBook Id : ");
        strcat(stat, book.Id);
        strcat(stat, "\nTitle : ");
        strcat(stat, book.Title);
        strcat(stat, "\nAuthor : ");
        strcat(stat, book.Author);
        strcat(stat, "\n");
        stackLabel[i] = gtk_label_new_with_mnemonic(stat);
        gtk_widget_add_css_class(GTK_WIDGET(stackLabel[i]), "bold-label");
        gtk_label_set_xalign(GTK_LABEL(stackLabel[i]), 0.0);
        gtk_box_append(GTK_BOX(box[0]), GTK_WIDGET(stackLabel[i]));
    }

    gtk_box_append(GTK_BOX(box[0]), GTK_WIDGET(bottomLabel[0]));
    
    while(!isSEmpty(T))
    {
        Pop(&T, &book);
        Push(&ReturnedBooks, book);
    }
}

static void GoToReturn(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "grid_return");
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

    if(tempLabel != NULL)
    {
        gtk_box_remove(GTK_BOX(box[1]), *tempLabel);

        free(tempLabel);
        tempLabel = NULL;

        gtk_box_remove(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
    }

    if(queueLabel != NULL)
    {
        for (int i = 0; i < queueSize; i++)
        {
            gtk_box_remove(GTK_BOX(box[1]), queueLabel[i]);
        }
        
        free(queueLabel);
        queueLabel = NULL;

        gtk_box_remove(GTK_BOX(box[1]), GTK_WIDGET(bottomLabel[1]));
    }

    if(stackLabel != NULL)
    {
        if(isSEmpty(ReturnedBooks))
        {
            gtk_box_remove(GTK_BOX(box[0]), GTK_WIDGET(stackLabel[0]));
            free(stackLabel);
            stackLabel = NULL;
        }
        else
        {
            for (int i = 0; i < stackSize; i++)
            {
                gtk_box_remove(GTK_BOX(box[0]), GTK_WIDGET(stackLabel[i]));
            }

            free(stackLabel);
            stackLabel = NULL;
        }
        
        gtk_box_remove(GTK_BOX(box[0]), GTK_WIDGET(bottomLabel[0]));
    }

    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(addBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(addBuffer[1]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(addBuffer[2]), "", 0);

    gtk_label_set_text(GTK_LABEL(addLabel[4]), "");

    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(borrowBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(borrowBuffer[1]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(borrowBuffer[2]), "", 0);

    gtk_label_set_text(GTK_LABEL(borrowLabel[4]), "");

    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(returnBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(returnBuffer[1]), "", 0);

    gtk_label_set_text(GTK_LABEL(returnLabel[3]), "");

    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(queueBuffer), "", 0);
}

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
                                        ".notExist { color: orange;}"
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

    addBuffer[0] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(addBuffer[0]), 12);

    addEntry[0] = gtk_entry_new_with_buffer(addBuffer[0]);

    addBuffer[1] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(addBuffer[1]), 50);

    addEntry[1] = gtk_entry_new_with_buffer(addBuffer[1]);

    addBuffer[2] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(addBuffer[2]), 50);

    addEntry[2] = gtk_entry_new_with_buffer(addBuffer[2]);

    addLabel[0] = gtk_label_new("");
    addLabel[1] = gtk_label_new("Book ID : ");
    gtk_label_set_xalign(GTK_LABEL(addLabel[1]), 0.0);
    addLabel[2] = gtk_label_new("Title : ");
    gtk_label_set_xalign(GTK_LABEL(addLabel[2]), 0.0);
    addLabel[3] = gtk_label_new("Author : ");
    gtk_label_set_xalign(GTK_LABEL(addLabel[3]), 0.0);
    addLabel[4] = gtk_label_new("");
    addLabel[5] = gtk_label_new("");

    gtk_widget_add_css_class(GTK_WIDGET(addLabel[1]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(addLabel[2]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(addLabel[3]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(addLabel[4]), "bold-label");

    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[0]), 0, 0, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[1]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addEntry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[2]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addEntry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[3]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addEntry[2]), 1, 6, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[4]), 1, 7, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(submit[0]), 1, 8, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(retour[0]), 6, 8, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[1]), GTK_WIDGET(addLabel[5]), 0, 9, 12, 1);

    g_signal_connect (go[0], "clicked", G_CALLBACK(GoToAdd), NULL);

    grid[2] = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(grid[2]), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid[2]), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid[2]), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid[2]), TRUE);

    g_signal_connect (go[1], "clicked", G_CALLBACK(GoToBorrow), NULL);
    submit[1] = gtk_button_new_with_mnemonic("Borrow");
    g_signal_connect (submit[1], "clicked", G_CALLBACK(Borrow), NULL);

    borrowBuffer[0] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(borrowBuffer[0]), 12);

    borrowEntry[0] = gtk_entry_new_with_buffer(borrowBuffer[0]);

    borrowBuffer[1] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(borrowBuffer[1]), 50);

    borrowEntry[1] = gtk_entry_new_with_buffer(borrowBuffer[1]);

    borrowBuffer[2] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(borrowBuffer[2]), 12);

    borrowEntry[2] = gtk_entry_new_with_buffer(borrowBuffer[2]);

    borrowLabel[0] = gtk_label_new("");
    borrowLabel[1] = gtk_label_new("User ID : ");
    gtk_label_set_xalign(GTK_LABEL(borrowLabel[1]), 0.0);
    borrowLabel[2] = gtk_label_new("Name : ");
    gtk_label_set_xalign(GTK_LABEL(borrowLabel[2]), 0.0);
    borrowLabel[3] = gtk_label_new("Requested Book ID : ");
    gtk_label_set_xalign(GTK_LABEL(borrowLabel[3]), 0.0);
    borrowLabel[4] = gtk_label_new("");
    borrowLabel[5] = gtk_label_new("");

    gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[1]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[2]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[3]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(borrowLabel[4]), "bold-label");

    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[0]), 0, 0, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[1]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowEntry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[2]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowEntry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[3]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowEntry[2]), 1, 6, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[4]), 1, 7, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(submit[1]), 1, 8, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(retour[1]), 6, 8, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[2]), GTK_WIDGET(borrowLabel[5]), 0, 9, 12, 1);

    grid[3] = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(grid[3]), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid[3]), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid[3]), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid[3]), TRUE);

    g_signal_connect (go[2], "clicked", G_CALLBACK(GoToReturn), NULL);
    submit[2] = gtk_button_new_with_mnemonic("Return the book");
    g_signal_connect (submit[2], "clicked", G_CALLBACK(Return), NULL);

    returnBuffer[0] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(returnBuffer[0]), 12);

    returnEntry[0] = gtk_entry_new_with_buffer(returnBuffer[0]);

    returnBuffer[1] = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(returnBuffer[1]), 12);

    returnEntry[1] = gtk_entry_new_with_buffer(returnBuffer[1]);

    returnLabel[0] = gtk_label_new("");
    returnLabel[1] = gtk_label_new("User ID : ");
    gtk_label_set_xalign(GTK_LABEL(returnLabel[1]), 0.0);
    returnLabel[2] = gtk_label_new("Book ID : ");
    gtk_label_set_xalign(GTK_LABEL(returnLabel[2]), 0.0);
    returnLabel[3] = gtk_label_new("");
    returnLabel[4] = gtk_label_new("");

    gtk_widget_add_css_class(GTK_WIDGET(returnLabel[1]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(returnLabel[2]), "bold-label");
    gtk_widget_add_css_class(GTK_WIDGET(returnLabel[3]), "bold-label");

    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnLabel[0]), 0, 0, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnLabel[1]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnEntry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnLabel[2]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnEntry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnLabel[3]), 1, 5, 12, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(submit[2]), 1, 6, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(retour[2]), 6, 6, 5, 1);
    gtk_grid_attach(GTK_GRID(grid[3]), GTK_WIDGET(returnLabel[4]), 0, 7, 12, 1);

    g_signal_connect (go[3], "clicked", G_CALLBACK(GoToStack), NULL);

    box[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box[0], GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box[0], GTK_ALIGN_CENTER);

    headLabel[0] = gtk_label_new_with_mnemonic("\n");

    gtk_box_append(GTK_BOX(box[0]), headLabel[0]);
    gtk_box_append(GTK_BOX(box[0]), retour[3]);

    scrolled_window[0] = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[0]), 
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC
    );

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window[0]), box[0]);

    g_signal_connect (go[4], "clicked", G_CALLBACK(GoToQueue), NULL);

    box[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box[1], GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box[1], GTK_ALIGN_CENTER);

    headLabel[1] = gtk_label_new_with_mnemonic("\nBook Id :");
    gtk_widget_add_css_class(GTK_WIDGET(headLabel[1]), "bold-label");

    queueBuffer = gtk_entry_buffer_new("", -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(queueBuffer), 12);

    queueEntry = gtk_entry_new_with_buffer(queueBuffer);

    submit[3] = gtk_button_new_with_mnemonic("Display");
    g_signal_connect (submit[3], "clicked", G_CALLBACK(Display), NULL);

    gtk_box_append(GTK_BOX(box[1]), retour[4]);
    gtk_box_append(GTK_BOX(box[1]), headLabel[1]);
    gtk_box_append(GTK_BOX(box[1]), queueEntry);
    gtk_box_append(GTK_BOX(box[1]), submit[3]);

    scrolled_window[1] = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[1]), 
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC
    );

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window[1]), box[1]);

    gtk_stack_add_titled(GTK_STACK(stack), grid[0], "grid_home", "Home");
    gtk_stack_add_titled(GTK_STACK(stack), grid[1], "grid_add", "Add");
    gtk_stack_add_titled(GTK_STACK(stack), grid[2], "grid_borrow", "Borrow");
    gtk_stack_add_titled(GTK_STACK(stack), grid[3], "grid_return", "Return");
    gtk_stack_add_titled(GTK_STACK(stack), scrolled_window[0], "grid_stack", "Stack");
    gtk_stack_add_titled(GTK_STACK(stack), scrolled_window[1], "grid_queue", "Queue");

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    Inventory = NULL;
    BorrowedBooks = NULL;
    InitStack(&ReturnedBooks);

    stackLabel = NULL;
    queueLabel = NULL;
    tempLabel = NULL;

    stackSize = 0;
    queueSize = 0;

    app = gtk_application_new ("stackof.holger.entry", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
