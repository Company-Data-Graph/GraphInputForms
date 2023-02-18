#include <core/FormHandler.hpp>

#include "gtest/gtest.h"

int main(int argc, char** argv)
{
	DataGraph::FormHandler form;
	if (form.init() != 0)
	{
		return -1;
	}

#ifdef FORMHANDLER_TESTS

	::testing::InitGoogleTest(&arc, argv);
	auto result = RUN_ALL_TESTS();

	return result;
#endif	// DEBUG

	form.run();

	return 0;
}
