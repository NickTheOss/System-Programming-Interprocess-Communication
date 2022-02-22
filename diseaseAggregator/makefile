CPP=g++

objects= diseaseAggregator.o #worker_hashtable.o tree.o

SOURCE = diseaseAggregator.cpp #worker_hashtable.cpp tree.cpp

HEADER = diseaseAggregator.h# worker_hashtable.h tree.h

OUT = diseaseAggregator

FLAGS = -g -c 

all : $(objects)
	$(CPP) -g $(objects) -o $(OUT) 

diseaseAggregator.o : diseaseAggregator.cpp
	$(CPP) $(FLAGS) diseaseAggregator.cpp

#WorkerHashtable.o : worker_hashtable.cpp
	#$(CPP) $(FLAGS) worker_hashtable.cpp

#tree.o: tree.cpp
	#$(CPP) $(FLAGS) tree.cpp
	
clean:
	rm -f diseaseAggregator $(objects)
