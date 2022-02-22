#ifndef WORKER_HASHTABLE_H
#define WORKER_HASHTABLE_H
#include "worker.h"
#include "tree.h"

using namespace std;

struct Avl;   
struct Record;
struct PriorityQueue;

struct Bucket_item{  

	string name;  //disease or country name
	Avl * avl; //root,counter 
	void insert_patient(string & name, int key, Record * rec);
	Bucket_item();
	~Bucket_item();
};



struct Bucket{  

	int bucket_size;
    int index; 
	Bucket_item * bucket_items;
	
	Bucket(int size):bucket_size(size), index(0), bucket_items(new Bucket_item[size]){}  
	~Bucket(){

		delete [] bucket_items;   

	}
	void insert_patient(string & name, int key, Record * rec);
	bool is_bucket_full(){return bucket_size == index; }
};



struct Bucket_node{   

	Bucket_node * next;
	Bucket * bucket_pointer;
	
	Bucket_node(int size):next(NULL), bucket_pointer(new Bucket(size)){}
	~Bucket_node(){

		delete bucket_pointer;

	}
};



struct Bucket_list{  


	Bucket_node * head;
	Bucket_node * tail;
	int bucket_node_counter;
	int bucket_size;
	Bucket_list(int size):head(NULL), tail(NULL), bucket_node_counter(0),bucket_size(size) {}
	~Bucket_list();
	int insert_new_patient(int date_key, string & name, Record * rec);
	bool is_empty(){ return bucket_node_counter == 0;} 
	void calculate_statistics(char buf[]);
	int diseaseFrequency(string & virus_name, int node_key1, int node_key2);
	int diseaseFrequency(string & country_name, string & virus_name, int node_key1, int node_key2);
	char * topk_diseases_in_country(int k, string & disease, int date_key1, int date_key2);
	char * num_patient_admissions(string & disease, int date_key1, int  date_key2);
	char * num_patient_admissions(string & country, string & disease, int date_key1, int  date_key2);
	char * num_patient_discharges(string & disease, int date_key1, int  date_key2);
	char * num_patient_discharges(string & country, string & disease, int date_key1, int  date_key2);

};




struct HashTable{  

	int size;
	int itemscounter;
	Bucket_list ** list_pointer;
	HashTable(int size, int bucket_size = 5);
	~HashTable();
	void insert_new_patient(int date_key, string & name, Record * rec);
	int hash(const char * );
	void calculate_statistics(char buf[]  );
	int diseaseFrequency(string & virus_name, int node_key1, int node_key2);   
	int diseaseFrequency(string & country_name, string & virus_name, int node_key1, int node_key2); 
	char * topk_diseases_in_country(int k , string & country, string & disease, int date_key1, int date_key2);
	char * num_patient_admissions(string & disease, int date_key1, int  date_key2, int country);
	char * num_patient_admissions(string & country, string & disease, int date_key1, int  date_key2);
	char * num_patient_discharges(string & disease, int date_key1, int  date_key2, int country);
	char * num_patient_discharges(string & country, string & disease, int date_key1, int  date_key2);

};

struct sort_table{

	int value;  
	string category;  
};


char * sort_and_print(int, int, int, int, int, int);



#endif