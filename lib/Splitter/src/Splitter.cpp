#include "Arduino.h"
#include "Splitter.h"

Splitter::Splitter(const char *source)
{
  _source = source;
}

char *Splitter::getNextChunk(const char delimiter, int max_size)
{
  char *buffer = new char[max_size];

  int index = 0;
  while (_source[current] != '\0' && _source[current] != delimiter)
    buffer[index++] = _source[current++];
  
  buffer[index++] = '\0';
  current++;

  return buffer;
}