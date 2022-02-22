#include "tree.h"



List_node::List_node(Record * rec):record(rec),next(NULL){} 

List_node::~List_node(){

	delete record;   

}


Data_list::~Data_list(){  

	List_node * current;
	while(head){

		current = head;
		head = head->next;
		delete current;

	}
}



List_node * Data_list::insert_node(Record * rec){ 

	List_node * node = new List_node(rec); 

	if(counter == 0 ){ 

		head = node;
		tail = node;
		counter++;
        rec->position = counter;

	}

	else{

		tail->next = node; 
		tail = node;
		counter++;
        rec->position = counter;
	}

	return node;

}




bool Data_list::entry_type_check(){  

    List_node * current = head;
    string record_check;
    int counter;
    bool flag;
    while(current){
        int found = 0;
        if( current->record->ENTER_EXIT == "EXIT"){  
            List_node * check = head; 
            counter = 0;
            record_check = current->record->recordID;
            while(check){

                counter++;
                if(record_check == check->record->recordID){  
                    found++;
                    if ( counter == current->record->position){  
                        check = check->next;  
                        continue;
                    }
                    if (check->record->ENTER_EXIT != "ENTER" || (check->record->ENTER_EXIT == "ENTER" && check->record->date_key > current->record->date_key )){ 
                        cout<<"Record with ID = "<< record_check <<  "  has invalid date or entry status(" << check->record->ENTER_EXIT <<")" << endl;
                        exit(1);
                    }    
                }
                check = check->next;
            }   
            if (found < 2) { 
                cout << "Record " << current->record->recordID << " has no ENTER date" << endl; 
                exit(1);    
            }
        }
        current = current->next; 
    }
    return true;
}



char * Data_list::search_patient_record(string recordID){ 

    List_node * current = head;
    char * buf = new char[MAXSIZE];
    char age_c[3], day_c[3], month_c[3], year_c[5], day_c_exit[3], month_c_exit[3], year_c_exit[5];
    bool exit_flag = false;
    bool found_flag = false;
    int entry_d, entry_m, entry_y, exit_d, exit_m, exit_y, age;
    string disease, first_name, last_name;
    while(current){

        if (recordID == current->record->recordID){ 
            found_flag = true;
            if(current->record->ENTER_EXIT == "ENTER"){  
                entry_d = current->record->day;
                entry_m = current->record->month;
                entry_y = current->record->year;
                disease = current->record->disease_name;
                first_name = current->record->patientFirstName;
                last_name = current->record->patientLastName;
                age = current->record->age;
            }
            if(current->record->ENTER_EXIT == "EXIT"){ 
                exit_flag = true;
                exit_d = current->record->day;
                exit_m = current->record->month;
                exit_y = current->record->year;
            }

        }
      
        current = current->next;

    }

    if (found_flag == false){ 
        strcpy(buf,"Record not found\n");
        return buf;
       
    }
    if (exit_flag == true){
        strcpy(buf, recordID.c_str());
        strcat(buf," ");
        strcat(buf,first_name.c_str());
        strcat(buf," ");
        strcat(buf,last_name.c_str());
        strcat(buf," ");
        strcat(buf,disease.c_str());
        strcat(buf," ");
        sprintf(age_c, "%d", age);
        strcat(buf, age_c);
        strcat(buf," ");
        sprintf(day_c, "%d", entry_d);
        strcat(buf, day_c);
        strcat(buf,"-");
        sprintf(month_c, "%d", entry_m);
        strcat(buf, month_c);
        strcat(buf,"-");
        sprintf(year_c, "%d", entry_y);
        strcat(buf, year_c);
        strcat(buf," ");
        sprintf(day_c_exit, "%d", exit_d);
        strcat(buf, day_c_exit);
        strcat(buf,"-");
        sprintf(month_c_exit, "%d", exit_m);
        strcat(buf, month_c_exit);
        strcat(buf,"-");
        sprintf(year_c_exit, "%d", exit_y);
        strcat(buf, year_c_exit);
        strcat(buf,"\n");
    
        return buf;
    }
    else{  
         strcpy(buf, recordID.c_str());
        strcat(buf," ");
        strcat(buf,first_name.c_str());
        strcat(buf," ");
        strcat(buf,last_name.c_str());
        strcat(buf," ");
        strcat(buf,disease.c_str());
        strcat(buf," ");
        sprintf(age_c, "%d", age);
        strcat(buf, age_c);
        strcat(buf," ");
        sprintf(day_c, "%d", entry_d);
        strcat(buf, day_c);
        strcat(buf,"-");
        sprintf(month_c, "%d", entry_m);
        strcat(buf, month_c);
        strcat(buf,"-");
        sprintf(year_c, "%d", entry_y);
        strcat(buf, year_c);
        strcat(buf," --\n");
        return buf;

    }
}



void Avl::set_ABCD_NULL(Avl * avl){ 

    avl->A = 0;
    avl->B = 0;
    avl->C = 0;
    avl->D = 0;

}



Tree_node * Avl::insert(Tree_node * node, int key, Record * rec){ 
   
    if (node == NULL)   

        return new Tree_node(key, rec);  
    
    if (key <= node->node_key)  
        node->left = insert(node->left, key, rec);  
    else 
        node->right = insert(node->right, key, rec);  

    node->height = 1 + max(height(node->left), height(node->right));  
  
    int balance = getBalance(node);   

  
    if (balance > 1 && key < node->left->node_key)   // case left left
        return rightRotate(node);  
  
    
    if (balance < -1 && key > node->right->node_key)  // case right right
        return leftRotate(node);  
                 
    if (balance > 1 && key > node->left->node_key)  //case Left Right
    {  
        node->left = leftRotate(node->left);  
        return rightRotate(node);  
    }  
    
    if (balance < -1 && key < node->right->node_key)   //case right left
    {  
        node->right = rightRotate(node->right);  
        return leftRotate(node);  
    }  
    
    return node;  

}


Tree_node * Avl::rightRotate(Tree_node *node_up){ 

	Tree_node * node_down = node_up->left;  
    Tree_node * node_down_right = node_down->right;  
  
    // Perform right rotation  
    node_down->right = node_up;  
    node_up->left = node_down_right;  
  
    // Update heights  
    node_up->height = max(height(node_up->left), height(node_up->right)) + 1;  
    node_down->height = max(height(node_down->left), height(node_down->right)) + 1;  
  
    // Return new root of subtree 
    return node_down;  

}

Tree_node * Avl::leftRotate(Tree_node *node_up){  

    Tree_node * node_down = node_up->right;  
    Tree_node * node_down_left = node_down->left;  
  
    // Perform left rotation  
    node_down->left = node_up;  
    node_up->right = node_down_left;  
  
    // Update heights  
    node_up->height = max(height(node_up->left), height(node_up->right)) + 1;  
    node_down->height = max(height(node_down->left), height(node_down->right)) + 1;  
  
    // Return new root of subtree
    return node_down;  

}



void Avl::insert_to_avl(int key, Record * rec){ 
    
    root = insert(root, key, rec);  
    if(rec->ENTER_EXIT == "ENTER"){ 
        tree_node_counter++; 
        if(rec->age > 0 && rec->age <= 20) { 
            tree_node_counterA++;
        }
        else if(rec->age > 20 && rec->age <= 40){
            tree_node_counterB++;
        }
        else if(rec->age > 40 && rec->age <= 60) {
            tree_node_counterC++;
        }
        else if(rec->age > 60 && rec->age <= 120) {
            tree_node_counterD++;
        }
        else
        { 
            cout<< "Age is invalid, error occured"<< endl;
            exit(1);
        
        }
    }
}

void Avl::free_post_traversal(Tree_node * root){  

    if(root == NULL) return;
    free_post_traversal(root->left);
    free_post_traversal(root->right);
    delete root;

}



int Avl::get_virus_frequency_rec (Tree_node * root,string & virus_name, int node_key1, int node_key2 ){  //diseaseFrequency 

    if(root == NULL) return 0;
    int c = get_virus_frequency_rec(root->left, virus_name, node_key1, node_key2);
    c += get_virus_frequency_rec(root->right, virus_name, node_key1, node_key2 );
    if(root->node_key >= node_key1 && root->node_key <= node_key2 && virus_name.compare(root->record->disease_name) == 0 && root->record->ENTER_EXIT == "ENTER") return c+1;
    else
        return c;

}



int Avl::get_country_diseasefreq_rec( Tree_node * root, string & disease, int date_key1, int date_key2 ){  //topk_Diseases with dates

    if(root == NULL) return 0;
    int c = get_country_diseasefreq_rec ( root->left, disease, date_key1, date_key2 );
    c += get_country_diseasefreq_rec ( root->right, disease, date_key1, date_key2 );

    if( root->record->disease_name == disease && root->record->date_key >= date_key1 && root->record->date_key <= date_key2 && root->record->ENTER_EXIT == "ENTER"){
        if(root->record->age >= 0 && root->record->age <=20){ 
            A++;
        }
        else if(root->record->age > 20 && root->record->age <=40){
            B++;   
        }
        else if(root->record->age > 40 && root->record->age <=60){
            C++;  
        }

        else if(root->record->age > 60 && root->record->age <=120){
            D++;    
        }
        return c+1;
    }
    else
        return c;

}



int Avl::get_virus_frequency_rec_ENTER (Tree_node * root,string & disease, int date_key1, int date_key2 ){ 


    if(root == NULL) return 0;
    int c = get_virus_frequency_rec_ENTER ( root->left, disease, date_key1, date_key2 );
    c += get_virus_frequency_rec_ENTER ( root->right, disease, date_key1, date_key2 );

    if(root->record->ENTER_EXIT == "ENTER" && root->record->disease_name == disease && root->record->date_key >= date_key1 && root->record->date_key <= date_key2) return c+1;
    else
        return c;

}


int Avl::get_virus_frequency_rec_EXIT (Tree_node * root,string & disease, int date_key1, int date_key2 ){ 

    if(root == NULL) return 0;
    int c = get_virus_frequency_rec_EXIT ( root->left, disease, date_key1, date_key2 );
    c += get_virus_frequency_rec_EXIT ( root->right, disease, date_key1, date_key2 );

    if(root->record->ENTER_EXIT == "EXIT" && root->record->disease_name == disease && root->record->date_key >= date_key1 && root->record->date_key <= date_key2) return c+1;
    else
       return c;

}

