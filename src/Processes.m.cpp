#include <core/FormHandler.hpp>

#include "gtest/gtest.h"

int main()
{
	DataGraph::FormHandler form;
	if (form.init() != 0)
	{
		return -1;
	}

#ifdef FORMHANDLER_TESTS

	::testing::InitGoogleTest();
	auto result = RUN_ALL_TESTS();
	form.run();

	return result;
#endif	// DEBUG

	form.run();

	return 0;
}
