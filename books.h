typedef struct Book Book;
struct Book
{
    char Title[51];
    char Author[51];
    char Publisher[51];
    int PublishYear;
    char Description[1001];
};

void CopyBook(Book *B1, Book B2)
{
    B1->PublishYear = B2.PublishYear;
    strcpy(B1->Title, B2.Title);
    strcpy(B1->Author, B2.Author);
    strcpy(B1->Publisher, B2.Publisher);
    strcpy(B1->Description, B2.Description);
}

typedef struct Estack* Stack;
typedef struct Estack { Book  Data; Stack Next;} SNode;

void InitStack(Stack *P)
{*P=NULL;}

void Push(Stack *S, Book x)
{Stack V;
 V=malloc(sizeof(SNode));
 CopyBook(&(V->Data), x);
 V->Next=*S;
 *S=V;
}

void Pop(Stack *S, Book *x)
{Stack V;
 CopyBook(x, V->Data);
 V=*S;
 *S=(*S)->Next;
 free(V);
}

int isSEmpty(Stack S)
{if (S==NULL) return 1;
 else return 0;
}

Book Top(Stack S)
{return S->Data;}
