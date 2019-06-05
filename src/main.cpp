#include "Core.h"

int main()
{
	auto& core = hockey::Core::get();
	if (auto ret = core.create(); !ret)
		return ret;

	if (auto ret = core.run(); !ret)
		return ret;

	return 0;
}
