#ifndef _CRAWLER_H_
#define _CRAWLER_H_

#include "webgraph.h"
#include "urlqueue.h"

#define URL_LIST_LEN 5000//url��ĳ��ȣ�Ĭ��Ϊ1024�������Է�1024��url��¼ 
#define SITE_IP "127.0.0.1"
#define SITE_PORT 80

void* do_crawler(void *arg); //�����ȡ�̵߳ĺ�������
int remove_duplicate(int *out_link, int size);

int comp(const void *a,const void *b);

#endif

