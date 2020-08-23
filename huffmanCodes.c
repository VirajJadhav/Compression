/*
	This file contains functions
	that generates Huffman codes
	required for compression
	of a file.
*/

#include "head.h"

/*
	Macro for stored
	code's size
*/
#define MAX 10000000

/*
	Data types for
	storing codes and characters
	for further bit manipulation
*/
static int remember = 0;
static int code_freq[256], codes[MAX];
static char store[256];
static int gb = 0, cf = 0, st = 0;

/* 
	A Huffman tree node 
*/
struct node {
    char data; 
    unsigned int freq; 
    struct node *left, *right; 
};

/* 
	A Min Heap -> Collection of min-heap 
	(or Huffman tree) nodes
*/
struct minheap { 
    unsigned int size, capacity;  
    struct node **array;
};

/* 
	Function to allocate a new 
	min heap node with given character 
	and frequency of the character 
*/
struct node *newNode(char data, int freq) {
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    if(!temp) {
		printf("Failed to allocate memory.\n");
		exit(0);
	}
    temp->left = NULL;
    temp->right = NULL;
    temp->data = data;
    temp->freq = freq;

    return temp; 
}

/* 
	Function to create 
 	a min heap of given capacity 
*/
struct minheap *createheap(int capacity) { 
    struct minheap *minheap = (struct minheap *)malloc(sizeof(struct minheap));
    if(!minheap) {
		printf("Failed to allocate memory.\n");
		exit(0);
	}
    minheap->size = 0;
    minheap->capacity = capacity;

    minheap->array = (struct node**)malloc(minheap->capacity * sizeof(struct node*));
    if(!minheap->array) {
		printf("Failed to allocate memory.\n");
		exit(0);
	}

    return minheap;
}

/* 
	Function to 
	swap two min heap nodes
*/
void swapnode(struct node **a, struct node **b) {
    struct node* t = *a;
    *a = *b;
    *b = t;
}

/*
	Function to arrange
	nodes according to
	frequency
*/
void do_minheap(struct minheap *minheap, int index) { 
    int smallest = index; 
    int left = 2*index + 1;
    int right = 2*index + 2;

    if((left < minheap->size) && ((minheap->array[left]->freq) < (minheap->array[smallest]->freq))) {
        smallest = left; 
    }
    if((right < minheap->size) && ((minheap->array[right]->freq) < (minheap->array[smallest]->freq))) {
        smallest = right; 
  	}
  	if(smallest != index) { 
        swapnode(&(minheap->array[smallest]), &(minheap->array[index])); 
        do_minheap(minheap, smallest);
    }
}

/* 
	Function to check 
 	if size of heap 
	is one or not 
 */
int checksize(struct minheap *minheap) { 
    return (minheap->size == 1); 
}

/* 
	Function to extract 
	minimum value node from heap 
*/
struct node *extractminheap(struct minheap *minheap) { 
    struct node *temp = minheap->array[0]; 
    minheap->array[0] = minheap->array[minheap->size - 1]; 
    --(minheap->size);
	/*
		Function call to
		arrange new minheap
	*/
    do_minheap(minheap, 0);

    return temp; 
}

/*
	Function to insert 
	a new node to Min Heap 
*/
void insertminheap(struct minheap *minheap, struct node *minNode) { 
    ++(minheap->size);
    int i = (minheap->size) - 1;
    while (i && minNode->freq < minheap->array[(i - 1) / 2]->freq) { 
        minheap->array[i] = minheap->array[(i - 1) / 2]; 
        i = (i - 1) / 2;
    } 
    minheap->array[i] = minNode; 
}

/* 
	function to build min heap
*/
void buildheap(struct minheap *minheap) { 
    int n = minheap->size - 1; 
    int i;
    for (i = (n - 1) / 2; i >= 0; --i) {
		/*
			Function call to arrange minheap
		*/
		do_minheap(minheap, i);
	}
} 

/* 
	function to check 
	if this node is leaf
*/
int checkleaf(struct node *root) { 
    return (!(root->left) && !(root->right)); 
}

/* 
	Creates a min heap of capacity 
 	equal to size and inserts all character of 
	data[] in min heap. Initially size of 
	min heap is equal to capacity 
*/
struct minheap *assignheap(char data[], int freq[], int size) { 
    struct minheap *minheap = createheap(size); 
    for (int i = 0; i < size; ++i) {
		minheap->array[i] = newNode(data[i], freq[i]);
	}
    minheap->size = size;
	// Function call to build minheap
    buildheap(minheap);

    return minheap; 
}

/*
	Function that builds
	huffman tree
*/
struct node *bulidtree(char data[], int freq[], int size) { 
    struct node *left, *right, *top; 
    struct minheap *minheap = assignheap(data, freq, size);  
    while (!checksize(minheap)) { 
		// Function call to extract minheap
        left = extractminheap(minheap); 
        right = extractminheap(minheap);
		/*
			Allocating a special character '#'
			to differentiate between
			leaf and nodes
		*/
        top = newNode('#', left->freq + right->freq); 
        top->left = left; 
        top->right = right;
		// Function call to insert minheap
        insertminheap(minheap, top); 
    }

    return extractminheap(minheap); 
}

/*
	Function to store character
	frequency codes
	in a array
*/
void writeArr(int arr[], int n) { 
    int i; 
    for (i = 0; i < n; ++i) {
    	codes[gb++] = arr[i];
    }
}

/*
	Function to assign codes
	to characters according 
	to frequency and store
	in a array
*/
void writeCodes(struct node* root, int arr[], int top) { 
    if (root->left) {
		// Assign 0 is left way is present
        arr[top] = 0; 
        writeCodes(root->left, arr, top + 1); 
    }
    if (root->right) {
		// Assign 1 if right way is present
        arr[top] = 1;
        writeCodes(root->right, arr, top + 1); 
    }
    if (checkleaf(root)) {
	   	if(root->data == 0) {
	   		store[st++] = '0';
	   	}
	   	else {
	        store[st++] = root->data;
	    }
    	code_freq[cf++] = top;
		/* 
			Function call to store codes
			of corresponding character
		*/
        writeArr(arr, top); 
    }
}

/* 
	function to call other functions
	such as "buildtree" and "writeCodes"
*/
void huffman_codes(char values[], int freq[], int size) {
    struct node *root = bulidtree(values, freq, size);
    int arr[1000], top = 0;
	/*
		Function call to
		store character and tree
		in a array
	*/
    writeCodes(root, arr, top);
}

/*
	Function to
	decode the codes
	by generating read tree
*/
struct node *huffman_decode(char values[], int freq[], int size) {
	return bulidtree(values, freq, size);
}
