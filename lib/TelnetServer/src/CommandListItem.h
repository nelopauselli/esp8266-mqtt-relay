#ifndef COMMAND_LIST_ITEM_CLASS
#define COMMAND_LIST_ITEM_CLASS

#include "Arduino.h"
#include "Command.h"

class CommandListItem
{
  public:
	CommandListItem(Command *c)
	{
		command = c;
	}
	
	Command *command = NULL;
	CommandListItem *next = NULL;
};

#endif