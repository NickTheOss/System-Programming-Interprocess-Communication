#include "worker.h"


Arguments arguments;   
Data_list database;  
given_countries_list gl;  
listCountries_list lc;
HashTable * htcountries, * htdiseases, * disease_statistics[8]; 


given_countries_list::~given_countries_list(){   

	given_countries_node * current;
	while(head){

		current = head;
		head = head->next;
		delete current;

	}
}



listCountries_list::~listCountries_list(){  

	listCountries_node * current;
	while(head){

		current = head;
		head = head->next;
		delete current;

	}
}



void listCountries_list::insert_node(char * country,int pid ){ 

	listCountries_node * node = new listCountries_node;
	char * tok;
	tok = strtok(country, "/");
	tok = strtok(NULL, "/");
	tok = strtok(NULL, "$");

	if(counter == 0 ){ 

		head = node;
		tail = node;
		counter++;
		strcpy(node->country, tok);
	 	node->pid = pid;
	}

	else{

		tail->next = node; 
		tail = node;
		counter++;
		strcpy(node->country, tok);
	 	node->pid = pid;
	}
}

char * listCountries_list::print_node(){ 

	listCountries_node * current = head;
	char * buffer = new char[counter * 32 + 6 * 32];  
	char pid[5];
	while(current){

		if(current == head){
			strcpy(buffer, current->country);
			sprintf(pid, "%d", current->pid);
			strcat(buffer," ");
			strcat(buffer, pid);
			strcat(buffer,"\n");
			current = current->next;
			continue;
		}
		strcat(buffer, current->country);
		sprintf(pid, "%d", current->pid);
		strcat(buffer," ");
		strcat(buffer, pid);
		strcat(buffer,"\n");

		current = current->next;
	}
	return buffer;

}




void given_countries_list::insert_node(char * country,int key,int day, int month, int year ){ 

	given_countries_node * node = new given_countries_node;  
	if(nodes_num == 0 ){ 

		head = node;
		tail = node;
		nodes_num++;
		strcpy(node->country_name, country);
		node->key = key;
		node->d = day;
		node->m = month;
		node->y = year;
	}

	else{

		tail->next = node; 
		tail = node;
		nodes_num++;
		strcpy(node->country_name, country);
		node->key = key;
		node->d = day;
		node->m = month;
		node->y = year;
	}
}


void init(int c, char ** v){ 

	int i;
	strcpy(arguments.fifo_read, v[1]);  
	strcpy(arguments.fifo_write, v[2]);
	arguments.buffer_size = atoi(v[3]); 
	
	
	htdiseases = new HashTable(10, 80); 

	for ( i = 0; i < 8; ++i)  
	{
		disease_statistics[i] = new HashTable(10,80);
	}

}





bool readFile(char * fileName){  


	ifstream input(fileName);    
	
	if(!input) {               
		cout << "File Not Found"<<endl;  
		return false;
	}
	char buffer[MAXSIZE];
	int day, month, year, date_key;
	char * tok = strtok(fileName, "/");
	tok = strtok(NULL, "/");
	tok = strtok(NULL, "/");
	strcpy(buffer,tok);  
    tok = strtok(NULL,"-");
	day = atoi(tok);
	tok = strtok(NULL,"-");
	month = atoi(tok);
	tok = strtok(NULL," \n");
	year = atoi(tok);
	date_key = (year-1900) * 365 + 30 * month + day;
	gl.insert_node(buffer,date_key, day, month, year );
	char line[MAXSIZE];          

	while((input.peek()!=EOF)) {   

		input.getline(line, MAXSIZE);
		read_file_line( day, month, year, date_key, line , buffer);    

    }	

    input.close(); 

	return true;  	
}



void read_file_line(int day, int month, int year, int key, char * line, char country[]){

	List_node * datalist_node;            
	Record * rec = new Record;         
	rec->in_country = string(country);
	char * token = strtok(line," \n");
	rec->recordID = token;  		 
	token = strtok(NULL," \n"); 
	rec->ENTER_EXIT = token;
	token = strtok(NULL," \n");
	rec->patientFirstName = token;    
	token = strtok(NULL," \n");
	rec->patientLastName = token;     
	token = strtok(NULL," \n");    
	rec->disease_name = token;        
	token = strtok(NULL," \n");
	rec->age = atoi(token);    
	rec->date_key = key;
	rec->day = day;
	rec->month = month;
	rec->year = year;
	

	datalist_node = database.insert_node(rec);  

	
	htcountries->insert_new_patient(key, rec->in_country, rec); 
	htdiseases->insert_new_patient(key, rec->disease_name, rec);    
	disease_statistics[arguments.position]->insert_new_patient(key, rec->disease_name, rec); 
}	




char * given_countries_list::print_statistics(){

	given_countries_node * node = head;
	
	
	char * buffer = new char[arguments.file_counter*10*160];
	memset(buffer,0,arguments.file_counter*10*160);
	int i = 0;
	char d[3],m[3],y[5];
    while(node){
    	sprintf(d,"%d", node->d);
    	strcat(buffer,d);
    	strcat(buffer,"-");
    	sprintf(m,"%d", node->m);
    	strcat(buffer,m);
    	strcat(buffer,"-");
    	sprintf(y,"%d", node->y);
    	strcat(buffer,y);
    	strcat(buffer,"\n");
    	strcat(buffer,node->country_name);
    	strcat(buffer,"\n");
 
    	disease_statistics[i]->calculate_statistics(buffer); 
    	
    	node = node->next;
    	i++;
    }

    return buffer; 
}


char * read_from_fifo(int filedesc){  

    char buffer[10];
    memset(buffer,0,10);
    int fifo_buffer_size = arguments.buffer_size;  
    read(filedesc, buffer, 9); 
    char * tok = strtok(buffer, "@");  
    int length = atoi(tok); 
   
    char * input_read = new char[length + 1];
    char * p = input_read; 
    int bytes_read = 0, bytes_read_til_now = 0;
    fifo_buffer_size = length < fifo_buffer_size ? length : fifo_buffer_size; 

    while(bytes_read_til_now < length){

        p += bytes_read; 
        bytes_read = read(filedesc, p, fifo_buffer_size);
     	if (bytes_read == -1 ) break; 
        if(bytes_read >= 0)
        	bytes_read_til_now += bytes_read;
        else
        	bytes_read = 0;

        if((bytes_read_til_now + fifo_buffer_size) > length)
        	fifo_buffer_size = length - bytes_read_til_now; 
    }

    input_read[length] = '\0';  
    return input_read; 

}



void write_to_fifo(int filedesc, char * input ){

	int length = strlen(input); 
	char buffer[10];
	memset(buffer,0,10);
	sprintf(buffer, "%d@", length);
	
	write(filedesc, buffer, 9);      
	write(filedesc, input, length);

}



void access_country(char * country){ 
	
	DIR * d;
	char buffer[257];
	struct dirent * direntp;
	if((d = opendir(country)) == NULL){
		 perror("Error opening country folder\n");
		 exit(1);
	}

	while ((direntp = readdir(d)) != NULL){

      		if( strcmp(direntp->d_name, ".") != 0  && strcmp(direntp->d_name, "..") != 0 ){
        		sprintf(buffer, "%s/%s", country, direntp->d_name);
        		arguments.file_counter++;
  
        		if(!readFile(buffer)){   
    
					cout << "There is a problem with input file\n" << endl; 
					delete htcountries;          
					delete htdiseases;
					for (int i = 0; i < 8; ++i)
					{
						delete disease_statistics[i];
					}
					exit(1);          

				}

				arguments.position++; 
      		}
    }
    closedir(d);   
}





void receiving_countries_from_fifo(){

	char  * buffer; 

	struct stat stat_temp;  
	if(stat(arguments.fifo_read, &stat_temp) == -1){ 
		if (mkfifo(arguments.fifo_read,PERMISSIONS) < 0 ){              
            perror("Error creating the named read pipe");
            exit(1);
        }		
	}

	
	arguments.fd_read = open(arguments.fifo_read, O_RDONLY ); 
	arguments.fd_write = open(arguments.fifo_write, O_WRONLY );

	if((buffer = read_from_fifo(arguments.fd_read)) == NULL ){ 
		perror("Worker: Error reading from fifo");
		exit(1);
	}

	char  * fifo_statistics;
	char  buffer2[MAXSIZE];
	sprintf(buffer2,"%s",buffer);
	
	int counter = 0;
	char * tok = strtok(buffer,"$");
	while(tok){

		tok = strtok(NULL,"$");
		counter++;            

	}
	delete [] buffer;

	htcountries = new HashTable(counter, 80);  
	arguments.country_num = counter;

	char ** countries = new char * [counter]; 

	for (int i = 0; i < counter; i++)  
	{
		
		countries[i] = new char[30]; 
	}		
	
	
	tok = strtok(buffer2, "$");
	strcpy(countries[0], tok);
	int i = 1;

	while(tok){
		tok = strtok(NULL, "$");
		if(!tok) break;
		strcpy(countries[i], tok);
		i++;
	}


	
	for (int i = 0; i < counter; ++i) 
	{	
		access_country(countries[i]);

	}


	fifo_statistics = gl.print_statistics(); 

    write_to_fifo(arguments.fd_write, fifo_statistics); 
	delete [] fifo_statistics;

	int pid = getpid();

	for (int i = 0; i < counter; ++i) 
	{
		lc.insert_node(countries[i], pid);
	}

	for (int i = 0; i < counter; ++i) 
	{
		delete [] countries[i];
	}

	delete [] countries;

}




void read_query_and_write_back(char * buf){

	char * tok;
	char * country_return;
	char * topK;
	char * search;
	char * admission;
	char * discharge;
	char d_frequency[5];
	string virus, country, recordID;
	int key1, key2, sum, k;


	if (strcmp(buf,"/listCountries") == 0 ){ 

		country_return = lc.print_node();
		write_to_fifo(arguments.fd_write, country_return);
		delete [] country_return;
		return;

	}
	tok = strtok(buf,"$");  //case /diseaseFrequency
	if (strcmp(tok,"/diseaseFrequency") == 0){
		tok = strtok(NULL,"$");
		virus = string(tok);
		tok = strtok(NULL,"$");
		key1 = atoi(tok);
		tok = strtok(NULL,"$");
		key2 = atoi(tok);
		tok = strtok(NULL,"$");
		if(tok == NULL){  

			sum = htdiseases->diseaseFrequency( virus, key1, key2 );
			sprintf(d_frequency, "%d\n", sum);
			
			write_to_fifo(arguments.fd_write, d_frequency);
		}
		else 
		{
			
			country = string(tok);
			sum = htcountries->diseaseFrequency(country, virus, key1, key2);
			sprintf(d_frequency, "%d", sum);
			write_to_fifo(arguments.fd_write, d_frequency);
		}

	}

	else if(strcmp(tok,"/topk-AgeRanges") == 0){  //case /topk-AgeRanges

		tok = strtok(NULL,"$");
		k = atoi(tok);
		tok = strtok(NULL,"$");
		country = string(tok);
		tok = strtok(NULL,"$");
		virus = string(tok);
		tok = strtok(NULL,"$");
		key1 = atoi(tok);
		tok = strtok(NULL,"$");
		key2 = atoi(tok);
		topK = htcountries->topk_diseases_in_country(k, country, virus, key1, key2 );
		write_to_fifo(arguments.fd_write, topK);
		delete [] topK;

	}

	else if(strcmp(tok,"/searchPatientRecord") == 0){  // case /searchPatientRecord

		tok = strtok(NULL, "$");
		recordID = string(tok);
		search = database.search_patient_record(recordID);
		write_to_fifo(arguments.fd_write, search);
		delete [] search;
	}

	else if(strcmp(tok,"/numPatientAdmissions") == 0){  //case /numPatientAdmissions

		tok = strtok(NULL, "$");
		virus = string(tok);
		tok = strtok(NULL,"$");
		key1 = atoi(tok);
		tok = strtok(NULL,"$");
		key2 = atoi(tok);
		tok = strtok(NULL,"$");
		if(tok == NULL){ 

			admission = htcountries->num_patient_admissions(virus, key1, key2, arguments.country_num);
			write_to_fifo(arguments.fd_write, admission);
			delete [] admission;

		}	

		else 
		{

			country = string(tok);
			admission = htcountries->num_patient_admissions(country, virus, key1, key2);
			write_to_fifo(arguments.fd_write, admission);
			delete [] admission;

		}
	}

	else if(strcmp(tok,"/numPatientDischarges") == 0){  //case /numPatientDischarges

		tok = strtok(NULL, "$");
		virus = string(tok);
		tok = strtok(NULL,"$");
		key1 = atoi(tok);
		tok = strtok(NULL,"$");
		key2 = atoi(tok);
		tok = strtok(NULL,"$");
		if(tok == NULL){ 

			discharge = htcountries->num_patient_discharges(virus, key1, key2, arguments.country_num);
			write_to_fifo(arguments.fd_write, discharge);
			delete [] discharge;

		}	

		else 
		{

			country = string(tok);
			discharge = htcountries->num_patient_discharges(country, virus, key1, key2);
			write_to_fifo(arguments.fd_write, discharge);
			delete [] discharge;

		}
	}
	
}



void terminate(int signum ){  

	if(signum == SIGUSR1){

		cout << "I am child process " << getpid() << " and I quit" << endl;

	}

	delete htcountries;   
	delete htdiseases;	
	for (int i = 0; i < 8; ++i)
	{
		delete disease_statistics[i];
	}
	close (arguments.fd_write);
	close (arguments.fd_read);
	unlink(arguments.fifo_read); //erase fifos
	unlink(arguments.fifo_write);
	exit(0);
}





int main(int argc, char ** argv){

	signal ( SIGUSR1 , terminate);
	init(argc,argv); 
	receiving_countries_from_fifo();
	database.entry_type_check(); 
	char * query;

	while(1) 
	{
		query = read_from_fifo(arguments.fd_read);
		read_query_and_write_back(query);
	}	
} 	