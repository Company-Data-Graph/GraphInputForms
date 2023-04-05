#ifndef DATAGRAPHTOOL_CORE_UI_H__
#define DATAGRAPHTOOL_CORE_UI_H__

#include <list>
#include <string_view>

#include <Forms/Form.hpp>

#include <Forms/AuthForm.hpp>

#ifdef FORMHANDLER_TESTS
#include <Forms/UI-tests.hpp>
#endif	// FORMHANDLER_TESTS

namespace DataGraph
{
class UI
{
public:
	int init();
	void beginFrame();
	void endFrame();

	std::string_view getStatusMessage();
	int getStatusCode();

	~UI();

private:
	Forms::AuthForm m_auth;

#ifdef FORMHANDLER_TESTS
	ImGuiTestEngine* m_engine;

	friend std::vector<tests::ImguiTestStatus> tests::parseUITests();
#endif	// FORMHANDLER_TESTS

	std::string_view m_activeForm;
	std::list<Forms::Form*> m_forms;
};	// namespace class UI
}  // namespace DataGraph
#endif
