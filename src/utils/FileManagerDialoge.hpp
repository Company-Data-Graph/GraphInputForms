#ifndef DATAGRAPHTOOL_FORMS_FILEMANAGERDIALOGE_H__
#define DATAGRAPHTOOL_FORMS_FILEMANAGERDIALOGE_H__
#include <string>

namespace DataGraph::utils
{
bool openFile(std::string& filePath);

#ifdef FORMHANDLER_TESTS
extern std::string s_openFileResult;
#endif	// FORMHANDLER_TESTS

}
#endif
