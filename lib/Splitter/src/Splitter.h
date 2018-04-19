#ifndef SPLITTER_CLASS
#define SPLITTER_CLASS

class Splitter
{
  public:
    Splitter(const char *source);
    char *getNextChunk(const char delimiter, int max_size = 50);
    void skipTo(const char delimiter);

  private:
  const char*_source;
  int current=0;
};

#endif