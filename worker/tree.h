#ifndef TREE_H
#define TREE_H
#include "worker.h"

using namespace std;

struct Record;

struct List_node{  

	Record * record;
	List_node * next;
	List_node(Record *);
	~List_node();

};


struct Data_list{  

	List_node * head;
	List_node * tail;
	int counter;
	Data_list():head(NULL),tail(NULL),counter(0){}
	~Data_list();

	List_node * insert_node(Record * );
	bool entry_type_check();
	char * search_patient_record(string recordID);
};



struct Tree_node{   
	int node_key;   //date_key
	Record * record;  
	int height;       
	Tree_node * left;   
	Tree_node * right; 

	Tree_node(int key, Record *rec):node_key(key), record(rec), height(1), left(NULL), right(NULL){}

};

struct Avl{   

	Tree_node * root;   
	int tree_node_counter, tree_node_counterA, tree_node_counterB, tree_node_counterC, tree_node_counterD; 
	int A, B, C, D; 
	
	Avl(): root(NULL), tree_node_counter(0),tree_node_counterA(0),tree_node_counterB(0),tree_node_counterC(0),tree_node_counterD(0), A(0), B(0), C(0), D(0){}
	~Avl(){
		free_post_traversal(root); 

	}
	void free_post_traversal(Tree_node * root);
	void set_ABCD_NULL(Avl * avl);

	void insert_to_avl(int key, Record * rec); 
	Tree_node * insert(Tree_node * node, int key, Record * rec);
	Tree_node * rightRotate(Tree_node * node_up);  
	Tree_node * leftRotate(Tree_node * node_up);  

	int max(int h1, int h2){ return (h1 > h2)? h1 : h2; }   
	int height(Tree_node * node){  return (node == NULL) ? 0 : node->height; }  
	int getBalance(Tree_node * node) { return (node == NULL) ? 0 : height(node->left) - height(node->right); }  
    int get_virus_frequency(string & virus_name, int date_key1, int date_key2 ){ return get_virus_frequency_rec ( root, virus_name, date_key1, date_key2 ); }
    int get_virus_frequency_rec (Tree_node * root,string & virus_name, int date_key1, int date_key2 );
    int get_country_diseasefreq( string & disease, int date_key1, int date_key2 ){ return get_country_diseasefreq_rec( root, disease, date_key1, date_key2 ); }
    int get_country_diseasefreq_rec( Tree_node * root,string & disease, int date_key1, int date_key2);
    int get_virus_frequency_ENTER (string & virus_name, int date_key1, int date_key2 ){ return get_virus_frequency_rec_ENTER ( root, virus_name, date_key1, date_key2 ); }
    int get_virus_frequency_rec_ENTER (Tree_node * root,string & virus_name, int date_key1, int date_key2 );
  	int get_virus_frequency_EXIT (string & disease, int date_key1, int date_key2 ){ return get_virus_frequency_rec_EXIT ( root, disease, date_key1, date_key2 ); }
    int get_virus_frequency_rec_EXIT (Tree_node * root,string & disease, int date_key1, int date_key2 );
   
};

#endif