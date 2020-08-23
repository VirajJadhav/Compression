/*
	This file contains functions
	to compress and decompress a file
	using Huffman Algorithm.
*/

#include "head.h"
#include "huffmanCodes.c"
/* 
	function to 
	write compressed codes
	into file 
*/
void compressFile(char *string, char *argv[]) {
	int fdw, add = 0, x = 0, loop = 0;
	int i = 0, j = 0, k = 0, see = 0;
	fdw = open(argv[3], O_RDWR, S_IRUSR | S_IWUSR);
	if(fdw == -1) {
		printf("File not found.\n");
		return;
	}
	/*
		code for bit manipulation
		and writing into file
	*/
	lseek(fdw, 0, SEEK_END);
	add = 0;
	see = 0;
	i = 0;
	while(i < strlen(string)) {
		for(j = 0; j < strlen(store); j++) {
			if(*(string + i) == *(store + j)) {
				for(k = 0; k < j; k++) {
					add = add + code_freq[k];
				}
				/*
					Check if previous and next character
					frequency fits into the 32
					size limit of integer data type
				*/
				if(see + code_freq[j] <= 31) {
					see = see + code_freq[j];
					for(k = add; k < (add + code_freq[j]); k++) {
						x = x<<1;
						x = x | codes[k];
						remember++;
					}
					add = 0;
				}
				/*
					If not then write the stored code
					into file and store next data
				*/
				else if(see + code_freq[j] > 31) {
					k = add;
					loop = 31 - see;
					while(loop) {
						x = x << 1;
						x = x | codes[k++];
						loop--;
					}
					write(fdw, &x, sizeof(int));
					x = 0;
					remember = 0;
					see = (add + code_freq[j]) - k;
					for(k = k; k < (add + code_freq[j]); k++) {
						x = x<<1;
						x = x | codes[k];
						/* 
							Remember the leftover bit's count 
							and write them later
						*/
						remember++;
					}
					add = 0;
				}
			}
		}
		i++;
	}
	/*
		Write the last code saved which
		got left to be written in last
		iteration of loop
	*/
	if(remember) {
		for(int m = 0; m < 31 - remember; m++) {
			x = x<<1;
		}
		write(fdw, &x, sizeof(int));
		write(fdw, &remember, sizeof(int));
	}

	close(fdw);
	printf("File compressed using Huffman Algorithm.\n");
}

/*
	Function to
	write generated tree and
	frequencies into
	compressed file
*/
void writeIntoFile(char alpha[], int freq[], int c, char *argv[]) {
	int fd, i = 0, count;
	fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("File was not created.\n");
		return;
	}
	i = 0;
	/*
		Write the count of characters
		and their frequencies into
		file header
	*/
	count = c*2;
	write(fd, &count, sizeof(int));
	while(c) {
		if(alpha[i] == '0') {
			write(fd, "0", sizeof(char));
		}
		else {
			write(fd, &alpha[i], sizeof(char));
		}
		write(fd, &freq[i], sizeof(int));
		i++;
		c--;
	}

	close(fd); 	
}

/*
	Function to convert
	decimal to binary
*/
int *deci_bin(int deci) {
	static int p[32];
	int i = 0;
	for(int j = 0; j < 32; j++) {
		p[j] = 0;
	}
	while(deci) {
		if(deci & 1) {
			p[i] = 1;
		}
		else {
			p[i] = 0;
		}
		i++;
		deci = deci/2;
	}

	return p;
}

/*
	Function to 
	write decoded information
	into decompressed file
*/
void writeDecode(struct node* root, int *p, int n, char *argv[]) {
	int i;
	static int fd;
	fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("Decompressed File was not created.\n");
		return;
	}
	struct node *newnode = root;
	for(i = 1; i < n; i++) {
		if((i%32) == 0) {
			/* 
				Increment when 
				sign bit is encountered
			*/
			i++;
		}
		if(*(p + i) == 0) {
			// Search left side of tree
			newnode = newnode->left;
		}
		if(*(p + i) == 1) {
			// Search right side of tree
    		newnode = newnode->right;
    	}
    	if(*(p + i) == 2) {
    		break;
		}
		// Check if node is a leaf
	    if(checkleaf(newnode)) {
	    	if(newnode->data == 0) {
				write(fd, "0", sizeof(char));
			}
			else {
				write(fd, &newnode->data, sizeof(char));
			}
			newnode = root;
		}
	}
	close(fd);
}

/*
	Function to start
	decompressing the compressed file
*/
void decodeFile(char *argv[]) {
	int fd, bytes, code, count, i = 0, *p, temp = 0;
	static int j = 0;
	char ch;
	fd = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("File to decompress not found.\n");
		return;
	}
	p = (int *)malloc(sizeof(int)*32);
	if(!p) {
		printf("Failed to allocate memory.\n");
		return;
	}
	read(fd, &code, sizeof(int));
	bytes = code;
	count = bytes/2;
	int freq[count];
	char alpha[count];
	while(count) {
		read(fd, &ch, sizeof(char));
		alpha[i] = ch;
		read(fd, &code, sizeof(int));
		freq[i] = code;
		i++;
		count--;
	}
	/*
		Function call to 
		build the tree again
	*/
	struct node *root = huffman_decode(alpha, freq, bytes/2);
	/* 
		Integer pointer to 
		store the entire file code and 
		process it for decoding
	*/
	int *last = (int *)malloc(sizeof(int)*MAX);
	if(!last) {
		printf("Failed to allocate memory.\n");
		return;
	}
	while(read(fd, &code, sizeof(int))) {
		/*
			function call to 
			convert decimal value to binary
		*/
		p = deci_bin(code);
		for(int k = 31; k >= 0; k--) {
			*(last + j++) = *(p + k);
			*(p + k) = 0;
		}
	}
	if(code) {
		temp = (j - 64) + code + 1;
		for(int m = temp; m < j; m++) {
			*(last + m) = 2;
		}
	}
	writeDecode(root, last, j, argv);

	close(fd);
	printf("File decompressed using Huffman Algorithm.\n");
}

/*
	Function to read the file
	and calculate frequencies
	of occured characters
*/
void readfile_huffman(int fd, char *argv[]) {
	int freq[256], size = 1024, i = 0, j = 0, keep;
	static int count = 0;
	char *string, ch, alpha[256];
	for(i = 0; i < 256; i++) {
		freq[i] = 0;
		alpha[i] = '\0';
	}
	string = (char *)malloc(sizeof(char)*size);
	if(!string) {
		printf("Failed to allocate memory.\n");
		return;
	}
	i = 0;
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
	for(i = 0; *(string + i) != '\0' ; i++) {
		keep = *(string + i);
		alpha[keep] = *(string + i);
		freq[keep]++;
	}
	for(i = 0; i < 256; i++) {
		if(freq[i] > 0)	{
			count++;
		}
	}
	int final_freq[count];
	char final_alpha[count];
	for(i = 0; i < 256; i++) {
		if(alpha[i] != '\0' && freq[i] != 0) {
			final_alpha[j] = alpha[i];
			final_freq[j] = freq[i];
			j++;
		}
	}

	if(strcmp(argv[1], "-c1") == 0) {
		/* 
			Function call to 
			write the tree and 
			frequency into compressed file
		*/
		writeIntoFile(final_alpha, final_freq, count, argv);
		/* 
			Function call for huffman codes
		*/
		huffman_codes(final_alpha, final_freq, count);
		/*
			Function call for writing generated 
			huffman codes into file
		*/
		compressFile(string, argv);
	}
	
	if(strcmp(argv[1], "-uc1") == 0) {
		/*
			Function call to
			start decompression of
			compressed file
		*/
		decodeFile(argv);
	}
}
