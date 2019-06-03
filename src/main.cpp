#include "Shader.h"
#include "Core.h"

int main()
{
	hockey::Core core;
	if (auto ret = core.create(); !ret)
		return ret;

	if (auto ret = core.run(); !ret)
		return ret;

	return 0;
}
