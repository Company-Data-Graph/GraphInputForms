#ifndef DATAGRAPHTOOL_CORE_FORMHANDLER_H__
#define DATAGRAPHTOOL_CORE_FORMHANDLER_H__

#include <string_view>

#include "Logger.hpp"
#include "UI.hpp"

#include <Forms/AuthForm.hpp>

#include <Windows.h>
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
	static std::unique_ptr<Networking::ConnectionData>& getMedServConn();

	int init(bool resizeAble = true);
	void run();

	struct WindowData
	{
		std::string title;
		uint32_t width = -1;
		uint32_t height = -1;

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
	static std::unique_ptr<Networking::ConnectionData> medServConn;

	PROCESS_INFORMATION m_proxy;

	friend class UI;
};
}  // namespace DataGraph

#endif
