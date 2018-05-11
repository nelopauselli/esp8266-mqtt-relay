#ifndef SUBJECT_CLASS
#define SUBJECT_CLASS

#include "Observer.h"
#include "Arduino.h"
#include "Logger.h"


template <class TEventArgs>
class Subject
{
  public:
    void attach(const char *id, Observer<TEventArgs> *observer)
    {
        deattach(id);
        
        observer->id = id;
        observer->next = _observers;
        _observers = observer;
    }
    void deattach(const char *id)
    {
        Observer<TEventArgs> *current = _observers;
        Observer<TEventArgs> *previous = NULL;

        while (current != NULL)
        {
            if (strcmp(current->id, id) == 0)
            {
                DEBUG("Deattach ");
                DEBUG(current->id);

                if (previous == NULL) // if it is the first
                {
                    DEBUG(" [first]");

                    _observers = current->next;
                    delete current;
                    current = _observers;
                }
                else
                {
                    DEBUG(" [no first]");

                    previous->next = current->next;
                    delete current;
                    current = previous->next;
                }
                DEBUGLN();
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }
    void notify(TEventArgs args)
    {
        DEBUG("notifing: ");
        Observer<TEventArgs> *current = _observers;
        while (current != NULL)
        {
            DEBUG("\t");
            DEBUGLN(current->id);

            current->notify(args);
            current = current->next;
        }
    }

  private:
    Observer<TEventArgs> *_observers = NULL;
};

#endif