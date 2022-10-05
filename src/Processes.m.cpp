#include <core/FormHandler.hpp>

int main()
{
	DataGraph::FormHandler form;
	if (form.init() != 0)
	{
		return -1;
	}

	form.run();

	return 0;
}
