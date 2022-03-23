#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef int datatype;

typedef struct seq{
datatype buf[32];
int pos;

}sequeue;
sequeue* sequeue_create();
void sequeue_insert(sequeue*s,int data);
void sequeue_print(sequeue*s);
sequeue* sequeue_delete(sequeue*s);
void sequeue_change(sequeue*s,int value,int data);
int main(int argc, const char *argv[])
{
	sequeue*s=sequeue_create();
	sequeue_insert(s,10);
	sequeue_insert(s,20);
	sequeue_insert(s,30);
	sequeue_delete(s);
	sequeue_change(s,1,200);
	sequeue_print(s);
	return 0;
}
sequeue* sequeue_create()
{
	sequeue*s=(sequeue*)malloc(sizeof(sequeue));
	s->pos=-1;
	return s;
}
void sequeue_insert(sequeue*s,int data)
{
	s->pos++;
	s->buf[s->pos]=data;
}
void sequeue_print(sequeue*s)
{
	int i=0;
	for(i=0;i<=s->pos;i++)
	{
		printf("%d\n",s->buf[i]);
	}
}
sequeue* sequeue_delete(sequeue*s)
{
	datatype value;
	value=s->buf[s->pos];
	s->pos--;
	return s;
}
void sequeue_change(sequeue*s,int value,int data)
{
	s->buf[value]=data;
}
