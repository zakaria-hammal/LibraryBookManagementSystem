#ifndef LIBRARY
#define LIBRARY

typedef struct Book Book;
struct Book
{
    char Id[13];
    char Title[51];
    char Author[51];
    int available;
};

typedef struct Estack* Stack;
typedef struct Estack { Book  Data; Stack Next;} SNode;

typedef struct User User;
struct User
{
    char Id[13];
    char Name[51];
    char RequestedBookId[13];
};

typedef struct Element* EQueue;
typedef struct Element { User  Data; EQueue Next;} QNode;
typedef struct {EQueue Head,Tail;} Queue;

typedef struct EUser EUser;
struct EUser
{
    User User;
    EUser* next;
};

typedef struct EBook EBook;
struct EBook
{
    Book Book;
    EBook* next;
};

typedef struct EBorrowedBook EBorrowedBook;
struct EBorrowedBook
{
    Book Book;
    Queue UserQueue;
    User Borrower;
    EBorrowedBook* next;
};

void CopyBook(Book *B1, Book B2);
void InitStack(Stack *P);
void Push(Stack *S, Book x);
void Pop(Stack *S, Book *x);
int isSEmpty(Stack S);
Book Top(Stack S);

void CopyUser(User *Destination, User Source);
void InitQueue(Queue *Q);
void Enqueue(Queue *Q, User x);
void Dequeue(Queue *Q, User *x);
int isQEmpty(Queue Q);
User Head(Queue Q);

#endif