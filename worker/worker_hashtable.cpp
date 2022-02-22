#include "worker_hashtable.h"


void Bucket_item::insert_patient(string & name, int key, Record * rec){  
	
		if(this->name == name) avl->insert_to_avl( key, rec);
		else   
		{
			this->name = name;
			avl->insert_to_avl( key, rec);
		}

}


Bucket_item::Bucket_item(){

	avl = new Avl();  

}

Bucket_item::~Bucket_item(){

	delete avl;   

}



void Bucket::insert_patient(string & name, int key, Record * rec){


	bucket_items[index].insert_patient(name, key, rec); 
	index++; 

}



Bucket_list::~Bucket_list(){  

	Bucket_node * current;
	while(head){

		current = head;
		head = head->next;
		delete current;

	}
}



int Bucket_list::insert_new_patient(int date_key, string & name, Record * rec){ 
	
	
	if (is_empty()){  
		head = new Bucket_node(bucket_size); 
		tail = head;
		bucket_node_counter++;  
		head->bucket_pointer->insert_patient(name, date_key, rec);  
		return 1;

	}
	 
	Bucket_node * current = head; 
	bool flag = false;
	while(current){  

		for (int i = 0; i < current->bucket_pointer->index; ++i) 
		{
			
			if(current->bucket_pointer->bucket_items[i].name == name){ 

				current->bucket_pointer->bucket_items[i].insert_patient(name, date_key, rec); 
				flag = true;
				break;
			}

		}
		if(flag) break; 
		current = current->next;  

	}

	if(flag) return 0; 
	if(tail->bucket_pointer->is_bucket_full() == false){  

		tail->bucket_pointer->insert_patient(name, date_key, rec);  
		return 1;

	}
		//alliws
		tail->next = new Bucket_node(bucket_size);
		bucket_node_counter++;
		tail = tail->next;
		tail->bucket_pointer->insert_patient(name, date_key, rec); 
		return 1;

}



void Bucket_list::calculate_statistics(char buf[]){ 

	Bucket_node * current = head;
	char A[5], B[5], C[5] , D[5];
	while(current){  

		for (int i = 0; i < current->bucket_pointer->index; ++i)
		{
			if (current->bucket_pointer->bucket_items[i].avl->tree_node_counter != 0){ 
				strcat(buf, current->bucket_pointer->bucket_items[i].name.c_str());
				strcat(buf, "\n");
				strcat(buf, "Age range 0-20 years: ");
				sprintf(A,"%d", current->bucket_pointer->bucket_items[i].avl->tree_node_counterA);
				strcat(buf, A);
				strcat(buf, " cases");
				strcat(buf, "\n");
				strcat(buf, "Age range 21-40 years: ");
				sprintf(B,"%d", current->bucket_pointer->bucket_items[i].avl->tree_node_counterB);
				strcat(buf, B);
				strcat(buf, " cases");
				strcat(buf, "\n");
				strcat(buf, "Age range 41-60 years: ");
				sprintf(C,"%d", current->bucket_pointer->bucket_items[i].avl->tree_node_counterC);
				strcat(buf, C);
				strcat(buf, " cases");
				strcat(buf, "\n");
				strcat(buf, "Age range 60+ years: ");
				sprintf(D,"%d", current->bucket_pointer->bucket_items[i].avl->tree_node_counterD);
				strcat(buf, D);
				strcat(buf, " cases");
				strcat(buf, "\n\n");

			}
		}

		current = current->next;
	}
}



int Bucket_list::diseaseFrequency(string & virus_name, int node_key1, int node_key2){ 

	Bucket_node * current = head;
	int sum = 0;
	while(current){

		for (int i = 0; i < current->bucket_pointer->index; ++i){
			if (current->bucket_pointer->bucket_items[i].avl->tree_node_counter != 0){

					sum += current->bucket_pointer->bucket_items[i].avl->get_virus_frequency( virus_name, node_key1, node_key2);

			}

		}
		current = current->next;
	}
	return sum;
}



int Bucket_list::diseaseFrequency(string & country_name, string & virus_name, int node_key1, int node_key2){

	Bucket_node * current = head;
	int sum = 0;
	while(current){

		for (int i = 0; i < current->bucket_pointer->index; ++i)
		{
			if(country_name.compare(current->bucket_pointer->bucket_items[i].name) == 0){  
				if (current->bucket_pointer->bucket_items[i].avl->tree_node_counter != 0){
					sum += current->bucket_pointer->bucket_items[i].avl->get_virus_frequency( virus_name, node_key1, node_key2);
				}
			}	
		}

		current = current->next;
	}
	return sum;
}



char * sort_and_print(int A, int B, int C, int D, int freq, int k){ 

	struct sort_table table[4]; 
	char value[8];
	char * buffer = new char[50];

	table[0].value = A;
	table[0].category = "0-20";
	table[1].value = B;
	table[1].category = "21-40";
	table[2].value = C;
	table[2].category = "41-60";
	table[3].value = D;
	table[3].category = "60+";
	int temp, i, j;
	string temp2;
	for ( i = 0; i < 3; ++i)
	{
		for ( j = 0; j < 3-i; ++j)
		{
			if(table[j].value < table[j+1].value){ //bubblesort
				temp = table[j].value;
				table[j].value = table[j+1].value;
				table[j+1].value = temp;
				temp2 = table[j].category;
				table[j].category = table[j+1].category;
				table[j+1].category = temp2;
			}
		}
	}


	for ( i = 0; i < 4; ++i) 
	{
		if(freq != 0){
			table[i].value = (table[i].value*100) / freq;
		}
		else
			table[i].value = 0; 
	}
	
	switch (k){  

		case 1:
			strcpy(buffer, table[0].category.c_str());
			strcat(buffer, ": ");
			sprintf(value, "%d", table[0].value);
			strcat(buffer, value);
			strcat(buffer,"%\n");
			return buffer;
		
		case 2:

			strcpy(buffer, table[0].category.c_str());
			strcat(buffer, ": ");
			sprintf(value, "%d", table[0].value);
			strcat(buffer, value);
			strcat(buffer,"%\n");

			for ( i = 1; i < 2; ++i)
			{

				strcat(buffer, table[i].category.c_str());
				strcat(buffer, ": ");
				sprintf(value, "%d", table[i].value);
				strcat(buffer, value);
				strcat(buffer,"%\n");

			}
			return buffer;

		case 3:

			strcpy(buffer, table[0].category.c_str());
			strcat(buffer, ": ");
			sprintf(value, "%d", table[0].value);
			strcat(buffer, value);
			strcat(buffer,"%\n");

			for ( i = 1; i < 3; ++i)
			{	

				strcat(buffer, table[i].category.c_str());
				strcat(buffer, ": ");
				sprintf(value, "%d", table[i].value);
				strcat(buffer, value);
				strcat(buffer,"%\n");	

			}
			return buffer;

		case 4:

			strcpy(buffer, table[0].category.c_str());
			strcat(buffer, ": ");
			sprintf(value, "%d", table[0].value);
			strcat(buffer, value);
			strcat(buffer,"%\n");
			for ( i = 1; i < 4; ++i)
			{
				strcat(buffer, table[i].category.c_str());
				strcat(buffer, ": ");
				sprintf(value, "%d", table[i].value);
				strcat(buffer, value);
				strcat(buffer,"%\n");
				
			}
			return buffer;

	}
	
}




char * Bucket_list::topk_diseases_in_country(int k, string & disease, int date_key1, int date_key2){ 

	Bucket_node * current = head;
	Avl  temp;
	int freq, A, B, C, D;
	while(current){

		for (int i = 0; i < current->bucket_pointer->index; ++i)
		{
	
				freq = current->bucket_pointer->bucket_items[i].avl->get_country_diseasefreq( disease, date_key1, date_key2 );
				A = current->bucket_pointer->bucket_items[i].avl->A;  //A = 0-20
				B = current->bucket_pointer->bucket_items[i].avl->B;  //B = 21-40
				C = current->bucket_pointer->bucket_items[i].avl->C;  //C = 41-60
				D = current->bucket_pointer->bucket_items[i].avl->D;  //D = 60+
			    
				temp.set_ABCD_NULL(current->bucket_pointer->bucket_items[i].avl); //in case we want same querry once more
		}
		current = current->next;
	}
	return sort_and_print(A, B, C, D, freq, k);

}


char * Bucket_list::num_patient_admissions(string & disease, int date_key1, int  date_key2){ 

	Bucket_node * current = head;
	char * buf = new char[35];
	char freq[4];
	
	int counter = 0;

	while(current){
		
		for (int i = 0; i < current->bucket_pointer->index; ++i){
			if (current->bucket_pointer->bucket_items[i].avl->tree_node_counter != 0){
				if ( counter == 0){
					strcpy(buf, current->bucket_pointer->bucket_items[i].name.c_str());
					strcat(buf," ");
					sprintf(freq, "%d", current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_ENTER( disease, date_key1, date_key2));
					strcat(buf, freq);
					strcat(buf, "\n");
		
				}
				else
				{
					strcat(buf, current->bucket_pointer->bucket_items[i].name.c_str());
					strcat(buf," ");
					sprintf(freq, "%d", current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_ENTER( disease, date_key1, date_key2));
					strcat(buf, freq);
					strcat(buf, "\n");

				}
			}
			 counter++;

		}
		
		current = current->next;
	}

	return buf;
}



char * Bucket_list::num_patient_admissions(string & country, string & disease, int date_key1, int  date_key2){ 

	Bucket_node * current = head;
	char admissions[5];
	char * buf = new char[10];
	while(current){
		
		for (int i = 0; i < current->bucket_pointer->index; ++i){
			if(country.compare(current->bucket_pointer->bucket_items[i].name) == 0){
							
				sprintf(admissions, "%d",current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_ENTER( disease, date_key1, date_key2));
				strcpy(buf, admissions);
				strcat(buf, "\n");
			}
		}

		current = current->next;
	}
	return buf;
}




char * Bucket_list::num_patient_discharges(string & disease, int date_key1, int  date_key2){ 

	Bucket_node * current = head;
	char * buf = new char[35];
	char freq[4];
	
	int counter = 0;

	while(current){
		
		for (int i = 0; i < current->bucket_pointer->index; ++i){
			if (current->bucket_pointer->bucket_items[i].avl->tree_node_counter != 0){
				if ( counter == 0){
					strcpy(buf, current->bucket_pointer->bucket_items[i].name.c_str());
					strcat(buf," ");
					sprintf(freq, "%d", current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_EXIT( disease, date_key1, date_key2));
					strcat(buf, freq);
					strcat(buf, "\n");

				}
				else
				{
					strcat(buf, current->bucket_pointer->bucket_items[i].name.c_str());
					strcat(buf," ");
					sprintf(freq, "%d", current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_EXIT( disease, date_key1, date_key2));
					strcat(buf, freq);
					strcat(buf, "\n");

				}
			}
			 counter++;
		}
		
		current = current->next;

	}

	return buf;
}



char * Bucket_list::num_patient_discharges(string & country, string & disease, int date_key1, int  date_key2){ 

	Bucket_node * current = head;
	char discharges[5];
	char * buf = new char[10];
	while(current){
		
		for (int i = 0; i < current->bucket_pointer->index; ++i){
			if(country.compare(current->bucket_pointer->bucket_items[i].name) == 0){
					
				sprintf(discharges, "%d",current->bucket_pointer->bucket_items[i].avl->get_virus_frequency_EXIT( disease, date_key1, date_key2));
				strcpy(buf, discharges);
				strcat(buf, "\n");
			}
		}

		current = current->next;
	}
	return buf;
}




HashTable::HashTable(int size, int bucket_size):size(size), itemscounter(0){  

	if(bucket_size < sizeof(Bucket_item)){ 

		bucket_size = 1; 

	}

	else{

		bucket_size = bucket_size/sizeof(Bucket_item);   

	}

	list_pointer = new Bucket_list * [size]; 
	for (int i = 0; i < size; i++)  
	{
		
		list_pointer[i] = new Bucket_list(bucket_size); 
	}

}



HashTable::~HashTable(){  

	
	for (int i = 0; i < size; ++i)
	{
		delete list_pointer[i];
	}

	delete [] list_pointer;
}




int HashTable::hash(const char *str )  
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)

        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;  
}



void HashTable::insert_new_patient(int date_key, string & name, Record * rec){  

	
 	int position = hash(name.c_str()); 
 	itemscounter += list_pointer[position]->insert_new_patient( date_key, name, rec);


}





void HashTable::calculate_statistics( char buf[]){  

	for (int i = 0; i < size; ++i)  
	{
		
		list_pointer[i]->calculate_statistics(buf);  

	}

}





int HashTable::diseaseFrequency( string & virus_name, int node_key1, int node_key2){

	int position = hash(virus_name.c_str());  
	return list_pointer[position]->diseaseFrequency( virus_name, node_key1, node_key2); 

}



int HashTable::diseaseFrequency(string & country_name, string & virus_name, int node_key1, int node_key2){

	int position = hash(country_name.c_str());
	return list_pointer[position]->diseaseFrequency( country_name,  virus_name, node_key1, node_key2);

}



char * HashTable::topk_diseases_in_country( int k, string & country, string & disease, int date_key1, int date_key2 ){  

	int position = hash(country.c_str());
	return list_pointer[position]->topk_diseases_in_country(k, disease, date_key1, date_key2 );


}


char * HashTable::num_patient_admissions(string & disease, int date_key1, int  date_key2, int country_num){


	int counter = 0;
	char * buffer = new char[country_num * 35];
	char * buf_return;
	
 	for (int i = 0; i < size; ++i)
	{

		if(counter == 0) 
		{
			buf_return = list_pointer[i]->num_patient_admissions( disease, date_key1, date_key2);
			strcpy(buffer, buf_return); 
			delete [] buf_return;
			
		}
		else
		{	
			buf_return = list_pointer[i]->num_patient_admissions( disease, date_key1, date_key2);
			strcat(buffer, buf_return);
			delete  [] buf_return;
		}
		counter++;
		
	}
	return buffer;

}



char * HashTable::num_patient_admissions(string & country, string & disease, int date_key1, int  date_key2){

	int position = hash(country.c_str());  
	return list_pointer[position]->num_patient_admissions(country, disease, date_key1, date_key2); 

}



char * HashTable::num_patient_discharges(string & disease, int date_key1, int  date_key2, int country_num){

	int counter = 0;
	char * buffer = new char[country_num * 35];
	char * buf_return;
 	for (int i = 0; i < size; ++i)
	{

		if(counter == 0) 
		{

			buf_return = list_pointer[i]->num_patient_discharges( disease, date_key1, date_key2);
			strcpy(buffer, buf_return); 
			delete [] buf_return;
		}
		else

		{
			buf_return = list_pointer[i]->num_patient_discharges( disease, date_key1, date_key2);
			strcat(buffer, buf_return);
			delete  [] buf_return;
		}
		counter++;
		
	}
	return buffer;

}



char * HashTable::num_patient_discharges(string & country, string & disease, int date_key1, int  date_key2){	
	int position = hash(country.c_str());  
	
	list_pointer[position]->num_patient_discharges(country, disease, date_key1, date_key2); 

}



