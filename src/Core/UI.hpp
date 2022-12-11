#ifndef DATAGRAPHTOOL_CORE_UI_H__
#define DATAGRAPHTOOL_CORE_UI_H__

#include <list>

#include <Forms/Form.hpp>

#include <Forms/AuthForm.hpp>

namespace DataGraph
{
class UI
{
public:
	int init();
	void beginFrame();
	void endFrame();

	~UI();

private:
	Forms::AuthForm auth;

	std::list<Forms::Form*> forms;
};
}  // namespace DataGraph
#endif
