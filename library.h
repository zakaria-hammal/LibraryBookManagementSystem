typedef struct EBook EBook;
struct EBook
{
    Book Book;
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

    newBook->Book.available = 1;
    CopyBook(&(newBook->Book), book);

    newBook->next = *L;
    *L = newBook;

    return 0;
}

int SearchBook(EBook *L, char Id[51])
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

int BorrowBook(EBook **L1, EBorrowedBook **L2, User user, char Id[51])
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, Id);
    if(BookStatus == -1)
    {
        return -1;
    }


    EBook *Q = *L1;
    while (Q !=NULL && strcmp(Q->Book.Id, Id) != 0)
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
        while (P != NULL && strcmp(P->Book.Id, Id) != 0)
        {
            P = P->next;
        }

        Enqueue(&(P->UserQueue), user);
    }

    return BookStatus;
}

int ReturnBook(EBook **L1, EBorrowedBook **L2, char Id[51], Stack *ReturnedBooks)
{
    if(L1 == NULL || L2 == NULL)
    {
        return -1;
    }

    int BookStatus = SearchBook(*L1, Id);
    if(BookStatus == -1)
    {
        return -1;
    }

    if(!strcmp((*L2)->Book.Id, Id))
    {
        if(isQEmpty((*L2)->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Id, Id) != 0)
            {
                Q = Q->next;
            }

            Q->Book.available = 1;
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

    while (P->next != NULL && strcmp(P->next->Book.Id, Id) != 0)
    {
        P = P->next;
    }

    if(P->next == NULL)
    {
        return 1;
    }

    if(!strcmp(P->next->Book.Id, Id))
    {
        if(isQEmpty(P->next->UserQueue))
        {
            EBook *Q = *L1;
            while (Q !=NULL && strcmp(Q->Book.Id, Id) != 0)
            {
                Q = Q->next;
            }

            Q->Book.available = 1;
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