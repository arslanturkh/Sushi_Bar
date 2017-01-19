test: Homework2.o 
	g++ Homework2.o -o test -pthread -std=c++11
	
Homework1.o: Homework2.cpp
	g++ -c Homework2.cpp -pthread -std=c++11 
	
clean: 
	rm *.o test
