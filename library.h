typedef struct EBook EBook;
struct EBook
{
    Book Book;
    int available;
    EBook* next;
};

typedef struct EUser EUser;
struct EUser
{
    User User;
    EUser* next;
};

typedef struct EBorrowedBook EBorrowedBook;
struct EBorrowedBook
{
    Book Book;
    Queue UserQueue;
    User Borrower;
    EBorrowedBook* next;
};

int AddBook(EBook **L, Book book)
{
    if (L == NULL)
    {
        return 1;
    }

    EBook* B = *L;
    while (B != NULL)
    {
        if (!strcmp(B->Book.Title, book.Title)) {
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

    newBook->available = 1;
    CopyBook(&(newBook->Book), book);

    newBook->next = *L;
    *L = newBook;

    return 0;
}

int SearchBook(EBook *L, char Title[51])
{
    while(L != NULL && strcmp(L->Book.Title, Title) != 0)
    {
        L = L->next;
    }

    if(L == NULL)
    {
        return -1;
    }

    return L->available;
}

int BorrowBook(EBook **L1, EBorrowedBook **L2, User user, char Title[51])
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, Title);
    if(BookStatus == -1)
    {
        return -1;
    }


    EBook *Q = *L1;
    while (Q !=NULL && strcmp(Q->Book.Title, Title) != 0)
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
        Q->available = 0;
        *L2 = P;
    }
    else
    {
        P = *L2;
        while (P != NULL && strcmp(P->Book.Title, Title) != 0)
        {
            P = P->next;
        }

        Enqueue(&(P->UserQueue), user);
    }

    return BookStatus;
}

int ReturnBook(EBook **L1, EBorrowedBook **L2, char Title[51], Stack *ReturnedBooks)
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, Title);
    if(BookStatus == -1)
    {
        return -1;
    }

    if(!strcmp((*L2)->Book.Title, Title))
    {
        if(isQEmpty((*L2)->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Title, Title) != 0)
            {
                Q = Q->next;
            }

            Q->available = 1;
            Push(ReturnedBooks, Q->Book);

            EBorrowedBook *P = *L2;
            *L2 = (*L2)->next;
            free(P);

            return 0;
        }

        Dequeue(&((*L2)->UserQueue), &((*L2)->Borrower));

        return 0;
    }

    EBorrowedBook *P = *L2;

    while (P->next != NULL && strcmp(P->next->Book.Title, Title) != 0)
    {
        P = P->next;
    }

    if(P->next == NULL)
    {
        return 1;
    }

    if(!strcmp(P->next->Book.Title, Title))
    {
        if(isQEmpty(P->next->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Title, Title) != 0)
            {
                Q = Q->next;
            }

            Q->available = 1;
            Push(ReturnedBooks, Q->Book);

            EBorrowedBook *K =P->next;
            P->next= K->next;
            free(K);

            return 0;
        }

        Dequeue(&(P->next->UserQueue), &(P->next->Borrower));

        return 0;
    }
}