#ifndef buffer_h
#define buffer_h

#include <stdlib.h>
#include <stdint.h>

#define errchk //Comment this to avoid error check with assert.
//Not recommended, cause buffer items could be overwritten.

typedef struct
{
  void * next;
  void * data;
  uint8_t available;
} linkedListItem;

typedef struct
{
  linkedListItem * first;
  linkedListItem * writePointer;
  linkedListItem * readPointer;
  int indexSize;
  int size;
} buffer;

buffer * initBuffer(int size, int indexSize);
uint8_t bufferFull(buffer * b);
void writeBuffer(buffer * b, void * data);
uint8_t bufferAvailable(buffer * b);
void * readBuffer(buffer * b);
void freeBuff();

#endif
