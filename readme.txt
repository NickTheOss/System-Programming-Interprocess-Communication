The folder contains a total of 4 subfolders: 

i) diseaseAggregator: Contains a source code file (diseaseAggregator.cpp), a header file (diseaseAggregator.h), it's makefile and a file logfile_xxx with the statistics that the parent collects where xxx = his pid. It is the main application for the management of viruses and the parent of the processes that "generates" which will manage the questions. We will return to it's functionality later after we have completed the explanation of the content of the remaining files.

ii) fifos: This is where the named pipes (used by the parent-worker pair for their inter-branch communication) are created. 

iii) test_dir: Contains subfolders with countries and date files that were used to check the functionality of the program before the bash script was built. It contains 8 countries with 1-2 subfolders each. It was also used to make valid record control functions (e.g. if a record has only EXIT status or ENTER date is later than EXIT) where in this case the program terminates by displaying a corresponding message.

iv) worker: Contains 3 source code files (worker.cpp, worker_hashtable.cpp, tree.cpp), 3 header files (worker.h, worker_hashtable.h, tree.h) and it's makefile. It is the child-worker process that the diseaseAggregator "gives birth" to, and returns to him the data requested. (It's function will be analyzed below). 

Finally, the folder contains the bash-script "create_infiles.sh" which creates the folder input_dir in countries and files of any size given by the user are stored. Indicatively, the countriesFile and diseaseFile files contain some names.

A) diseaseAggregator: diseaseAggregator.cpp---> The application starts by making a single linked list from all the countries that are in the user's import folder. (line 1155) Then, it makes a check on whether the number of countries given is less than the number of workers. If this is the case, then it makes the number of workers equal to the number of countries in order not to create unnecessarily "children" who do not perform any work. Then the disperse_countries function (line 850) distributes evenly to each worker the countries that belong to him, making for each one his own single linked list.Then the function make_and_open_fifos is called (row 907) where the fifos are created in the folder we analyzed above for each worker. The create_workers() then makes an execl( line 953) and creates the worker processes that will send back the desired data. Then we open the fifos for writing using the open_fifo_for_write function(line 935) and update the workers' table with the additional information of the file_descriptors. Since we have carried out all the necessary actions for fifos, it's time to pass on the countries to the workers.With the send_countries_to_workers function (row 890), we receive for each worker a string in the following format-> country1_path $country2_path$... countryn_path, which we pass on to him through fifo. Here in order for there to be a continuous flow of explanation let's open the worker folder and then the employee source file worker.cpp

worker.cpp ---> So we have stayed in the situation where the parent has sent the countries to the worker. On line 593 there is a handler to know when we will end the process. In the init function (line 126) we place in a structure the arguments that the parent has transmitted through exec, the file_descriptors and the buffer_size. Then we reserve space for each hashtable because we will scatter the records on an avl tree. Therefore we need a hashtable for the countries with size = the number of countries, a hashtable for the diseases that conventionally we have given the max 10 as a size and a table of hashtables of the disease type that we will need for the statistics since each date file has a group of diseases. Finishing with these initializations, we go to the receiving_countries_from_fifo function (line 340) which we call in the main() function immediately after the init(). After creating the fifos for reading and writing, we keep in the variable buffer, the data that the parent has sent through the pipeline (the countries he will take over).Because strtok processes the buffer, we keep a copy of it in a buffer2. We first call an strtok function with the "$" separator to find the number of countries that this worker has taken over. Then we create a table of strings that in each cell we place the pathname of each country to "feed" the function access_country (row 301). The access_country takes the folder of a country and after opening it, gives as an argument to the readFile function (row 147) each subfolder-date that the country contains.
ReadFile opens each file, inserts each country with each file into a list (in order to send them to the pipeline and be properly separated later) and for each record-line of the file calls the function read_file_line (row 186). This updates the corresponding Record structure and then inserts it into the corresponding structures and hashtables. So we return to row 407 and call the print_statistics function (row 220). First we reserve the necessary space needed in a buffer and cross each node of the list of countries and files to display the requested statistics for each file. In the worker_hashtable.cpp line 104 we calculate the cases per category (in the tree.cpp line 286 we kept the cases per age category in the correct meter). When we finish the initial buffer it will hold a string with the stats that the worker has to return. We return to the worker.cpp module on line 409 where we write the data in the pipeline in order to be read by the worker. The function ends by making an additional list of a pid country pair needed for the first query and releasing the memory that is not needed. On line 596 the entry_type_check() checks the validity of a record and is implemented in tree.cpp (line 56). We now return to the diseaseAggregator.cpp module.

diseaseAggregator.cpp---> line 1162 read_from_all_workers(line 1049). Since the worker has written data in the pipeline (the statistics) the time has come for the parent to read them and print them. Before explaining the function of the above function, we will analyze below, the functions read_from_fifo (line 976) and write_to_fifo (line 1011) that form the basis of the interprofessional communication we implement.

write_to_fifo(): Takes an input-string as an argument and initially calculates it's length. Then it writes in the conductor how many bytes ir writes, separated by "@" (conventionally after @there are "garbage" that we do not want to be read) so that the reader knows what to read, and the content of the string it writes.

read_from_fifo(): It takes as an argument a filedescriptor to clarify by whom it is reading. It first reads how many bytes will be read and keeps them in the buffer variable. However, the buffer may also contain "garbage" so we make a strtok with delimiter the "@" to read exactly what we want. After capturing memory as much as length, we initialize a p-pointer that points at the beginning of the input. If the bytes we will read are smaller than buffer_size, we keep in the fifo_buffer_size the correct value. Then we enter a while loop to read a packet of bytes each time. If we read 1000 bytes at a time and the memory-packet contains 2030, we will read 30 last time. This is what the check on the line 1000-1001 does. Finally we return what we read.

We go back to line 1049. Now we have to read the statistics. But here we have another problem to deal with. How do we regularly read the data sent by workers? In the beginning we call the set_worker_flags (line 1026) that initializes the flags of the workers in "true" value since they all have data to send. After some checks we slowly read the data that has been written in the pipeline by each worker and as soon as we finish we print it on the screen. In row 1164 we call the function input_command (implemented on row 154). In the implementation that has been done, the parent is responsible for sending correct questions to the employees, and therefore the one who carries out the checks. At the end of the program only one file is printed by the worker for statistics and the workers who always get correct files obviously do not print anything. Perhaps it is more efficient that way so  the processes with unnecessary controls will not be overloaded since the father makes them once per question. Let's go to line 164 in order to write the questions in the pipeline and get an answer by calling the function read_from_all_workers.
There is another variation of the read_from_all_workers (line 1088) used for the queries /diseaseFrequency and /searchPatientRecord to print the results aggregated and not per worker. E.g. If one worker is responsible for germany, another for italy and we want cases of H1N1 be printed cumulatively and not eg worker1 = 5, worker2 = 7. This is done on line 1111 and similarly for the other question. Another differentiation is the questions we ask as an argument and a country. We need to find out which worker has taken the relevant country first. This is done on line 832. Returns the position on the workers table to send the appropriate file_descriptor to the fifo writer. The calls of the queries are made with the following order:

/listCountries - line 175 
/diseaseFrequency - line 302,317 
/topK-AgeRanges - line 457 
/searchPatienRecord - line 498 
/numPatientAdmissions - line 604, 621 
/numPatientDischarges - line 731, 744 
/exit - line 763.

When /exit is given, the parent sends a signal for the workers to finish, and then the fifos and file_descriptor that have been created are deleted. In any other case, an error is printed. We return one last time to the worker folder and the worker.cpp.

worker.cpp---> On line 599, the worker enters a never-ending loop and waits to read a query. Once it reads it then goes to line 431. Note that the parent sends the queries in string1$string$... format. For example if given the query "/diseaseFrequency H1N1 05-05-2010 06-06-2020 greece" the worker will receive something of the form "/diseaseFrequency$H1N1$key1$key2$greece$"" where key1,key2 are the dates that have been converted by the parent through the type key = (year-1900) * 365 + 30 * month + day.So depending on the first argument that will be broken with the strtok function, it goes to the appropriate if case and calls the competent function from the worker_hashtable.cpp except for the query /listCountries that crosses the list in line 66 and the       /searchPatientRecord that calls the function to the tree.cpp line 96. Then he writes the data in the pipeline and the parent reads it at the other end which prints it. Once it receives a signal to finish, it terminates and prints it's id.

All the above constitute the basic idea of the work as it was implemented. Here are brief descriptions of the header files:

diseaseAggregator folder:
diseaseAggregator.h---> It includes the necessary libraries that we need for the implementation of the application and the corresponding structures. In line 38 the worker_list is the list of countries, the worker_info is each worker with his information and arguments = the arguments given by the command line.

employee folder: 
worker.h---> It includes the necessary libraries that we need for the implementation of the worker and the corresponding structures. It has the lists we make for statistics and the first query, the Record structure with a patient's information and the arguments where he gets the information from the parent. 

worker_hashtable.h: This includes the statements of the functions implemented in the worker_hashtable.cpp along with the corresponding structures (from the bottom up).

struct HashTable: Table of lists of type Bucket_list. It also contains it's size. 

struct Bucket_list: The list of buckets(The highest interface level ). A head and tail pointer of type Bucket_node (which is the node of the list). In addition it contains the number of nodes and the corresponding size. 

struct Bucket_node: The node of the list that contains a pointer at the next node and of course a pointer in the bucket(bucket) table.

struct Bucket : A table where each cell has the data inside. Contains the index that is the next position in the table that is empty, the size of the table and a marker in the Bucket_item.

struct Bucket_item: Bucket data. It contains the name (of the disease or country depending on the hashtable we use) and a marker on the avl tree to have their required connection. 

struct sort_table: Needed for query /topK-AgeRanges in the classification. 

tree.h: 
list_avl.h: This contains the statements of the functions used in the list_avl.cpp along with the corresponding structures (+ constructors, destructors where appropriate).

struct List_node: The node of the list of records, which contains the record and one pointer to the next. 

struct Data_list: The list of records. Contains a counter with the number of nodes and 2 head-tail markers List_node.

struct Tree_node: The node of the tree. It contains the key date_key, a pointer in record, it's height and 2 pointers to each child. 

Struct Avl: The structure of the tree. Contains a Tree_node type marker and the number of nodes.

Command to compile: 'make' and 'make clean' to clean the object files of the program (In the diseaseAggregator folder and in the worker folder) 

Execution command from the command line with arguments: "./diseaseAggregator -w numWorkers -b bufferSize -i input_dir "
Execution command from the command line without arguments: "./diseaseAggregator". In this case, default values are assigned.
Input file: 'test_dir.txt' or 'input_dir' if the bash-script is executed.

B) Bash-Script: It is located in the main folder named "create_infiles.sh". Initially, after stating the functions that we will need in order to create the folders, in line 74-100 it performs debugging checks, so that we always get correct inputs from the user. It then fills the tables and folders with the data and writes records in each file.

Execution Order: "./create_infiles diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"
where diseasesFile = diseases filename
countriesFile = countries filename
input_dir = the folder where the subfolders will be written 
numFilesPerDirectory = how many files each country folder will have inside 
numRecordsPerFile = how many records each file will have inside.

Before execution we must type the command "chmod 0777 create_infiles.sh" 
Implementation Language:C++ 
Implementation Environment: Linux
