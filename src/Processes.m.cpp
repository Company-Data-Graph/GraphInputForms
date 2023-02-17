#include <core/FormHandler.hpp>

#include "gtest/gtest.h"

int main()
{
#ifdef FORMHANDLER_TESTS
	::testing::InitGoogleTest();

	return RUN_ALL_TESTS();
#endif	// DEBUG

	DataGraph::FormHandler form;
	if (form.init() != 0)
	{
		return -1;
	}

	form.run();

	return 0;
}
