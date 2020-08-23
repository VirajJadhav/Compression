/*
	This file contains functions
	to compress a file using 
	LZW Algorithm
*/

#include "head.h"

/*
	Maximum allowed
	dictionary size
*/ 
#define SIZE 4096

typedef struct dict {
	int val, previous, character;
	struct dict *next;
}dict;

dict *head, *tail;
/* 
	Globals to write 
	character code into file
*/
int remaining = 0, remainBit;

/*
	function to add
	newnodes (dictionary elements)
	as a linked list
*/
void addDict(dict *newnode) {
	if(!head && !tail) {
		head = tail = newnode;
		return;
	}
	tail->next = newnode;
	tail = newnode;
}

/*
	function to 
	initialize ASCII dictionary
*/
void initdict() {
	int i = 0;
	for(i = 0; i < 256; i++) {
		dict *newnode = (dict *)malloc(sizeof(dict));
		if(!newnode) {
			printf("Memory was not alloted.\n");
			break;
		}
		newnode->character = i;
		newnode->previous = -1;
		newnode->next = NULL;
		addDict(newnode);
	}
}

/*
	function to find
	character avaliability
	in dictionary
*/
int findDict(int current, int next) {
	dict *newnode = head;
	while(newnode != NULL) {
		if((newnode->previous == current) && (newnode->character == next)) {
			// For non-ASCII character codes
			return newnode->val;
		}
		newnode = newnode->next;
	}
	/*
		 returns one for
		ASCII characters in dictionary
	*/
	return -1;
}

/*
	Function to add new
	character codes to
	dictionary
*/
void addNewDict(int current, int next, int available) {
	dict *newnode = (dict *)malloc(sizeof(dict));
	if(!newnode) {
		printf("Memory was not alloted.\n");
		return;
	}
	newnode->previous = current;
	newnode->character = next;
	newnode->val = available;
	newnode->next = NULL;
	/*
		Append this node
		to the dictionary
	*/
	addDict(newnode);
}

/*
	Function to write the
	character code into
	compressed file
*/
void writeFile(int fd, int current) {
	if(remaining != 0) {
		int temp = (remainBit << 4) + (current >> 8);
		write(fd, &temp, sizeof(char));
		write(fd, &current, sizeof(char));
		/* 
			Indicates no bits
		    left to be written
			into file
		*/
		remaining = 0;
	}
	else {
		int temp = current >> 4;
		remainBit = current & 15;
		/* 
			Indicates some bits
		    are yet to be written
			into file
		*/
		remaining = 1;
		write(fd, &temp, sizeof(char));
	}
}

/*
	function to
	start compression using
	LZW algorithm
*/
void compress_lzw(char *string, char *argv[]) {
	int i = 0, fd;
	fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("File was not created.\n");
		return;
	}
	int current, next, check, available = 256;
	current = *(string + i++);
	if(current == EOF) {
		return;
	}
	/*
		Function call to initialize
		ASCII dictionary and add them
		as a linked list
	*/
	initdict();

	while(*(string + i) != '\0') {
		next = *(string + i);
		/* 
			function call 
			to check character in dictionary
		*/
		check = findDict(current, next);
		if(check != -1) {
			current = check;
		}
		else {
			/*  
				Function call
				write character code 
				into compressed file
			*/
			writeFile(fd, current);
			/*
				Function call to add
				current and next character
				into dictionary
			*/
			if(available < SIZE) {
				addNewDict(current, next, available++);
			}
			current = next;
		}
		i++;
	}
	/* 	
		Function call to 
		write the last character code 
		which got left in last iteration
	*/
	writeFile(fd, current);

	if(remaining != 0) {
		int temp = remainBit << 4;
		write(fd, &temp, sizeof(char));
	}
	printf("File compressed using LZW Algorithm.\n");

	close(fd);
}

/*
	Function to
	read the input file and store
	it in a string
*/
char *readfile_lzw(int fd) {
	int i = 0, size = 1024;
	char ch;
	static char *string;
	string = (char *)malloc(sizeof(char)*size);
	if(!string) {
		printf("Memory was not alloted.\n");
		exit(0);
	}
	while(read(fd, &ch, sizeof(char))) {
		*(string + i++) = ch;
		if(i == size) {
			size *= 2;
			string = (char *)realloc(string, size);
			if(!string) {
				printf("Memory was not alloted.\n");
				break;
			}
		}
	}
	*(string + i) = '\0';

	return string;
}
