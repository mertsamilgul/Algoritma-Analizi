#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define M 250000
#define R 13
#define MAXBUFFER 4000
struct node{
    char *name; // film-aktor ismi
    struct node **edge; // bagli oldugu nodelar listesi
    int size; // bagli oldugu node sayisi
    int visited;
};

struct queue{ // queue yapisi
    struct node *val;
    struct queue *next;
};

struct queue *head=NULL;
struct queue *tail=NULL;

void push(struct node* i)
{
    struct queue *j=(struct queue*)malloc(sizeof(struct queue));
    j->val=i;
    j->next=NULL;
    if(!head)
        head=tail=j;
    else
        tail=tail->next=j;
}

struct node *pop()
{
    if(head==NULL)
        return NULL; // bos queue'da pop yapiliyorsa NULL doner
    struct queue *i=head;
    head=head->next;
    return i->val;
}

struct node *newNode(char *name)
{
    struct node *i=(struct node*)malloc(sizeof(struct node));
    char *x=(char*)malloc(strlen(name)+1);
    strcpy(x,name);
    i->edge=NULL;
    i->name=x;
    i->size=0;
    i->visited=0;
    return i;
}

void addEdge(struct node *first,struct node *second)
{
    if(!first->edge) // edge bos
        first->edge=(struct node**)malloc(sizeof(struct node*)*20);
    else if(first->size%20==0) // edge dolu
        first->edge=(struct node**)realloc(first->edge,sizeof(struct node*)*(20+first->size));// yer ac
    first->edge[first->size]=second;
    first->size++;
}

unsigned long hash(char *name) // film veya aktorlerin ismine gore
{                              // key'ini cikartip hash adresini bulmak icin
    unsigned long key=0;
    unsigned long c=1;
    int i;
    for(i=0;name[i];i++)
    {
        if(name[i]==' '||name[i]==',')
            c=1;
        else
        {
            key+=name[i]*c;
            c*=R;
            c=c%M;
            key=key%M;
        }
    }
    return key;
}

int getIndex(char *name,struct node **hashT) // listede varsa o indexi, yoksa yerlesecegi bos adresin indexini dondurur
{
    int index=hash(name);
    while(hashT[index] && strcmp(hashT[index]->name,name))
        if(index!=M)
            index++;
    return index;
}

void createHashTable(FILE *fp,struct node **hashT)
{
    char buffer[MAXBUFFER],*token;
    int index;
    struct node *movie,*actor,*i;
    while(fgets(buffer,MAXBUFFER,fp)) // tum satirlar teker teker buffera atilir
    {
        token=strtok(buffer,"/"); // film ismi
        movie=newNode(token); // yeni film node'u
        while(token=strtok(NULL,"/\n)")) // aktor ismi / bazi isimlerin sonunda ')' oldugu icin almadim.
        {
            index=getIndex(token,hashT);
            if(!hashT[index]) // yeni aktor
            {
                actor=newNode(token);
                hashT[index]=actor; // aktor hashe yerlestirilir
            }
            else // aktor hashte var
                actor=hashT[index];
            addEdge(movie,actor); // aktor filmin
            addEdge(actor,movie); // film de aktorun listesine eklenir
        }
    }
}

int bfs(struct node *src,struct node *dest)
{
    int i,notfound=0;
    struct node *p;
    src->visited=1;
    while(src && src!=dest)
    {
        for(i=0;i<src->size;i++)
            if(!((p=src->edge[i])->visited))
            {
                push(p);
                p->visited=src->visited+1;
            }
        src=pop(); // eger bos queue poplanirsa src NULL olur
    }
    if(src) // src NULL degil ise bulunmustur
        return src->visited;
    return 0;
}

int print(struct node *src)
{
    int i;
    while(src->visited!=1)
    {
        printf("%s\n      |\n",src->name);
        i=-1;
        while(src->edge[++i]->visited!=src->visited-1);
        src=src->edge[i];
    }
    printf("%s\n\n",src->name);
}

int main()
{
    int i,k,dist,kevin;
    struct node *act1,*act2; // verilen aktorlerin hashte yerini tutan pointerlar
    struct node **hashT=(struct node**)calloc(sizeof(struct node*),M); // aktorler icin hash table
    char actname[50];

    FILE *fp=fopen("input.txt","r");

    createHashTable(fp,hashT);

    printf("0.Kevin Bacon sayisi bulma\n1.Aktorler arasi uzaklik bulma\nLutfen Secim yapiniz: ");
    scanf("%d",&kevin);
    while ((getchar()) != '\n'); // scanfin almadigi bufferda kalan newline karakterini almak icin
    do
    {
        printf("Aktor ismini giriniz : ");
        fgets(actname,100,stdin);
        actname[strlen(actname)-1]='\0';
        act1=hashT[getIndex(actname,hashT)];
    }while(!act1); // gecerli bir aktor bulana kadar devam et

    if(kevin) // bacon num degilse ikinci aktoru de al
    {
        do
        {
            printf("Diger Aktor ismini giriniz : ");
            fgets(actname,100,stdin);
            actname[strlen(actname)-1]='\0';
            act2=hashT[getIndex(actname,hashT)];
        }while(!act2);
    }
    else
        act2=hashT[getIndex("Bacon, Kevin",hashT)];

    dist=bfs(act2,act1);
    if(dist)
    {
        if(kevin)
            printf("\n%s - %s arasi uzaklik %d\n\n",act1->name,act2->name,dist/2);
        else
            printf("\n%s Kevin Bacon sayisi: %d\n\n",act1->name,dist/2);
        print(act1);
    }
    else
        printf("Baglanti Bulunamadi.\n");

    for(i=0;i<M;i++)
        if(act1=hashT[i]) // hash temizleme
        {
            free(act1->name);
            free(act1);
        }

    struct queue *q=head;
    while(q) // queue temizleme
    {
        q=q->next;
        free(head);
        head=q;
    }

    return 0;
}
