/*****
** main.c
** - ���̣߳�ά���̳߳أ��̼߳�ͨ�ŵ�
*/
#include <stdio.h>
#include <pthread.h>

#include "crawler.h"
#include "http_client.h"
#include "urlqueue.h"
#include "webgraph.h"
#include "link_parser.h"

#define MAX_THREADS     1024

typedef struct _threadpool_st {
    pthread_t threadinfo;	//pointer to threads
    pthread_mutex_t waitlock; //�ȴ�URL��������
    int idle; //��url����Ϊ��ʱ��λΪ1������Ϊ0
}threadpool_item;

threadpool_item tpool[MAX_THREADS];
int num_threads;	//number of threads

int main()
{
    
}


