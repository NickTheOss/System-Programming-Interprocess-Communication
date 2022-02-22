#ifndef DISEASEAGGREGATOR_H   //dilwseis kai include twn vivliothikwn pou xreiazontai
#define DISEASEAGGREGATOR_H
#define MAXSIZE 100
#define ec_neg1(x) if((x) == -1) {perror("Error with fork");  exit(errno);}
#define PERMISSIONS S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

#include <fcntl.h>
#include <sys/wait.h>  //wait
#include <sys/types.h> //catalogs
#include <sys/stat.h>  //fifos
#include <errno.h>    //error codes
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <fstream>
#include <dirent.h> 
#include <poll.h>
#include <signal.h>
#include <sstream>

using namespace std;



struct worker_node{

	string country;
	worker_node * next;
	worker_node(string country): country(country), next(NULL){}

};



struct worker_list{  

	worker_node * head;
	worker_node * tail;
	int worker_node_counter;

	worker_list(): head(NULL), tail(NULL), worker_node_counter(0){}
	~worker_list();
	void insert_node(const char []);
	bool search_node(string );
	void print_exit(int, int, int);
	bool list_empty(){ return worker_node_counter == 0; }
	char * get_countries();
};	


struct worker_info{   

	worker_list countries;
	pid_t pid;
    int  fifo_fd_read, fifo_fd_write;
	char read_fifo[MAXSIZE], write_fifo[MAXSIZE];
	bool not_sent_yet;

};



struct Arguments{  

		int workers_num, fifo_buffer_size;
		string input_dir;
		
};


void init(int x,char ** y );
void get_countries(const char * , worker_list & wl);
void disperse_countries(worker_list &, worker_info *);
void make_and_open_fifos(worker_info * workers);
char * read_from_fifo(int filedesc);
void write_to_fifo(int filedesc, char * input );
void create_workers(int buffer_size, worker_info * workers);
void open_fifo_for_write(worker_info * workers);
void send_countries_to_workers(worker_info * workers);
void command_workers(worker_info * workers, char * query);
void read_from_all_workers( worker_info * workers);
void read_from_all_workers( worker_info * workers, string query);
void set_worker_flags(worker_info * workers);
bool worker_sent_remain(worker_info * workers); 
void input_command(worker_info * workers, worker_list & wl);
int tokenize_date(string & input_date);
int search_country(string & country, worker_info * w_info);
#endif