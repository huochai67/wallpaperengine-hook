#include "hijack.h"

bool hijack_load()
{
	return Load() && Init();
}