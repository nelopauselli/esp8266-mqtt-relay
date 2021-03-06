#ifndef OBSERVER_CLASS
#define OBSERVER_CLASS

template <class TEventArgs>
class Observer
{
  public:
	virtual void notify(TEventArgs eventArgs) = 0;
	const char *id;
	Observer *next;
};

#endif