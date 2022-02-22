CPP=g++

objects= worker.o worker_hashtable.o tree.o

SOURCE = worker.cpp worker_hashtable.cpp tree.cpp

HEADER = worker.h worker_hashtable.h tree.h

OUT = worker

FLAGS = -g -c 

all : $(objects)
	$(CPP) -g $(objects) -o $(OUT) 

worker.o : worker.cpp
	$(CPP) $(FLAGS) worker.cpp

worker_hashtable.o : worker_hashtable.cpp
	$(CPP) $(FLAGS) worker_hashtable.cpp

tree.o: tree.cpp
	$(CPP) $(FLAGS) tree.cpp
	
clean:
	rm -f worker $(objects)
