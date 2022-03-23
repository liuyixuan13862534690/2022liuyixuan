#include<stdlib.h>
#include<stdio.h>
#include<string.h>
typedef int datatype;
typedef struct node{
	datatype data;
	struct node*next; 
}seqlist;
#define min(A,B) ((A)>=(B)?B:A)
seqlist* seqlist_create();
void seqlist_insert(seqlist*h,datatype pos);
datatype seqlist_delete(seqlist*h);
void seqlist_print(seqlist*h);
datatype seqlist_lookfor(seqlist*h,int pos);
int main(int argc, const char *argv[])
{
	seqlist*h=seqlist_create();
	seqlist_insert(h,100);
	seqlist_insert(h,200);
	seqlist_insert(h,300);
	seqlist_delete(h);
	seqlist_print(h);
	int p=min(520,530);
	printf("%d\n",p);
	
	return 0;
}
seqlist* seqlist_create(){
	seqlist*h=(seqlist*)malloc(sizeof(seqlist));
	h->data=0;
	h->next=NULL;
	return h;
}
void seqlist_insert(seqlist*h,datatype pos)
{
	seqlist*s=(seqlist*)malloc(sizeof(seqlist));
	seqlist*value=h->next;
	s->next=value;
	s->data=pos;
	h->next=s;
}
datatype seqlist_delete(seqlist*h)
{
	seqlist*value=h->next;
	datatype pos=value->data;
	h->next=value->next;
	free(value);
	value=NULL;
	return pos;
}
void seqlist_print(seqlist*h)
{
	while(h->next!=NULL)
	{
		h=h->next;
		printf("%d\n",h->data);
	}
}
datatype seqlist_lookfor(seqlist*h,int pos)
{
	while(h->next!=NULL)
	{
		if(pos!=0)
		{
			pos--;
			h=h->next;
		}
		if(pos==0)
		{
			return h->data;
		}
	}
}

