#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

void CopyUser(User *Destination, User Source)
{
    strcpy(Destination->Id, Source.Id);
    strcpy(Destination->Name, Source.Name);
    strcpy(Destination->RequestedBookId, Source.RequestedBookId);
}

void InitQueue(Queue *Q)
{
    (*Q).Head=NULL; (*Q).Tail=NULL;
}

void Enqueue(Queue *Q, User x)
{
    EQueue V;
    V=malloc(sizeof(QNode));
    CopyUser(&(V->Data), x);
    V->Next=NULL;
    if ((*Q).Head==NULL) (*Q).Head=V; 
    else (*Q).Tail->Next=V;
    (*Q).Tail=V;
}

void Dequeue(Queue *Q, User *x)
{
    EQueue V;
    V=(*Q).Head;
    CopyUser(x, V->Data);
    if ((*Q).Head==(*Q).Tail) {(*Q).Head=NULL; (*Q).Tail=NULL;}
    else (*Q).Head=(*Q).Head->Next; // ou  (*Q).Head=V->Next;
    free(V);
}

int isQEmpty(Queue Q)
{
    if (Q.Head==NULL) return 1;
    else return 0;
}

User Head(Queue Q)
{
    return Q.Head->Data;
}
