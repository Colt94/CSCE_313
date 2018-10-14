/* 
    File: my_allocator.h

    Original Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

*/

#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	bool used;
	uint size;
	BlockHeader *next;
	// 16 bytes

	BlockHeader()
	{
		size = 0;
		used = 0;
		next = NULL;
	}
	
	// decide what goes here
	// hint: obviously block size will go here
};

class LinkedList {
	// this is a special linked list that is made out of BlockHeader s. 
private:

	BlockHeader * head;		// you need a head of the list

public:

	LinkedList()
	{
		head = NULL;
	}

	void insert(BlockHeader* b) {// adds a block to the list

		BlockHeader * temp = b;
		temp->next = head;
		head = temp;

	}

	void remove(BlockHeader* b) {  // removes a block from the list
		BlockHeader *temp = head;
		if (head == b)
			if (head == NULL)
				head = NULL;
			else
				head = head->next;
		else
		{
			while (temp->next != b)
				temp = temp->next;

			temp->next = b->next;
		}
	}

	int nofb() // returns number of free block is free list index
	{
		int c = 0;
		BlockHeader * temp = head;
		while (temp != NULL)
		{
			temp = temp->next;
			c += 1;
		}
		return c;
	}


	bool isempty()
	{
		if (head == NULL)
			return 1;
		else
			return 0;
	}

	BlockHeader * front()
	{
		return head;
	}


	BlockHeader * find(BlockHeader * block)
	{
		BlockHeader * ptr = head;
		while (ptr != block && ptr != NULL)
			ptr = ptr->next;
		return ptr;
	}
};


class BuddyAllocator{
private:
	/* declare member variables as necessary */
	BlockHeader * H;
	char * mem;
	uint basic_block_size;
	uint block_size;
	vector<LinkedList> v;
private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	char* getbuddy(char * addr) {

		if (addr == NULL)
			return NULL;
		BlockHeader * temp = (BlockHeader *)addr;
		int a = addr - mem;
		int m = temp->size;
		char * b = (char*)((a ^ m) + mem);
		return b;
	}
	// given a block address, this function returns the address of its buddy 
	
	bool isvalid(char *addr) {

	}
	// Is the memory starting at addr is a valid block
	// This is used to verify whether the a computed block address is actually correct 

	bool arebuddies(char* block1, char* block2) {

		char * b = getbuddy(block1);
		BlockHeader * buddy = (BlockHeader *)b;
		BlockHeader * buddy2 = (BlockHeader *)block2;
		if (b == block2 && (buddy->size) == (buddy2->size))
			return true;
		else
			return false;
	}
	// checks whether the two blocks are buddies are not

	char* merge(char* block1, char* block2) {

		BlockHeader * temp = (BlockHeader*)block1;

		if (block2 == block1 + temp->size)
		{
			block2 = block2 - temp->size;
			return block2;
		}
		else
		{
			block2 = block1 - temp->size;
			return block2;
		}


	}
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	char* split(char* block) {
		char * ptr;
		BlockHeader *temp = (BlockHeader *)block;
		ptr = block + ((temp->size) / 2);
		return ptr;

	}
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected


public:
	BuddyAllocator (uint _basic_block_size, uint _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(uint _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */ 
   
	void debug ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	128: 5
	256: 0
	512: 3
	1024: 0
	....
	....
	 which means that at point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 
