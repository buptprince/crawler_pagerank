#ifndef _WEBGRAPH_H_
#define _WEBGRAPH_H_

#define GRAPH_SIZE 400000
#define HASH_TABLE_SIZE 400000  //(the 13th prime int)
#define LINK_LEN 512
#define INIT_FAIL 0
#define INIT_SUCCEED 1
#define NOT_IN_SET -1

struct node
{
	char *url;
	int number;//url��ţ���1��ʼ
	struct node *next_ptr;
};//hash_table�Ľڵ�

struct web_graph
{
	struct node *vertex_set;
	char **all_url_list;
	int **edge_set;
	int all_url_list_size;
	int *ind;//���һά��������ͳ�ƶ�������
	int *existed_page;//���һά��������ͳ��ҳ���Ƿ���ڣ�������200�ͼ�Ϊ1��
	int existed_page_size;//ͳ��һ���ж��ٸ�page�Ǵ��ڵ�
	int edge_set_size;
};

/*************************************
�����Ƕ�web_grah��Ҫ��5�����ݽṹ��˵����

1��vertex_set���㼯��:
��һ��hash_table����Ҫ����struct nodeȥ�������hash_table
node�����url���ڴ洢������ݣ�������url
node�����number���ڴ洢��ı�ţ���1��ʼ��ţ�
���hash_table�������ڿ����ж�һ�����Ƿ��ڵ㼯��

2��all_url_list(�洢����url��һ�������������Ϳ�����֪����ŵ�����¿��ٵõ�url�ľ���������
**************************
|1|url1���ݣ���index.html|
|2|url2���ݣ���index.html|
.......
**************************
��Ҫע����Ǳ�Ŵ�1��ʼ��

3��edge_set���߼�������һ��ϡ�����
�߼���һ����ά���飬��һ�д����һ��url�ı߼���ȥ�غ�ģ�����һ�����������ĳ���n����n�������������url��ȥ��Ŀ��url�ı��
| ����n | n�����֣���ȥ��n��url��

4:all_url_list_size��¼�����Ѿ������ٸ�url������ˣ�ע����1��ʼ��ţ�

5:ind��һ��һά���飬����ͳ�Ƹ���url�����
*************************************/

int init_webg(struct web_graph *webg);// ��ʼ��web graph
int insert_vertex(struct web_graph *webg, char * url);// ����һ������,���ɹ�����-1���ɹ����ز���Ķ���ı��
int has_vertex(struct web_graph *webg, char *url);// �жϸ��������Ƿ���graph�У����ҵ����ض����ţ���û�ҵ�������-1
void insert_edge(struct web_graph *webg, int src_num, int dst_num, int pos);// ����һ���ߵ�graph��
int vertex_size(struct web_graph *webg);// graph�Ķ�����
//calc_ind_cdf();// �������ж������ȵ��ۻ��ֲ�
//calc_pagerank();// �������ж����pagerank
//find_ind(char *url);// ���Ҹ�����������
//gen_graphviz();// ��������ͼ���˵�graphviz�ű�
void destroy_webg(struct web_graph *webg);// ����web graph
void destroy_list(struct node *head_ptr);//�������ڴ���ͷ�
void print_webg_to_file(struct web_graph *webg);
void output_result_file(int argc, char *argv[]);
#endif

