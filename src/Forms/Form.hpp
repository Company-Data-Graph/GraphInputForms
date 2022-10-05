#ifndef DATAGRAPHTOOL_FORMS_FORM_H__
#define DATAGRAPHTOOL_FORMS_FORM_H__

namespace DataGraph::Forms
{
class Form
{
public:
	virtual int init() = 0;
	virtual int draw() = 0;
};
}  // namespace DataGraph::Forms
#endif	// !
