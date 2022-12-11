#ifndef DATAGRAPHTOOL_FORMS_FORM_H__
#define DATAGRAPHTOOL_FORMS_FORM_H__

namespace DataGraph::Forms
{
class Form
{
public:
	virtual int init() = 0;
	virtual int draw() = 0;
	virtual const char* name() const = 0;
	virtual void reset() = 0;
};
}  // namespace DataGraph::Forms
#endif	// !
