#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

#include "debug_console.h"


namespace Debug
{
	Console::Console()
	{
		AllocConsole();
	}
	Console::~Console()
	{
		if (this->active)
		{
			this->active = false;
			FreeConsole();
		}
	}

	bool Console::IsActive()
	{
		return this->IsActive();
	}

	bool Console::SetActive(bool active)
	{
		this->active = active;
		if (this->active)
		{

		}
		else
		{

		}

		return this->active;
	}

	bool Console::ClearContext()
	{

	}
	std::string Console::GetContextAll()
	{
		std::string context{""};

		return context;
	}
}