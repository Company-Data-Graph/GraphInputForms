#ifndef DATAGRAPHTOOL_NETWORKING_BACKEND_INTEGRATION_H__
#define DATAGRAPHTOOL_NETWORKING_BACKEND_INTEGRATION_H__

#include <gtest/gtest.h>

namespace DataGraph::tests
{
class IntegratedTest: public ::testing::Test
{
	void SetUp() override;
};

void setIntegrationTests(bool enable);
bool isIntegrationTestsEnabled();
}  // namespace DataGraph::tests

#define TEST_INTEGRATED_F(base_fixture, test_name)                                                 \
	class test_name##_##base_fixture: public base_fixture, public DataGraph::tests::IntegratedTest \
	{};                                                                                            \
	TEST_F(test_name##_##base_fixture, test_name)

#define TEST_INTEGRATED(base_fixture, test_name)                              \
	class test_name##_##base_fixture: public DataGraph::tests::IntegratedTest \
	{};                                                                       \
	TEST_F(test_name##_##base_fixture, test_name)

#endif
