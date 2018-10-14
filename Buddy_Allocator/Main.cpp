#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char ** argv) {

  //int basic_block_size = 128, memory_length = 134217728;

  // create memory manager
  int option;
  int basic_block_size;
  int memory_length;
  int bflag = 0;
  int cflag = 0;
  
  while((option = getopt(argc,argv,"b:s:")) != -1)
  {
      switch(option){
        case 'b':
              basic_block_size = atoi(optarg);
              bflag = 1;
              break;
        case 's':
              memory_length = atoi(optarg);
              cflag = 1;
              break;
              
        default:
              break;
              
      }
  }
  
  if(bflag == 0)
    basic_block_size = 128;
  if(cflag == 0)
    memory_length = 512;
  cout << "Block size: " << basic_block_size << endl;
  cout << "Memory length: " << memory_length << endl;

  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
  //BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
  //char * ptr;
  //char * ptr2;
  //ptr = allocator->alloc(26210);
  //ptr = allocator->alloc(26210);
  //ptr = allocator->alloc(26210);
  //ptr = allocator->alloc(26210);
  //ptr = allocator->alloc(10000);
  //ptr2 = allocator->alloc(10000);
  //int m = allocator->free(ptr);
  //m = allocator->free(ptr2);
 

 //allocator->debug();
  // test memory manager
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test. 
  
  // destroy memory manager
  delete allocator;

  //system("pause");
}
