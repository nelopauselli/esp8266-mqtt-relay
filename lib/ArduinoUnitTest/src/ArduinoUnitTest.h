#ifndef ARDUINO_UNIT_TESTS_CLASS
#define ARDUINO_UNIT_TESTS_CLASS

class Assert_t
{
    int _fails = 0;
    int _success = 0;

  public:
    void areEqual(char *expected, char *actual);

    void summary();
};

extern Assert_t Assert;

#endif