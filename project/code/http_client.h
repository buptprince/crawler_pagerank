#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#define HTTP_PORT 80
#define HOST_NAME "127.0.0.1"
#define HEADER_SIZE 1024
#define BUF_SIZE 1024
#define RESPONSE_SIZE 1024 * 1500//����1500k���ڴ����Խ���http�������ݣ�����requestͷ��html���ݣ�

char* http_do_get(int sockfd, char *path);// ��get��������Զ�̷���������ҳ
int http_response_status(char *http_response);// Զ��WEB��������http��Ӧ���룬��404
//http_response_body();//��ȡhttp��Ӧ����Ϣ���ֽ���


#endif

