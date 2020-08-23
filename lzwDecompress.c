/*
	This file contains functions
	to decompress a file using 
	LZW Algorithm
*/

#include "head.h"

typedef struct nextDict {
	int prefix, character;
}nextDict;
nextDict dictArray[SIZE];

/*
	Function to add
	Decompressed character and
	prefix values to dictionary
*/
void dictAdd(int previous, int first, int available) {
	dictArray[available].prefix = previous;
	dictArray[available].character = first;
}

/*
	Function to return
	decompressed character value
	from dictionary
*/
int dictChar(int val) {
	return dictArray[val].character;
}

/*
	Function to return
	decompressed prefix value
	from dictionary
*/
int dictPrefix(int val) {
	return dictArray[val].prefix;
}

/*
	Function to read
	compressed file
*/
int readCompress(int fd) {
	int current = 0;
	read(fd, &current, sizeof(char));
	/*
		To null previous
		stored data.
	*/
	current = (current & 255);
	if(current == EOF) {
		return 0;
	}
	if(remaining != 0) {
		current = (remainBit << 8) + current;
		/*
			Indicates no bits
			are left to be read
			from file
		*/
		remaining = 0;
	}
	else {
		int next = 0;
		read(fd, &next, sizeof(char));
		/*
			To null previous
			stored data.
		*/
		next = next & 255;
		remainBit = next & 15;
		/*
			Indicates some bits
			are left to be read
			from file
		*/
		remaining = 1;
		current = (current << 4) + (next >> 4);
	}

	return current;
}

/*
	Function to
	decode codes above
	256 value
*/
int decode(int previous, int fdw) {
	int keep = 0, send = 0;
	if(previous > 255) {
		keep = dictChar(previous);
		/*
			Recursion function call
			to return value of
			prefix character code
		*/
		send = decode(dictPrefix(previous), fdw);
	}
	else {
		keep = previous;
		send = previous;
	}
	write(fdw, &keep, sizeof(char));

	return send;
}

/*
	Function to
	Decompress the
	compressed File
*/
void decodeLZW(char *argv[]) {
	int previous, current, first;
	/*
		next available code 
		in dictionary
	*/
	int available = 256;
	int fd = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("File not found.\n");
		return;
	}
	int fdw = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fdw == -1) {
		printf("Failed to create file.\n");
		return;
	}
	previous = readCompress(fd);
	if(previous == 0) {
		return;
	}
	/*
		write first ASCII character
		directly into file
	*/
	write(fdw, &previous, sizeof(char));

	while((current = readCompress(fd)) > 0) {
		if(current >= available) {
			first = decode(previous, fdw);
			write(fdw, &first, sizeof(char));
		}
		else {
			first = decode(current, fdw);
		}
		
		if(available < SIZE) {
			dictAdd(previous, first, available++);
		}
		previous = current;
	}
	printf("File decompressed using LZW Algorithm.\n");
	
	close(fd);
	close(fdw);
}
