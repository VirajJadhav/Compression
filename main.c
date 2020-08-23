/*
	C project for compression of file using Huffman algorithm and LZW algorithm.
*/

/*
	This file contains the main function
	that calls other compression/decompression functions
	of the mentioned algorithms.
*/

#include "head.h"
#include "huffman.c"
#include "lzwCompress.c"
#include "lzwDecompress.c"

void syntax() {
	printf("\nFollow below syntax to Compress / Decompress a file.\n\n");
	printf("For Huffman File Compression And Decompression :-\n1. Compression ->\n\t <compiled file name>  <-c1>  <filename to compress>  <compressed file name>\n");
	printf("2. Decompression ->\n\t<compiled file name>  <-uc1>  <compressed file name>  <filename to decompress>\n\n");
	printf("For LZW File Compression And Decompression :-\n1. Compression ->\n\t <compiled file name>  <-c2>  <filename to compress>  <compressed file name>\n");
	printf("2. Decompression ->\n\t<compiled file name>  <-uc2>  <compressed file name>  <filename to decompress>\n\n");
}

int main(int argc, char *argv[]) {
	int fd;
	if(argc != 4) {
		syntax();
		exit(0);
	}
	fd = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("File not found.\n");
		exit(0);
	}
	if((strcmp(argv[1], "-c1") == 0) || (strcmp(argv[1], "-uc1") == 0)) {
		/*
			Function call for huffman compression
			and decompression algorithm
		*/
		readfile_huffman(fd, argv);
	}
	else if((strcmp(argv[1], "-c2") == 0) || (strcmp(argv[1], "-uc2") == 0)) {
		char *string = readfile_lzw(fd);
		if(strcmp(argv[1], "-c2") == 0) {
			/*
				Function call for
				LZW compression algorithm
			*/
			compress_lzw(string, argv);
		}
		else {
			/*
				Function call for
				LZW decompression algorithm
			*/
			decodeLZW(argv);
		}
	}
	else {
		syntax();
	}
	
	close(fd);
  
    return 0; 
}
