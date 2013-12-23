/*****
** http_client.c
** - implements the methods declared in http_client.h
** - ��װ����httpЭ��
*/

#include "http_client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ��get��������Զ�̷���������ҳ
char* http_do_get(int sockfd, char *path)
{
	char *tail_of_header =
		 "User-Agent: Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.11 (KHTML, like Gecko) Ubuntu/11.10 Chromium/17.0.963.79 Chrome/17.0.963.79 Saf      ari/535.11\r\n"
		 "Content-Type: application/x-www-form-urlencoded\r\n"
		 "\r\n"; 
	char http_header[HEADER_SIZE];
	char recv_buf[BUF_SIZE];
	char *http_response = NULL;
	int offset;
	int tmp;

	//printf("GET %s HTTP/1.1\r\nHost: %s\r\n%s", path, HOST_NAME, tail_of_header);
	sprintf(http_header, "GET %s HTTP/1.1\r\nHost: %s\r\n%s", path, HOST_NAME, tail_of_header);
	send(sockfd, http_header, HEADER_SIZE, 0);
	http_response = (char *)malloc(sizeof (char) * RESPONSE_SIZE);//����1500k�ڴ����Խ���һ����ҳ
	if (http_response == NULL)
	{
		printf("================================fail to malloc 1500k to recv the page!\n");
		return NULL;
	}
	memset(http_response, 0, sizeof (char) * RESPONSE_SIZE);
	offset = 0;
	while (1)
	{
		if ((tmp = recv(sockfd, recv_buf, BUF_SIZE, 0)) == 0)
			break;
		else if (tmp == -1)
		{
			free(http_response);
			return NULL;
		}
		sprintf(http_response + offset, "%s", recv_buf);
		offset += tmp;
		if (offset > 1000000)
		{
			printf("large page!!!!!!!!!!!!!!!: %s\n", path);
			free(http_response);
			printf("larger than 1000k!\n");
			return NULL;
		}
	}

	http_response[offset] = '\0';//Ϊ����ַ������������Ƿ��ص�http���ݣ���ӡ�\0����������strstr������ʱ������о�ͷ���ַ�����������Ҫ�н�β
	return http_response;
}

// Զ��WEB��������http��Ӧ���룬��404
int http_response_status(char *http_response)
{
	int status;
	char buf[20];//get the string:"HTTP/1.1"

	sscanf(http_response, "%s%d", buf, &status);
	return status;
}
//http_response_body();//��ȡhttp��Ӧ����Ϣ���ֽ���
