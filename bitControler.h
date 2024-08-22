#define CELL_SIZE 15
#define NUM_CELLS 4096
/*1920 byte*/
#define ARRAY_SIZE ((NUM_CELLS * CELL_SIZE + BITS_PER_INT - 1) / BITS_PER_INT)  
/*for the most - this number will be 32 (bit)*/
#define BITS_PER_INT (sizeof(int) * 8)
/*len of the array */
#define NUM_INTS ((CELL_SIZE + BITS_PER_INT - 1) / BITS_PER_INT)

typedef  int ram_array[ARRAY_SIZE];
typedef int set[NUM_INTS]; /* Single array to store 15 bits*/

/*set bit is a function that set 1 or zero in binary index. value - true mean set 1 and false will set 0. */
void set_bit(int *array, int index, int value);

/*get bit return the value in index bit. for example: we want to get the value in index 3 in this binary number 
...0000 1000 we can see that the value in index 3 is 1 so we will get 1. and we know the number 3 is in our array. */ 
int get_bit(int *array, int index);
/*enter to specsific cell and set bit */
void set_bit_in_cell(ram_array array, int cell_index, int bit_index_in_cell, int value);
/*enter to spesific cell and get the bit - return 0 or 1*/
int get_bit_in_cell( ram_array array, int cell_index, int bit_index_in_cell);
/*if have a value set the cell to this value*/
void set_cell_value(ram_array array, int cell_index, int value);
/*return cell value*/
int get_cell_value(ram_array array, int cell_index);
/*return string of binary cell (word) example 000100000011010 to "000100000011010"*/
char* cell_to_binary_string(int value);
/* Sets the bits at spesific range based on the binary representation of the given value (0 to 15). */
void setOpcodeBit(set array, int value, int startIndex, int endIndex);