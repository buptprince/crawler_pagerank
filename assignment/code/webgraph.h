#ifndef _WEBGRAPH_H_
#define _WEBGRAPH_H_


init_webg();// ��ʼ��web graph
insert_vertex(char * url);// ����һ������
has_vertex(char *url);// �жϸ��������Ƿ���graph��
insert_edge();// ����һ���ߵ�graph��
vertex_size();// graph�Ķ�����
calc_ind_cdf();// �������ж������ȵ��ۻ��ֲ�
calc_pagerank();// �������ж����pagerank
find_ind(char *url);// ���Ҹ�����������
gen_graphviz();// ��������ͼ���˵�graphviz�ű�
destroy_webg();// ����web graph


#endif

