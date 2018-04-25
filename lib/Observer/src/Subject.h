#ifndef SUBJECT_CLASS
#define SUBJECT_CLASS

#include "Observer.h"

template <class TEventArgs>
class Subject
{
  public:
    void attach(Observer<TEventArgs> *observer)
    {
        observer->next = _observers;
        _observers = observer;
    }
    void deattach(Observer<TEventArgs> *observer)
    {
        //TODO: Method not implemented yet
    }
    void notify(TEventArgs args)
    {
        Observer<TEventArgs> *current = _observers;
        while (current != NULL)
        {
            current->notify(args);
            current = current->next;
        }
    }

  private:
    Observer<TEventArgs> *_observers = NULL;
};

#endif