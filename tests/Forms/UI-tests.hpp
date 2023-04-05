#pragma once
#include <string_view>
#include <vector>

struct ImGuiTestEngine;
struct ImGuiTestLog;
enum ImGuiTestStatus;

namespace DataGraph::tests
{
void enableUITests();
bool isUITestsEnabled();

void setupWindowParams();

void setWindowVisibility(bool isVisible);

void registerImguiTests(ImGuiTestEngine* testEngine);

void enableAutoTests();
bool isAutoTestsEnabled();

struct ImguiTestStatus
{
	std::string_view category;
	std::string_view name;
	ImGuiTestStatus status;

	uint64_t duration;

	ImGuiTestLog* logBuffer;
};
std::vector<ImguiTestStatus> parseUITests();
}  // namespace DataGraph::tests
