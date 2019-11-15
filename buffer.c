#include "buffer.h"

#include <string.h>
#include <assert.h>

linkedListItem * createItems(int indexSize, int size)
{
  linkedListItem * head;
  head = (linkedListItem *)malloc(sizeof(linkedListItem));
  head->data = (void *)malloc(indexSize);
  head->available = 0;

  linkedListItem * current = head;

  for(int i = 0; i < size-1; i++)
  {
    current->next = (linkedListItem *)malloc(sizeof(linkedListItem));
    current = current->next;
    current->data = (void *)malloc(indexSize);
    current->available = 0;
  }

  current->next = head;
  return head;
}

buffer * initBuffer(int size, int indexSize)
{
  buffer * b = (buffer *)malloc(sizeof(buffer));
  b->indexSize = indexSize;
  b->writePointer = createItems(indexSize, size);
  b->readPointer = b->writePointer;
  b->first = b->writePointer;
  b->size = size;
  return b;
}

uint8_t bufferFull(buffer * b)
{
  return b->writePointer->available;
}

void writeBuffer(buffer * b, void * data)
{
  #ifdef errchk
  assert(!b->writePointer->available); //Make use of bufferFull to avoid this assert.
  #endif

  memcpy(b->writePointer->data, data, b->indexSize);
  b->writePointer->available = 1;
  b->writePointer = b->writePointer->next;
}

uint8_t bufferAvailable(buffer * b)
{
  return b->readPointer->available;
}

void * readBuffer(buffer * b)
{
  #ifdef errchk
  assert(b->readPointer->available); //Make use of bufferAvailable to avoid this assert.
  #endif

  linkedListItem * temp;
  temp = b->readPointer;
  //b->readPointer->available = 0; //Comment out, to loop over the elements
  b->readPointer = b->readPointer->next;

  return temp->data;
}

void freeBuff(buffer * b)
{
  linkedListItem * temp;
  for(int i = 0; i < b->size; i++)
  {
    temp = b->readPointer;
    b->readPointer = b->readPointer->next;
    free(temp);
  }
}
