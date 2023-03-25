#include "Integration.hpp"

namespace DataGraph::tests
{
bool s_integrationTestsEnabled = false;
void setIntegrationTests(bool enable) { s_integrationTestsEnabled = enable; }
bool isIntegrationTestsEnabled() { return s_integrationTestsEnabled; }
void IntegratedTest::SetUp()
{
	if (!isIntegrationTestsEnabled())
	{
		GTEST_SKIP() << "Skipping Integration tests";
	}
}
}  // namespace DataGraph::tests
