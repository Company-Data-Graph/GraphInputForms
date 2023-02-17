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

	return RUN_ALL_TESTS();
#endif	// DEBUG

	form.run();

	return 0;
}
