#ifndef WORKER_H  
#define WORKER_H
#define MAXSIZE 100
#define PERMISSIONS S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include "tree.h"
#include "worker_hashtable.h"
#include <dirent.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <signal.h>


using namespace std;


struct listCountries_node{ 

	char  country[32];
	int pid;
	listCountries_node * next;
	listCountries_node():next(NULL){}

};


struct listCountries_list{ 

	listCountries_node * head;
	listCountries_node * tail;
	int counter;
	listCountries_list():head(NULL),tail(NULL),counter(0){}
	~listCountries_list();

	void insert_node(char [], int );
	char * print_node();
};




struct given_countries_node{

	char country_name[32];
	int key, d, m, y;
	given_countries_node * next;
	given_countries_node():next(NULL){}

};


struct given_countries_list{ 

	int nodes_num;
	given_countries_node * head;
	given_countries_node * tail;
	given_countries_list():head(NULL),tail(NULL), nodes_num(0){}
	~given_countries_list();

	void insert_node(char [], int, int, int, int);
	char * print_statistics();
	char * print_countries();

};





struct Record{  

	string recordID, ENTER_EXIT, patientFirstName, patientLastName, disease_name, in_country;
	int age, date_key, day, month, year, position;

};


struct Arguments{  

	char  fifo_read[MAXSIZE],  fifo_write[MAXSIZE];
	int buffer_size, fd_read, fd_write;
	int position, country_num, file_counter;
	Arguments():position(0), file_counter(0){}
		
};


bool readFile( char * fileName);
void read_file_line(int d, int m, int y, int key, char * line, char file[] );
void init(int x, char ** y );
void input_command();
int tokenize_date(string & x );
int tokenize_date(string & x, Record * rec );
void globalDiseaseStats();
void numCurrentPatients();
char * read_from_fifo(int filedesc);
void receiving_countries_from_fifo();
void access_country(char  country[]);
void read_query_and_write_back(char * fifo_query);
void terminate(int );
#endif