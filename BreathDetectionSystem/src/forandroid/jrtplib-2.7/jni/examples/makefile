all:sample1 sample2

sample1:sample1.cpp
	g++ sample1.cpp -ljrtp -L../ -I../ -g -o sample1

sample2:sample2.cpp
	g++ sample2.cpp -ljrtp -L../ -I../ -g -o sample2

clean:
	rm -f sample1 sample2

