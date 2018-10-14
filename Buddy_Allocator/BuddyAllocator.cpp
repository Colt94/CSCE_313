/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
	
	mem = new char[_total_memory_length*sizeof(char)];

	block_size = _total_memory_length;
	basic_block_size = _basic_block_size;

	H = (BlockHeader*)mem;
	H->size = _total_memory_length;
	H->used = 0;

	uint length = (log2(block_size)-log2(basic_block_size)) + 1;
	v.resize(length);

	v[0].insert(H);
	
}

BuddyAllocator::~BuddyAllocator (){

	delete [] mem;
}

char* BuddyAllocator::alloc(uint _length) {

	uint request = _length + sizeof(BlockHeader);
	int count = 0;
	int index = 0;
	if (request > block_size)
		return NULL;
	else if (request < basic_block_size)
	{
		index = (v.size() - 1);
	}
	else
	{
		request = ceil(log2(request));
		uint pow2 = pow(2, request);
		index = log2((block_size / pow2));
	}
	if (v[index].isempty())
	{
		if (index == 0)
			return NULL;
		if (v[index - 1].isempty() == 0)
		{
			BlockHeader * p = v[index - 1].front();
			v[index - 1].remove(p);
			char *next = split((char*)p);
			BlockHeader * next_block = (BlockHeader *)next;
			next_block->size = (p->size / 2);
			p->size = (p->size / 2);
			next_block->used = 0;
			p->used = 0;
			v[index].insert(next_block);
			v[index].insert(p);
		}
		else
		{
			for (int i = 0; i < index; i++)
			{
				if (v[i].isempty() == 0)
				{

						BlockHeader * p = v[i].front();
						v[i].remove(p);
						char *next = split((char*)p);
						BlockHeader * next_block = (BlockHeader *)next;
						next_block->size = (p->size / 2);
						p->size = (p->size / 2);
						next_block->used = 0;
						p->used = 0;
						v[i + 1].insert(next_block);
						v[i + 1].insert(p);
						count += 1;
				
				}
				
			}
			if (count == 0)
				return NULL;  //If it cant find blocks to split then no more memory available
		}

	}

	BlockHeader * ptr = v[index].front();
	v[index].remove(ptr);
	ptr->used = 1;
	return ((char*)ptr + sizeof(BlockHeader));
	


		/* This preliminary implementation simply hands the call over the
		   the C standard library!
		   Of course this needs to be replaced by your implementation.
		*/
	
}

int BuddyAllocator::free(char* _a) {
  /* Same here! */
  
	char * block = _a - sizeof(BlockHeader);
	BlockHeader * freed = (BlockHeader *)block;
	int index = log2((block_size / freed->size));
	freed->used = 0;
	v[index].insert(freed);

	
	for (int i = index; i > 0; i--)
	{
		if (v[i].nofb() >= 2)
		{
	
			BlockHeader * block1 = v[i].front();
			char * buddy = getbuddy((char*)block1);
			BlockHeader *B = (BlockHeader *)buddy;
			if (B->used == 0 && arebuddies(buddy,(char*)block1))
			{
				v[i].remove(block1);
				v[i].remove(B);
				char * merged_b = merge((char*)block1, buddy);
				BlockHeader * merged_h = (BlockHeader *)merged_b;
				merged_h->size = (B->size) * 2;
				merged_h->used = 0;
				v[i - 1].insert((BlockHeader *)merged_b);
			}
			else
				break;
			
		}
		else
			break;
	}
 
  return 0;
}

void BuddyAllocator::debug (){

	int size = block_size;
	for (int i = 0; i < v.size(); i++)
	{
		cout << "Block Size:  " << size << "  Free Blocks:  " << v[i].nofb() << endl;
		size = size / 2;
	}
}

