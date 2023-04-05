#include <core/FormHandler.hpp>

#ifdef FORMHANDLER_TESTS
#include <Networking/backend/Unit-tests.hpp>
#include <Networking/backend/Integration.hpp>
#include <Forms/UI-tests.hpp>
#include <unordered_map>
#include <functional>
#include <string>
#endif

#include "gtest/gtest.h"

int main(int argc, char** argv)
{
#ifdef FORMHANDLER_TESTS

	std::string unitTestServerPort = "44852";
	std::unordered_map<std::string_view, std::function<void(std::string_view)>> argHandlers
		 = {{"--integration-tests",
				 [](std::string_view arg) {
					 DataGraph::tests::setIntegrationTests(true);
				 }},
			  {{"--UnitTestServerPort"},
				   [&](std::string_view arg) {
					   unitTestServerPort = arg;
				   }},
			  {{"--UI-tests"},
				   [](std::string_view arg) {
					   DataGraph::tests::enableUITests();
					   if (arg == "Auto")
					   {
						   DataGraph::tests::enableAutoTests();
					   }
				   }},
			  {{"--enableViewPort"}, [](std::string_view arg) {
				   DataGraph::tests::setWindowVisibility(true);
			   }}};
	for (int i = 1; i < argc; i++)
	{
		std::string_view arg(argv[i]);
		size_t equalPos = arg.find('=');
		std::string_view key = arg.substr(0, equalPos);
		if (auto it = argHandlers.find(key); it != argHandlers.end())
		{
			std::string_view value = "";
			if (equalPos != std::string_view::npos)
			{
				value = arg.substr(equalPos + 1);
			}

			it->second(value);
		}
	}
#endif

	DataGraph::FormHandler form;
	if (form.init() != 0)
	{
		return -1;
	}

#ifdef FORMHANDLER_TESTS
	if (DataGraph::tests::isUITestsEnabled())
	{
		form.run();
	}

	::testing::InitGoogleTest(&argc, argv);

	::testing::AddGlobalTestEnvironment(new DataGraph::tests::ConnectionServer(unitTestServerPort));


	return RUN_ALL_TESTS();
#endif

	form.run();

	return 0;
}
