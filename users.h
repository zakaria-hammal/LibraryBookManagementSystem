typedef struct User User;
struct User
{
    char FirstName[51];
    char LastName[51];
};

void CopyUser(User *Destination, User Source)
{
    strcpy(Destination->FirstName, Source.FirstName);
    strcpy(Destination->LastName, Source.LastName);
}

typedef struct Element* EQueue;
typedef struct Element { User  Data; EQueue Next;} QNode;
typedef struct {EQueue Head,Tail;} Queue;

void InitQueue(Queue *Q)
{(*Q).Head=NULL; (*Q).Tail=NULL;}

void Enqueue(Queue *Q, User x)
{EQueue V;
 V=malloc(sizeof(QNode));
 CopyUser(&(V->Data), x);
 V->Next=NULL;
 if ((*Q).Head==NULL) (*Q).Head=V; 
 else (*Q).Tail->Next=V;
 (*Q).Tail=V;
}

void Dequeue(Queue *Q, User *x)
{EQueue V;
 V=(*Q).Head;
 CopyUser(x, V->Data);
 if ((*Q).Head==(*Q).Tail) {(*Q).Head=NULL; (*Q).Tail=NULL;}
 else (*Q).Head=(*Q).Head->Next; // ou  (*Q).Head=V->Next;
 free(V);
}

int isQEmpty(Queue Q)
{if (Q.Head==NULL) return 1;
 else return 0;
}

User Head(Queue Q)
{return Q.Head->Data;}
