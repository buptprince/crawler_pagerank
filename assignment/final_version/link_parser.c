/*****
** link_parser.c
** - implements the methods declared in link_parser.h
** - 处理http响应字节流，提取网络链接
*/

#include "link_parser.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

int is_rela_path_html(char *link)
{
	char link_head[10], link_tail[10];
	int head_len = strlen("http"), tail_len = strlen("html");

	memcpy(link_head, link, head_len);
	link_head[head_len] = '\0';
	memcpy(link_tail, link + strlen(link) - tail_len, 5);
	if (strcmp(link_head, "www.") != 0 && strcmp(link_head, "http") != 0 && (strcmp(link_tail, ".htm") == 0 || strcmp(link_tail, "html") == 0))
		return 1;
	return 0;
}

//将../../test.html这类链接转化成techqq/test.html这类路径
int get_real_path_link(char *cur_url, char *link)
{
	char tmp_url[512];                                                                                                           
	int upper_level;
	int cnt;
	int pos_arr[10];
	int offset;
	int pos;

	strcpy(tmp_url, cur_url);
	upper_level = 0;
	offset = 0;
	while (strstr(&link[offset], "../") != NULL)
	{
		upper_level++;
		offset = upper_level * 3;
	}
	cnt = 0;
	for (pos = strlen(tmp_url); pos >= 0; pos--)
	{
		if (tmp_url[pos] == '/')
		{
			cnt++;
			pos_arr[cnt] = pos;
		}
	}
	if (cnt > upper_level)
	{
		strcpy(&tmp_url[pos_arr[upper_level+1] + 1], &link[upper_level * 3]);
	}
	else
	{
		return IGNORE_THIS_URL;
	}
	strcpy(link, tmp_url);
	return SUCCESS;
}

int extract_link(char *htmltxt, char **url_list, char *cur_url, char *cur_path)
{
	int pos = 0;
	int state = 0;
	int link_pos = 0;
	char link[LINK_LEN];
	int url_list_size = 0;
	char tmp[LINK_LEN];

	while (htmltxt[pos])
	{
		switch (state)
		{
			case 0:
				while (htmltxt[pos] && htmltxt[pos] != '<')
					pos++;
				if (htmltxt[pos] == '\0')
					break;//to break while
				else if (htmltxt[pos] == '<')
				{
					state = 1;
					pos++;
				}
				break;

			case 1:
				while (htmltxt[pos] && htmltxt[pos] == ' ')
					pos++;
				if (htmltxt[pos] == '\0')
					break;
				else if (htmltxt[pos] == 'a' || htmltxt[pos] == 'A')
					state = 2;
				else
					state = 0;
				pos++;
				break;

			case 2:
				while (htmltxt[pos] && htmltxt[pos] != '>' && htmltxt[pos] != 'h' && htmltxt[pos] != 'H')
					pos++;
				if (htmltxt[pos] == '\0')
				{
					printf("break\n");
					break;
				}
				else if (htmltxt[pos] == '>')
					state = 0;
				else if (htmltxt[pos] == 'h' || htmltxt[pos] == 'H')
					state = 3;
				pos++;
				break;

			case 3:
				if (htmltxt[pos] == 'r' || htmltxt[pos] == 'R')
					state = 4;
				else if (htmltxt[pos] == '>')
					state = 0;
				else
					state = 2;
				pos++;
				break;

			case 4:
				if (htmltxt[pos] == 'e' || htmltxt[pos] == 'E')
					state = 5;
				else if (htmltxt[pos] == '>')
					state = 0;
				else
					state = 2;
				pos++;
				break;

			case 5:
				if (htmltxt[pos] == 'f' || htmltxt[pos] == 'F')
					state = 6;
				else if (htmltxt[pos] == '>')
					state = 0;
				else
					state = 2;
				pos++;
				break;

			case 6:
				while (htmltxt[pos] && htmltxt[pos] == ' ')
					pos++;
				if (htmltxt[pos] == '\0')
					break;
				else if (htmltxt[pos] == '=')
					state = 7;
				else if (htmltxt[pos] == '>')
					state = 0;
				else
					state = 2;
				pos++;
				break;

			case 7:
				while (htmltxt[pos] && htmltxt[pos] == ' ')
					pos++;
				if (htmltxt[pos] == '\0')
					break;
				else if (htmltxt[pos] == '\'' || htmltxt[pos] == '\"')
					state = 8;
				else if (htmltxt[pos] == '>')
					state = 0;
				else 
					state = 2;
				pos++;
				break;

			case 8:
				memset(link, 0, sizeof (char) * LINK_LEN);
				link_pos = 0;
				while (htmltxt[pos] && htmltxt[pos] != '\'' && htmltxt[pos] != '\"')
				{
					link[link_pos] = htmltxt[pos];
					link_pos++;
					if (link_pos > LINK_LEN)
					{
						printf("extract link fail::link_len > 512!!!!!!!!\n");
						pos++;
						state = 0;
						break;
					}
					pos++;
				}
				link[link_pos] = '\0';
				if (is_rela_path_html(link))
				{
					if (link[0] == '.' && link[1] == '.')//处理../../test.html这类连接
					{
						if (get_real_path_link(cur_url, link) == SUCCESS)
						{
							strcpy(url_list[url_list_size], link);
							url_list_size++;
						}
					}
					else
					{
						strcpy(tmp, cur_path);
						strcat(tmp, link);
						strcpy(url_list[url_list_size], tmp);
						url_list_size++;
					}
				}
				state = 0;
				pos++;
				break;
		}
	}
	//regfree(&reg);
	//free(link);
	return url_list_size;
}
