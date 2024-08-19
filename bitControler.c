#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitControler.h"

/*set bit is a function that set 1 or zero in binary index. value - true mean set 1 and false will set 0. */
void set_bit(int *array, int index, int value) {
    if (value)
        array[index / BITS_PER_INT] |= (1U << (index % BITS_PER_INT));
    else
        array[index / BITS_PER_INT] &= ~(1U << (index % BITS_PER_INT));
}
/*get bit return the value in index bit. for example: we want to get the value in index 3 in this binary number 
...0000 1000 we can see that the value in index 3 is 1 so we will get 1. and we know the number 3 is in our array. */ 
int get_bit(int *array, int index) {
    return (array[index / BITS_PER_INT] >> (index % BITS_PER_INT)) & 1U;
}

void set_bit_in_cell(ram_array array, int cell_index, int bit_index_in_cell, int value) {
    /*Calculate global bit index*/
    int bit_index = cell_index * CELL_SIZE + bit_index_in_cell; 
    /*Set the bit at the calculated index*/
    set_bit(array, bit_index, value);
}

int get_bit_in_cell( ram_array array, int cell_index, int bit_index_in_cell) {
    /*Calculate bit index*/
    int bit_index = cell_index * CELL_SIZE + bit_index_in_cell;
    /*Get the bit at the calculated index*/
    return get_bit(array, bit_index);
}
/*set */
void set_cell_value(ram_array array, int cell_index, int value) {
    int i;
    for (i = 0; i < CELL_SIZE; i++) {
        /*Extract the i-th bit from the value*/
        int bit_value = (value >> i) & 1;
        /*Set bit in the cell*/
        set_bit_in_cell(array, cell_index, i, bit_value);
    }
}
/*return binary cell*/
int get_cell_value(ram_array array, int cell_index) {
    int value = 0;
    int i;
    for (i = 0; i < CELL_SIZE; i++) {
        /*Get the corresponding bit from the cell*/
        int bit_value = get_bit_in_cell(array, cell_index, i);
        /*Set bit in value*/
        value |= (bit_value << i);
    }
    return value;
}
/*return string of binary cell (word) example 000100000011010 to "000100000011010"*/
char* cell_to_binary_string(int value) {
    int i;
    /*Allocate memory for 15 bits + null terminator*/
    char *binary_str = (char*)malloc(CELL_SIZE + 1);
    if (binary_str == NULL) {
        /*Return NULL if memory allocation fails*/
        return NULL; 
    }
    /*Null-terminate the string*/
    binary_str[CELL_SIZE] = '\0';

    for (i = CELL_SIZE - 1; i >= 0; i--) {
        /*Set '1' or '0' based on the bit*/
        binary_str[i] = (value & 1) ? '1' : '0';
        /*Shift the value to process the next bit*/
        value >>= 1;
    }

    return binary_str;
}

/* Sets the bits at index 0 to 3 based on the binary representation of the given value (0 to 15). */
void setOpcodeBit(set array, int value, int startIndex, int endIndex) {
  int i, bit, bitPosition;
  if (endIndex < startIndex)
  {
    return;
  }
  
  for (i = startIndex; i <= endIndex; i++) {
      bitPosition = endIndex - i;
      bit = (value >> bitPosition) & 1;  /* Extract the (3-i)-th bit from value*/
      set_bit(array, i, bit);            /* Set the i-th bit in the array*/
  }
}
