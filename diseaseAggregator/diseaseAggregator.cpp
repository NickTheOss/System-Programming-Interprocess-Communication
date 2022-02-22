#include "diseaseAggregator.h"

Arguments arguments;    

void init(int c, char ** v){  

	int i;

	if(c < 2){  
			cout << "default values for arguments are workers = 20, buffer_size = 50 , input_dir = ../input_dir" << endl;  
			arguments.workers_num = 8;
			arguments.fifo_buffer_size = 50;
			arguments.input_dir = "../test_dir";  
			if(arguments.workers_num <= 0){
				cout << "Worker number must be positive, error occured" << endl;
				exit(1);
			}
	
		}

	for ( i = 1; i < c; i++)  
	{
		
		if(strcmp(v[i], "-w") == 0){  
			arguments.workers_num = atoi(v[i+1]);   
			if(arguments.workers_num <= 0){  
				cout << "Worker number must be positive, error occured" << endl;
				exit(1);
			}

		}
		else if(strcmp(v[i], "-b") == 0){
			arguments.fifo_buffer_size = atoi(v[i+1]);   

		}
		else if(strcmp(v[i], "-i") == 0){
			arguments.input_dir = v[i+1];  

		}	
	}	
}



void get_countries( const char * path , worker_list & wl){	  

  	DIR * dir_ptr = opendir(path); 

  	if(dir_ptr == NULL){   

  		cout<< "Error opening directory" << endl;
  		exit(1); 
  	} 

	char buffer[257]; 
	struct dirent * direntp; // for the directory entries
	

	while ((direntp = readdir(dir_ptr)) != NULL){   

  		if(direntp->d_type == DT_DIR && strcmp(direntp->d_name, ".") != 0  && strcmp(direntp->d_name, "..") != 0 ){
    		sprintf(buffer, "%s/%s", path, direntp->d_name);
    		wl.insert_node(buffer); //insert to list

  		}
    }
    closedir(dir_ptr); 


}

worker_list::~worker_list(){  


	worker_node * current;

	while(head){

		current = head;
		head = head->next;
		delete current;

	}
}


void worker_list::print_exit(int total, int valid, int error){ 

	worker_node * current = head;
	int pid = getpid();
	char pid_c[6];
	char buffer[MAXSIZE];
	char * tok;
	strcpy(buffer, "log_file.");
	sprintf(pid_c, "%d", pid);
	strcat(buffer,pid_c);
	FILE * fp = fopen(buffer, "w+");
	while(current){
		tok = strtok((char *)current->country.c_str(), "/");
		tok = strtok(NULL, "/");
		tok = strtok(NULL, "$");
		fprintf(fp,"%s\n", tok);
		current = current->next;
	}
	fprintf(fp,"TOTAL %d\nSUCCESS %d\nFAIL %d\n", total, valid, error);

}



int tokenize_date(string & input_date){   

	istringstream iss(input_date);   
	string token;
	int d, m, y, key, counter = 0;

	while (getline(iss, token, '-'))   
	    {	
	    	if(counter == 0) d = atoi(token.c_str());   
	    	if(counter == 1) m = atoi(token.c_str());  
	    	if(counter == 2) y = atoi(token.c_str());  
	    	counter++;
	     
	    }
	    if (d <= 0 || d > 31){   

	    	cout<< "Invalid day input. Value must be between 1-31,error occured"<<endl;
	    	
	    	return -1;
	    }
	    if(m <= 0 || m > 12){  

	    	cout<< "Invalid month input. Value must be between 1-12, error occured" << endl;
	    	
	    	return -1;
	    }
	    if(y < 1900 || y > 2020){   

	    	cout<< "Invalid year input. Value must be between 1900-2020,error occured" <<endl;
	    	
	    	return -1;
	    }

	    key = (y-1900) * 365 + 30 * m + d;  
	    return key;
}







void input_command(worker_info * workers, worker_list & wl){   
	int error_counter = 0, valid_counter = 0, total_sum;
	string input, tok;    
	int date_key1, date_key2, worker_num;
	char query[MAXSIZE];
	char key1[15], key2[15], kk[5];
	string check_query, country_path;
	char * buf;
	

	for (string line; getline(cin, line);){   

		istringstream iss(line);  
		iss >> input;
		strcpy(query, input.c_str());  

		if(input.compare("/listCountries") == 0){    //  case /listCountries
			check_query = input;
			input = "";
		    iss >> input;

	    	if( input == "") {   //valid input

	    		for (int i = 0; i < arguments.workers_num; ++i) 
	    		{	
	    			write_to_fifo(workers[i].fifo_fd_write, query);
	    		}

	 
	    		iss.clear(); 
	    		valid_counter++;
	    		cout << "Processing request.." << endl;
	    		read_from_all_workers( workers);  
	    		continue;   
	    	}

	    	else {   //error
	    		error_counter++;
	    		iss.clear();
	    		cout << "'/listCountries' requires one argument only,error occured" << endl;
	    		continue;
	    	}	   
		}
		

		else if (input.compare("/diseaseFrequency") == 0){     //  case /diseaseFrequency
			check_query = input;
			string virus_name, country_name;
			
			int date_key1, date_key2, sum = 0;
			input = "";
    		iss >> input;     

    		if(input == "") {  

	    		cout << "Too few arguments,please give at least virus name and date1,date2 " << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}
	    	strcat(query, "$");
	    	virus_name = input;
	    	strcat(query, virus_name.c_str());
	    	strcat(query, "$");
	        input = "";
	        iss >> input;

	        if(input == "") {   

	    		cout << "Entry and exit Dates are required,try again " << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	if( input[2] == '-' ||  input[1] == '-'){  

	    		date_key1 = tokenize_date(input); 
	    		sprintf(key1,"%d", date_key1);
	    		strcat(query, key1);
	    		strcat(query, "$");
	    		if (date_key1 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}
	    		
	    	}

	    	else  
	    	{
	    		cout << "Entry date value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

	    	if (input == "" || input[0] == '-' ){   

	    		cout << "Exit date is also required, please try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	else if(input[2] == '-' ||  input[1] == '-'){  

	    		date_key2 = tokenize_date(input);
	    		sprintf(key2,"%d", date_key2);
	    		strcat(query, key2);
	    		strcat(query, "$");
	    		if (date_key2 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}
	    		
	    		if(date_key2 < date_key1){  

	    			cout << "Date2 value must be greater than Date1, error occured" << endl;
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}

	    	}

	    	else   
	    	{

	    		cout << "Exit date value must have dashes between numbers,error occured" << endl;
	    		//cout << "error" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

	    	if (input == ""){  
	    		
	    		for (int i = 0; i < arguments.workers_num; ++i)  
	    		{
	    			write_to_fifo(workers[i].fifo_fd_write, query);
	    		}
	    	
	    		read_from_all_workers( workers, check_query); 

	    		iss.clear();
	    		valid_counter++;
	    		continue;

	    	}
	 
	    	else  
	    	{
	    		country_name = input;
	    		country_path = arguments.input_dir + "/" + country_name;
	    		worker_num = search_country(country_path, workers); 
	    		strcat(query, country_name.c_str());
	    		strcat(query, "$");
	    		write_to_fifo(workers[worker_num].fifo_fd_write, query);  
	  			usleep(10);  
	    		
	    		buf = read_from_fifo(workers[worker_num].fifo_fd_read);  
	    		cout << buf << endl;
	    		delete [] buf;   //free
	    		iss.clear();
	    		valid_counter++;
	    		continue;

	    	}
	    }
		

		else if (input.compare("/topk-AgeRanges") == 0){      //  case /topk-AgeRanges

			int date_key1, date_key2, k;
			input = "";
			string country, disease;
    		iss >> input;

    		if(input == "") {  

	    		cout << "Too few arguments,please give at least top k and country name " << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	} 	
	    	k = atoi(input.c_str());  

	    	if(k <= 0 || k > 4){
	    		cout << "k must be greater than zero(0) and less or equal than four(4), try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}
	    	strcat(query, "$");
	    	sprintf(kk, "%d", k);
	    	strcat(query, kk);
	    	strcat(query, "$");
	    	
	        input = "";
    		iss >> input;

	        if(input == "") {  

	    		cout << "Country name is also required ,try again please" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	country = input;
	    	strcat(query, country.c_str());
	    	strcat(query, "$");
    		input = "";
    		iss >> input;

    		if(input == "") {  

	    		cout << "Disease name is also required, please try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	disease = input;
	    	strcat(query, disease.c_str());
	    	strcat(query, "$");
	    	input = "";
    		iss >> input;

    		if(input == "") {  

	    		cout << "date1 and date2 are also required" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	if( input[2] == '-' ||  input[1] == '-'){ 

	    		date_key1 = tokenize_date(input); 
	    		sprintf(key1, "%d", date_key1);
	    		strcat(query, key1);
	    		strcat(query, "$");
	    		if (date_key1 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}
	    		
	    	}

	    	else  
	    	{
	    		cout << "Entry date value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

	    	if (input == "" || input[0] == '-' ){ 

	    		cout << "Exit date is also required, please try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	else if(input[2] == '-' ||  input[1] == '-'){  

	    		date_key2 = tokenize_date(input);
	    		sprintf(key2, "%d", date_key2);
	    		strcat(query, key2);
	    		strcat(query, "$");
	    		if (date_key2 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}

	    		country_path = arguments.input_dir + "/" + country;
	  			worker_num = search_country(country_path, workers);  
	  			write_to_fifo(workers[worker_num].fifo_fd_write, query);
	    	   
	    	  	usleep(10); 
	  			buf = read_from_fifo(workers[worker_num].fifo_fd_read); 
	  			cout << buf << endl;
	  			delete [] buf;

	    		iss.clear();
	    		valid_counter++;
	  			continue;

	    	}

	    	else  
	    	{
	    		cout << "Exit date value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}
		}


		else if (input.compare("/searchPatientRecord") == 0){      // case /searchPatientRecord
			check_query = input;
			string recordID;
			input = "";
    		iss >> input;

    		if(input == "") {   

	    		cout << "Too few arguments,recordID is also required " << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}
	    	strcat(query, "$");

	        recordID = input;
	        strcat(query, recordID.c_str());
	        strcat(query, "$");
	        for (int i = 0; i < arguments.workers_num; ++i)  
	        {
	        	write_to_fifo(workers[i].fifo_fd_write, query);
	        }
	          
	        read_from_all_workers(workers,check_query);
	
	        iss.clear();
	        valid_counter++;
	    	continue;
    		
		}


		else if (input.compare("/numPatientAdmissions") == 0 ){    ///numPatientAdmissions

			int date_key1, date_key2; 
			string disease, country;
			input = "";
    		iss >> input;

    		if( input == "") {  

	    		cout << "Too few arguments" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}
	    	strcat(query, "$");
	    	
	        disease = input;    
	        strcat(query, disease.c_str());
	        strcat(query, "$");
	        input = "";
    		iss >> input;

    		if( input == "") {

	    		cout << "Date1 and Date2 are also required" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	

	    	if( input[2] == '-' ||  input[1] == '-'){ 

	    		date_key1 = tokenize_date(input);
	    		if (date_key1 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}
	    		sprintf(key1, "%d", date_key1);
	    		strcat(query, key1);
		 		strcat(query, "$");
	    	}

	    	else 
	    	{
	    		cout << "Date1 value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

	    	if (input == "" || input[0] == '-' ){ 

	    		cout << "Date2 is also required, please try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	else if(input[2] == '-' ||  input[1] == '-'){  

	    		date_key2 = tokenize_date(input);
	    		if (date_key2 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}
	    		sprintf(key2, "%d", date_key2);
	    		strcat(query, key2);
		 		strcat(query, "$");

	    	}

	    	else  
	    	{
	    		cout << "Date2 date value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

    		if( input == "") { 
    		
    			for (int i = 0; i < arguments.workers_num; ++i)
    			{
    				write_to_fifo(workers[i].fifo_fd_write, query);
    			}

    			read_from_all_workers(workers);
    		
	    		iss.clear();
	    		continue;
	    	}

	    	country = input; 
	    	country_path = arguments.input_dir + "/" + country;
	    	strcat(query, country.c_str());
		 	strcat(query, "$");
		 	worker_num = search_country(country_path, workers);  
		 	write_to_fifo(workers[worker_num].fifo_fd_write, query);  
	    		
			usleep(10);
			buf = read_from_fifo(workers[worker_num].fifo_fd_read);  
			cout << buf << endl;
			delete [] buf;
	    	iss.clear();
	    	valid_counter++;
	    	continue;

		}


		else if (input.compare("/numPatientDischarges") == 0 ){   // /numPatientDischarges

	
			int date_key1, date_key2; 
			string disease, country;
			input = "";
    		iss >> input;

    		if( input == "") {  

	    		cout << "Too few arguments" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}
	    	strcat(query, "$"); 
	        disease = input;   
	        strcat(query, disease.c_str());
	        strcat(query, "$"); 
	        input = "";
    		iss >> input;

    		if( input == "") {

	    		cout << "Date1 and Date2 are also required" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	

	    	if( input[2] == '-' ||  input[1] == '-'){ 

	    		date_key1 = tokenize_date(input);
	    		if (date_key1 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}

	    		sprintf(key1, "%d", date_key1);
	    		strcat(query, key1);
		 		strcat(query, "$");
	 	

	    	}

	    	else  
	    	{
	    		cout << "Date1 value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

	    	if (input == "" || input[0] == '-' ){

	    		cout << "Date2 is also required, please try again" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;

	    	}

	    	else if(input[2] == '-' ||  input[1] == '-'){  

	    		date_key2 = tokenize_date(input);
	    		if (date_key2 < 0)
	    		{
	    			iss.clear();
	    			error_counter++;
	    			continue;
	    		}

	    		sprintf(key2, "%d", date_key2);
	    		strcat(query, key2);
		 		strcat(query, "$");

	    	}

	    	else  
	    	{
	    		cout << "Date2 date value must have dashes between numbers,error occured" << endl;
	    		iss.clear();
	    		error_counter++;
	    		continue;
	    	}

	    	input = "";
	    	iss >> input;

    		if( input == "") { 
	
    			for (int i = 0; i < arguments.workers_num; ++i) 
    			{
    				write_to_fifo(workers[i].fifo_fd_write, query);
    			}

    			read_from_all_workers(workers);

	    		iss.clear();
	    		continue;
	    	}

	    	country = input; 
	    	country_path = arguments.input_dir + "/" + country; 
	    	strcat(query, country.c_str());
		 	strcat(query, "$");
		 	worker_num = search_country(country_path, workers);  
		 	write_to_fifo(workers[worker_num].fifo_fd_write, query);

		 	usleep(10);
		 	buf = read_from_fifo(workers[worker_num].fifo_fd_read);
		 	cout << buf << endl;
		 	delete [] buf;

	    	iss.clear();
	    	valid_counter++;
	    	continue;

		} 

	
		else if(input.compare("/exit") == 0 ){  // case /exit

			int status;
			
			for (int i = 0; i < arguments.workers_num; ++i)
			{
				kill( workers[i].pid,  SIGUSR1);  
				waitpid(workers[i].pid, &status, 0); 
			}
			valid_counter++;
			total_sum = valid_counter + error_counter;  
			wl.print_exit(total_sum, valid_counter, error_counter); 
			break;
		
		}
			

		else   // any other /case
		{
			cout << "error" << endl;
			error_counter++;
			iss.clear();
			continue;
		}
		
	}
}





void worker_list::insert_node(const char buffer[]){ 
	worker_node * node = new worker_node(buffer);

	if( list_empty()){

		head = node;
		tail = node;
		worker_node_counter++;


	}

	else{

		tail->next = node; 
		tail = node;
		worker_node_counter++;

	}
}


bool worker_list::search_node(string country){  

	worker_node * node = head;
	char * tok;
	while(node){

		if (node->country == country) return true;  
		node = node->next;

	}
	return false;  //not found

}


int search_country( string & country, worker_info * w_info){

	for (int i = 0; i < arguments.workers_num; ++i)
	{
		
		if(w_info[i].countries.search_node(country)) return i;  

	}

	return -1;

}






void disperse_countries(worker_list & wl, worker_info * w_info){  

	worker_node * current = wl.head;

	int counter = 0;  
	while(current){

		w_info[counter].countries.insert_node(current->country.c_str()); 
		current = current->next;
		counter++;
		if (counter == arguments.workers_num) counter = 0;  

	}
}

char *  worker_list::get_countries(){  

	char * buffer = new char[MAXSIZE * worker_node_counter]; 
	worker_node * current = head;
	
	while(current){	

		
		if(current == head) 
		 {
			strcpy(buffer, current->country.c_str());
			strcat(buffer, "$");
			current = current->next;	
			continue;
		}
		strcat(buffer, current->country.c_str()); 
		strcat(buffer, "$");
		current = current->next;

	}

	return buffer;

}

void send_countries_to_workers(worker_info * workers){  


	char * countries;
	
	for (int i = 0; i < arguments.workers_num; ++i)
	{
		
		countries = workers[i].countries.get_countries(); // line 865
		write_to_fifo(workers[i].fifo_fd_write, countries );
		delete [] countries;
	}

}



void make_and_open_fifos(worker_info * workers){  

	char buf[MAXSIZE];
	struct stat stat_temp;

	for (int i = 0; i < arguments.workers_num; ++i) 
	{
		sprintf(buf, "../fifos/read%d", i );  
		strcpy(workers[i].read_fifo, buf);

		if(stat(workers[i].read_fifo, &stat_temp) == -1){  
			if (mkfifo(workers[i].read_fifo,PERMISSIONS) < 0 ){                
	            perror("Error creating the named read pipe");
	            exit(1);
	        }
		}	

		workers[i].fifo_fd_read = open(workers[i].read_fifo, O_RDONLY | O_NONBLOCK); 

		sprintf(buf, "../fifos/write%d", i );
		strcpy(workers[i].write_fifo, buf);

	}

}



void open_fifo_for_write(worker_info * workers){ 

	for (int i = 0; i < arguments.workers_num; ++i)
	{
		
		workers[i].fifo_fd_write = open(workers[i].write_fifo, O_WRONLY | O_NONBLOCK);
		while(workers[i].fifo_fd_write == -1){  

			usleep(10); 
			workers[i].fifo_fd_write = open(workers[i].write_fifo, O_WRONLY | O_NONBLOCK);
		}

	}

}



void create_workers(int buffer_size, worker_info * workers){ 

	char buffer_size_str[MAXSIZE];
	sprintf(buffer_size_str,"%d", buffer_size);
	pid_t pid;

	for (int i = 0; i < arguments.workers_num; ++i)
	{	
		
		ec_neg1(pid = fork());  
		if (pid == 0){  

			execl("../worker/worker","../worker/worker", workers[i].write_fifo, workers[i].read_fifo, buffer_size_str, (char * )NULL);

		}
		else
			workers[i].pid = pid;

	}
}



char * read_from_fifo(int filedesc){  

    char buffer[10];
    memset(buffer,0,10);
    int fifo_buffer_size = arguments.fifo_buffer_size;  // buffer size
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




void set_worker_flags(worker_info * workers){  

	for (int i = 0; i < arguments.workers_num; ++i)
	{
		workers[i].not_sent_yet = true;
	}

}


bool worker_sent_remain(worker_info * workers){         

	for (int i = 0; i < arguments.workers_num; ++i)
	{
		if(workers[i].not_sent_yet) return true;
	}

	return false;

} 



void read_from_all_workers( worker_info * workers){  

	set_worker_flags(workers);  
	struct pollfd filedescs[arguments.workers_num];
	char * buf;
 
	while(worker_sent_remain(workers)){ 
	   for (int i = 0; i < arguments.workers_num; i++)

	   {
	   		filedescs[i].fd = workers[i].fifo_fd_read;
		    filedescs[i].events = POLLIN;
	   }
	   poll(filedescs,arguments.workers_num,10); //wait time 10

	   for (int i = 0; i < arguments.workers_num; i++)

	   {
			if((filedescs[i].revents & POLLIN)){ 
	            if(filedescs[i].fd == workers[i].fifo_fd_read){
	               buf = read_from_fifo(workers[i].fifo_fd_read); 
	              
	               cout << buf << endl;
	              
	               delete [] buf;
	               workers[i].not_sent_yet = false;  

	            }
	   	    }              
	    }	
	}
}







void read_from_all_workers( worker_info * workers, string query){ 
	set_worker_flags(workers);
	struct pollfd filedescs[arguments.workers_num];
	char * buf;
	int d_freq, freq_counter = 0;
	bool flag = false;
	bool flag_search = false;
 
	while(worker_sent_remain(workers)){  
	   for (int i = 0; i < arguments.workers_num; i++)

	   {
	   		filedescs[i].fd = workers[i].fifo_fd_read;
		    filedescs[i].events = POLLIN;
	   }
	   poll(filedescs,arguments.workers_num,10);

	   for (int i = 0; i < arguments.workers_num; i++)

	   {
			if((filedescs[i].revents & POLLIN)){
	            if(filedescs[i].fd == workers[i].fifo_fd_read){
	               buf = read_from_fifo(workers[i].fifo_fd_read);
	               if(query == "/diseaseFrequency"){ 

	               		d_freq = atoi(buf);
	               		freq_counter += d_freq;
	               		flag = true;

	               }
	               else if(query == "/searchPatientRecord"){ 

	               		if( strcmp(buf, "Record not found\n") == 0){
	               			delete [] buf;
	               			workers[i].not_sent_yet = false; 
	               			continue;

	               		}
	               		else
	            
	               			flag_search = true;
	               			cout << buf << endl;


	               }
	               else
	               		cout << buf << endl;
	              
	               delete [] buf;
	               workers[i].not_sent_yet = false; 

	            }
	   	    }              
	    }	 
	}
	if(flag) cout << freq_counter << endl;
	if (!flag_search && query == "/searchPatientRecord") cout << "Record not found" << endl;
}


int main(int argc, char ** argv){

	worker_list wl;
	int status;
	init(argc,argv);  	
	worker_info workers[arguments.workers_num]; 
	cout << "Getting statistics..." << endl;
	get_countries(arguments.input_dir.c_str(), wl);
	if(wl.worker_node_counter < arguments.workers_num) arguments.workers_num = wl.worker_node_counter;
	disperse_countries( wl, workers);
	make_and_open_fifos(workers);
	create_workers( arguments.fifo_buffer_size, workers);
	open_fifo_for_write( workers);
	send_countries_to_workers(workers);
	read_from_all_workers( workers); //gia ta statistic
	cout << "Fifo is ready for query..type /exit for exit" << endl;
	input_command(workers, wl);
	cout << "Parent process now terminates.." << endl;

	exit(0);
} 	