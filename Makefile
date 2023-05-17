all:  L1cahce

clean: 
	rm -rf L1cache 

L1cache: L1cache.c 
	gcc -Wall -Werror -fsanitize=address L1cache.c -o L1cache -lm
