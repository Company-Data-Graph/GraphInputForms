#ifndef DATAGRAPHTOOL_CORE_FORMHANDLER_H__
#define DATAGRAPHTOOL_CORE_FORMHANDLER_H__

#include <string_view>

#include "Logger.hpp"
#include "UI.hpp"

#include <pgfe/pgfe.hpp>

class GLFWwindow;
namespace DataGraph
{
class FormHandler
{
public:
	static constexpr const uint32_t defaultWidth = 1920;
	static constexpr const uint32_t defaultHeight = 1080;

	FormHandler(const std::string_view& title = "DataGraphForms",
		 const uint32_t width = defaultWidth,
		 const uint32_t heigth = defaultHeight);

	static std::unique_ptr<Logger>& logs();
	static std::unique_ptr<UI>& getUI();
	static std::unique_ptr<dmitigr::pgfe::Connection>& getDbConn();

	int init(bool resizeAble = true);
	void run();

	struct WindowData
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		bool closed = false;
		bool minimized = false;
	};

	~FormHandler();

private:
	struct Window
	{
		GLFWwindow* window;
	};

	static Window window;

	static Window& getWindow();
	WindowData data;

	static std::unique_ptr<Logger> loggers;
	static std::unique_ptr<UI> ui;
	static std::unique_ptr<dmitigr::pgfe::Connection> dbConnection;

	friend class UI;
};
}  // namespace DataGraph

#endif
