#include "FormHandler.hpp"

#include <cassert>
#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Logger.hpp"
#include "UI.hpp"

#include <stb_image.h>

#include <filesystem>
#ifdef __has_include(<windows.h)
#include <windows.h>
#else
#include <unistd.h>
#include <cstdlib>
#endif

#ifdef FORMHANDLER_TESTS
#include <Forms/UI-tests.hpp>
#endif	// FORMHANDLER_TESTS

namespace DataGraph
{
std::unique_ptr<Logger> FormHandler::loggers;
std::unique_ptr<UI> FormHandler::ui;
FormHandler::Window FormHandler::m_window;
FormHandler::WindowData FormHandler::m_data;
std::unique_ptr<dmitigr::pgfe::Connection> FormHandler::dbConnection;
std::unique_ptr<Networking::ConnectionData> FormHandler::medServConn;

std::unique_ptr<Logger>& FormHandler::logs() { return FormHandler::loggers; }
std::unique_ptr<UI>& FormHandler::getUI() { return FormHandler::ui; }
std::unique_ptr<dmitigr::pgfe::Connection>& FormHandler::getDbConn() { return FormHandler::dbConnection; }
std::unique_ptr<Networking::ConnectionData>& FormHandler::getMedServConn() { return FormHandler::medServConn; }

void glfwCallbackFunction(int glfwErrorCode, const char* description)
{
	FormHandler::logs()->error(
		 "Core", fmt::format(fmt::fg(fmt::color::red), "Window API error occured with Error code {0} it could mean \"{1}\".", glfwErrorCode,
					  description));
}

void setWorkingDirectory()
{
	constexpr const uint16_t MaxPathLength = 2048;
#ifdef __has_include(<windows.h>)
	wchar_t path[MaxPathLength] = {0};
	GetModuleFileNameW(NULL, path, MaxPathLength);
	std::wstring wpath(path);
#else
	char result[MaxPathLength];
	ssize_t count = readlink("/proc/self/exe", result, MaxPathLength);
	std::wstring wpath;
	wpath.resize(count, '\0');
	mbstowcs(wpath.m_data(), result, count);
#endif

	std::filesystem::current_path(std::filesystem::path(wpath).remove_filename().parent_path().parent_path().parent_path().parent_path());
}

FormHandler::FormHandler(const std::string_view& title, const uint32_t width, const uint32_t height)
{
	m_data.title = title;
	m_data.width = width;
	m_data.height = height;
}

int FormHandler::init(bool resizeAble)
{
	setWorkingDirectory();

	// Initialize Winsock
	WSADATA wsa_data;
	int sockResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (sockResult != 0)
	{
		FormHandler::logs()->error("Core", "Error initializing Winsock: {}", sockResult);
		return -1;
	}

	// Set up the STARTUPINFO structure
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	FormHandler::loggers = std::make_unique<Logger>();
	auto result = loggers->init();
	assert(result == 0);

	FormHandler::logs()->log("Core", "Window initialization has begun.");

	glfwSetErrorCallback(glfwCallbackFunction);

	auto apiResult = glfwInit();
	assert(apiResult);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, resizeAble);
#ifdef FORMHANDLER_TESTS
	tests::setupWindowParams();
#endif	// FORMHANDLER_TESTS

	m_window.window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
	if (!m_window.window)
	{
		FormHandler::logs()->error("Core", "Window initialization has failed.");
		return -2;
	}

	glfwSetWindowUserPointer(m_window.window, &m_data);

	glfwSetWindowSizeCallback(m_window.window, [](GLFWwindow* window, int width, int height) {
		WindowData* windowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

		windowData->height = height;
		windowData->width = width;

		if (width == 0 || height == 0)
		{
			windowData->minimized = true;
			return;
		}

		windowData->minimized = false;
	});

	glfwSetWindowCloseCallback(m_window.window, [](GLFWwindow* window) {
		WindowData* windowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		windowData->closed = true;
	});

	GLFWimage icon{};
	icon.pixels = stbi_load("resources/icons/AppIcon.png", &icon.width, &icon.height, nullptr, STBI_rgb_alpha);
	assert(icon.pixels != nullptr);

	glfwSetWindowIcon(m_window.window, 1, &icon);
	stbi_image_free(icon.pixels);

	glfwMakeContextCurrent(m_window.window);

	FormHandler::logs()->log("Core", "Window initialization has ended successfully.");
	assert(result == 0);

	ui = std::make_unique<UI>();
	result = ui->init();
	assert(result == 0);

	return 0;
}

void FormHandler::close() { m_data.closed = true; }

void FormHandler::run()
{
	while (!m_data.closed)
	{
		if (!m_data.minimized)
		{
			ui->beginFrame();
			ui->endFrame();
		}

		glfwSwapBuffers(m_window.window);
		glfwPollEvents();
	}
}

FormHandler::~FormHandler()
{
	if (m_window.window)
	{
		glfwTerminate();
	}

	WSACleanup();
}

FormHandler::Window& FormHandler::getWindow() { return m_window; }
}  // namespace DataGraph
