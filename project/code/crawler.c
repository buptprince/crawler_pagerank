/*****
** crawler.c
** - implements the methods declared in crawler.h
** -��ȡ�̵߳ĺ�������
*/
#include "crawler.h"
#include "http_client.h"
#include "link_parser.h"
#include "network.h"
#include "tpool.h"
#include <stdlib.h>

void* do_crawler(void *arg) //�����ȡ�̵߳ĺ�������
{
	struct para_for_crawler *para = (struct para_for_crawler *)arg;
	struct web_graph *webg = para->webg;
	urlq_t *queue = para->queue;
	tpool_t *tpool = para->tpool;

	int client_fd = -1;
	char *http_response = NULL;
	char **url_list = NULL;
	int url_list_size = 0;
	int i = 0, j;
	int num = 0, cur_num = 0;
	int *out_link = NULL;//��ŵ�ǰ���url�ı߼���������������ŵ���Ŀ�궥���ţ�
	char *cur_url = NULL;
	int try_times = 0;//��ȡ��ҳʧ��֮�󣬳���3�Σ����3�ζ����ǻ�ȡ���ˣ���˵�����У���������
	char *cur_path = NULL;

	url_list = (char **)malloc(sizeof (char *) * URL_LIST_LEN);
	for (i = 0; i < URL_LIST_LEN; i++)
		url_list[i] = (char *)malloc(sizeof (char) * LINK_LEN);//���СΪ5000*512

	cur_url = (char *)malloc(sizeof (char ) * LINK_LEN);
	cur_path = (char *)malloc(sizeof (char ) * LINK_LEN);

	while (1)
	{
		/*����̳߳�û�б�������û������Ҫִ�У�������߳�*/
		pthread_mutex_lock(&tpool->lock);
		while (queue->size == 0 && tpool->shutdown == 0)
		{
			tpool->at_work--;
			pthread_cond_wait(&tpool->cond, &tpool->lock);//����ǰ�����,���Ѻ�����¼���
			tpool->at_work++;
		}
		if (tpool->shutdown)
		{
			tpool->at_work--;
			pthread_mutex_unlock(&tpool->lock);
			pthread_exit(NULL);
		}
		pthread_mutex_unlock(&tpool->lock);

		/*������Ҫ��������Ϊ��Ҫ�Զ��н��иĶ�*/
		pthread_mutex_lock(&tpool->lock);
		cur_num = queue_pop(queue);
		pthread_mutex_unlock(&tpool->lock);
		/*ȡ�ö����е�url�����*/

		/*��Ϊ���Ե�ʱ�򣬷���cur_num������all_url_list_size�ķ�Χ�����ԼӸ�����жϣ��������bug*/
		if (cur_num > webg->all_url_list_size)
		{
			printf("-----------------------cur_num over url list size!---------------------------------\n");
			continue;
		}
		if (cur_num == -1)
		{
			printf("--------------------empty_queue!-------------------\n");
			continue;
		}
		/*��ͼ�и��ݱ�Ż��url�ľ�������*/
		strcpy(cur_url, webg->all_url_list[cur_num]);
		if (strlen(cur_url) >= LINK_LEN)
		{
			printf("fail:::too long link!\n len: %d\n", strlen(cur_url));
			printf("long url: %s\n", cur_url);
			continue;
		}

		for (i = strlen(cur_url) - 1; i >= 0; i--)
			if (cur_url[i] == '/')
				break;
		for (j = 0; j <= i; j++)
			cur_path[j] = cur_url[j];
		cur_path[i+1] = '\0';

		/*�ⲿ�ֹ�������Ҫ���������ٽ���Դ*/
		
		/*����url��ȡ��ҳ����*/
		//һ����3�γ��Ի���
		try_times = 3;
		http_response = NULL;
		while (try_times != 0 && http_response == NULL)
		{
			client_fd = open_tcp(SITE_IP, SITE_PORT);
			if (client_fd < 0)
			{
				printf("fail to open tcp!\n");
				try_times--;
				continue;
			}
			http_response = http_do_get(client_fd, cur_url);
			try_times--;
			close(client_fd);
		}
		/*client_fd = open_tcp(SITE_IP, SITE_PORT);
		if (client_fd < 0)
		{
			printf("fail to open tcp!\n");
			continue;
		}
		http_response = http_do_get(client_fd, cur_url);
		close(client_fd);*/

		//3�γ��Ի��ỹ��ΪNULL���Ǿ�˵�������ҳӦ����ץ�����ˣ���������
		if (http_response == NULL || strlen(http_response) < 10)//���ճ�ʱ��, �����ǽ��ջ����ĳ���С��10�����������ջ�������Ϊ0
		{
			printf("fail to fetch this page: %s\n", cur_url);
			continue;
		}
		
		if (http_response_status(http_response) == 200)
		{
			webg->existed_page[cur_num] = 1;
			pthread_mutex_lock(&tpool->lock);
			webg->existed_page_size++;
			pthread_mutex_unlock(&tpool->lock);

			url_list_size = extract_link(http_response, url_list, cur_url, cur_path);//���url_list�����С
			if (url_list_size > 4000)
			{
				printf("url_list_size > 4000!\nurl_list_size: %d\n", url_list_size);
				free(http_response);
				continue;
			}
			//��url_list�е�url����㼯
			out_link = (int *)malloc(sizeof (int ) * url_list_size);
			if (out_link == NULL)
			{
				printf("malloc out_link fail!\n");
				continue;
			}

			/*����漰���Զ��к�ͼ���޸ģ�������Ҫ����*/
			for (i = 0; i < url_list_size; i++)
			{
				if (strlen(url_list[i]) >= LINK_LEN)
				{
					printf("-----------fail::too long url!\n url_len: %d\nurl: %s\n-----------\n", strlen(url_list[i]), url_list[i]);
					i++;
					continue;
				}
				pthread_mutex_lock(&tpool->lock);
				if ((num = has_vertex(webg, url_list[i])) == NOT_IN_SET)//��ʾ���url���ڵ㼯�У����ﻹ��Ҫ��ӽ����url�����
				{
					num = insert_vertex(webg, url_list[i]);
					if (num == -1)
						printf("fail to insert!\n");
					if (num > 400000)//��ֹ��������嵽��������ȥ
					{
						printf("fail: wrong number to be inserted into the graph\n");
						i++;
						pthread_mutex_unlock(&tpool->lock);
						continue;
					}
					queue_push(queue, num);
					if (tpool->at_work < tpool->max_thr_num)//�����ر���Ҫ������ʲôʱ����signal�ģ�����̶߳��ڹ����Ļ������signal�ͻᶪʧ
						pthread_cond_signal(&tpool->cond);
				}
				pthread_mutex_unlock(&tpool->lock);
				out_link[i] = num;//��Ŀ�궥��ı�Ŵ�ŵ����url�ı߼�����ȥ
			}
		
			/*��Ϊÿ���߳���Ҫ�����url��ͬ����cur_url����ͬ�����ԶԱ߼��Ĳ����Ͳ�ͬ����������Ա߼��Ĳ�������Ҫ����*/
			num = remove_duplicate(out_link, url_list_size);//����ȥ�غ�Ķ������ 
			webg->edge_set[cur_num] = (int *)malloc(sizeof (int ) * (num + 1));//����ǰurl�ı߼�����ռ�
			if (webg->edge_set[cur_num] == NULL)
			{
				printf("-----------------------------fail to malloc space for edge set for %d  url\n", cur_num);
				free(out_link);
				continue;
			}
			webg->edge_set[cur_num][0] = num;
			for (i = 0; i < num; i++)
			{
				pthread_mutex_lock(&tpool->lock);
				webg->ind[out_link[i]]++;//ͳ�����
				webg->edge_set_size++;
				pthread_mutex_unlock(&tpool->lock);

				insert_edge(webg, cur_num, out_link[i], i+1);//i+1����Ϊ����߼��������棬��0���Ѿ����˳�����
			}

			//�ͷ�edgeռ�õ��ڴ�
			free(out_link);
			out_link = NULL;
		}
		free(http_response);//����ǵý�http_response�ͷŵ�������ܺ��ڴ��
		http_response = NULL;
	}

	//�ͷ�url_list��ռ�õ��ڴ�
	for (i = 0; i < URL_LIST_LEN; i++)
		free(url_list[i]);
	free(url_list);
	url_list = NULL;
	free(cur_url);
}

int remove_duplicate(int *out_link, int size)
{
	int cur, cur_pos, i;
	
	qsort(out_link, size, sizeof (int ), comp);
	cur = -1;//��Ϊ��ı�Ų����ܴ���-1
	cur_pos = -1;
	for (i = 0; i < size; i++)
	{
		if (out_link[i] != cur)
		{
			cur_pos++;
			cur = out_link[i];
			out_link[cur_pos] = cur;
		}
	}
	return cur_pos + 1;
}

int comp(const void *a,const void *b)
{
	return *(int *)a-*(int *)b;
}
