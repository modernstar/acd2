#pragma once

#include <sstream>

class Logging{
private:

public:
	Logging();
	~Logging();

    static void log(const char *st,...);
};


