all:  first

clean: 
	rm -rf first 

first: first.c 
	gcc -Wall -Werror -fsanitize=address first.c -o first -lm