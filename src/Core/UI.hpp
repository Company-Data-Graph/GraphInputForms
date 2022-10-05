#ifndef DATAGRAPHTOOL_CORE_UI_H__
#define DATAGRAPHTOOL_CORE_UI_H__

#include <unordered_map>
#include <string>

#include <Forms/Form.hpp>

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
	std::unordered_map<std::string, Forms::Form*> forms;
};
}  // namespace DataGraph
#endif
