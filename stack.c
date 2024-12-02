#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

void CopyBook(Book *B1, Book B2)
{
    strcpy(B1->Id, B2.Id);
    strcpy(B1->Title, B2.Title);
    strcpy(B1->Author, B2.Author);
    B1->available = B2.available;
}

void InitStack(Stack *P)
{
    *P=NULL;
}

void Push(Stack *S, Book x)
{
    Stack V;
    V=malloc(sizeof(SNode));
    CopyBook(&(V->Data), x);
    V->Next=*S;
    *S=V;
}

void Pop(Stack *S, Book *x)
{
    Stack V;
    V=*S;
    CopyBook(x, V->Data);
    *S=(*S)->Next;
    free(V);
}

int isSEmpty(Stack S)
{
    if (S==NULL) return 1;
    else return 0;
}

Book Top(Stack S)
{
    return S->Data;
}
