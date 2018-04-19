#include "Arduino.h"
#include "Splitter.h"

Splitter::Splitter(const char *source)
{
  _source = source;
}

void Splitter::skipTo(const char delimiter)
{
  while (_source[current] != '\0' && _source[current] != delimiter)
    current++;

  if (_source[current] != '\0')
    current++;
}

char *Splitter::getNextChunk(const char delimiter, int max_size)
{
  char *buffer = new char[max_size];

  int index = 0;
  while (_source[current] != '\0' && _source[current] != delimiter)
    buffer[index++] = _source[current++];

  buffer[index++] = '\0';
  
  if (_source[current] != '\0')
    current++;

  return buffer;
}