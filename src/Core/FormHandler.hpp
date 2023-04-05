#ifndef DATAGRAPHTOOL_CORE_FORMHANDLER_H__
#define DATAGRAPHTOOL_CORE_FORMHANDLER_H__
#include <string_view>

#include "Logger.hpp"
#include "UI.hpp"

#include <Forms/AuthForm.hpp>

#include <pgfe/pgfe.hpp>

class GLFWwindow;
namespace DataGraph
{
class FormHandler
{
public:
	static constexpr const uint32_t DefaultWidth = 1920;
	static constexpr const uint32_t DefaultHeight = 1080;

	FormHandler(const std::string_view& title = "DataGraphForms",
		 const uint32_t width = DefaultWidth,
		 const uint32_t heigth = DefaultHeight);

	static std::unique_ptr<Logger>& logs();
	static std::unique_ptr<UI>& getUI();
	static std::unique_ptr<dmitigr::pgfe::Connection>& getDbConn();
	static std::unique_ptr<Networking::ConnectionData>& getMedServConn();

	int init(bool resizeAble = true);
	static void close();

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

	static Window m_window;

	static Window& getWindow();
	static WindowData m_data;

	static std::unique_ptr<Logger> loggers;
	static std::unique_ptr<UI> ui;
	static std::unique_ptr<dmitigr::pgfe::Connection> dbConnection;
	static std::unique_ptr<Networking::ConnectionData> medServConn;

	// Could be replaced with some accessors
	friend class UI;
};
}  // namespace DataGraph

#endif
