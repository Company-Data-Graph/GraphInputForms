#ifndef DATAGRAPHTOOL_FORMS_TIME_H__
#define DATAGRAPHTOOL_FORMS_TIME_H__
#include <time.h>
namespace DataGraph::utils
{
char* strptime(const char* s, const char* format, struct tm* tm);
}
#endif
