#include "AuthForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int AuthForm::init() { return 0; }
int AuthForm::draw()
{
	ImGui::Text("Database ip");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##DatabaseIP", &m_databaseIP);
	ImGui::SameLine(50);
	ImGui::SetCursorPosX(210 + ImGui::CalcTextSize("MediaServer Password").x);

	ImGui::Text("MediaServer ip");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##MediaServerIP", &m_mediaServerIP);

	ImGui::Text("Database port");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##DatabasePort", &m_databasePort);
	ImGui::SameLine(50);
	ImGui::SetCursorPosX(210 + ImGui::CalcTextSize("MediaServer Password").x);

	ImGui::Text("MediaServer port");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##MediaServerPort", &m_mediaServerPort);

	ImGui::Text("Database login");
	ImGui::SameLine();
	ImGui::InputText("##DatabaseLogin", &m_databaseLogin);
	ImGui::SameLine(50);
	ImGui::SetCursorPosX(210 + ImGui::CalcTextSize("MediaServer Password").x);

	ImGui::Text("MediaServer login");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##MediaServerLogin", &m_mediaServerLogin);

	ImGui::Text("Database Password");
	ImGui::SameLine();
	ImGui::InputText("##DatabasePassword", &m_databasePassword, ImGuiInputTextFlags_Password);
	ImGui::SameLine(50);
	ImGui::SetCursorPosX(210 + ImGui::CalcTextSize("MediaServer Password").x);

	ImGui::Text("MediaServer Password");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("##MediaServerPassword", &m_mediaServerPassword, ImGuiInputTextFlags_Password);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0.6, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0, 0.2, 1});
	if (ImGui::Button("Connect"))
	{
		try
		{
			dmitigr::pgfe::Connection_options options;
			if (m_databasePort.empty())
			{
				throw std::invalid_argument("No database port was given");
			}
			options.set(dmitigr::pgfe::Communication_mode::net)
				 .set_hostname(m_databaseIP)
				 .set_port(std::stoi(m_databasePort))
				 .set_database("DataGraph")
				 .set_username(m_databaseLogin)
				 .set_password(m_databasePassword);
			FormHandler::getDbConn().swap(std::make_unique<dmitigr::pgfe::Connection>(options));
			FormHandler::getDbConn()->connect();
			m_errorMessage = "Connected";
			m_errorCode = 0;
		}
		catch (const std::exception& ex)
		{
			m_errorMessage = ex.what();
			m_errorCode = 1;
		}
	}
	if (!m_errorMessage.empty())
	{
		if (m_errorCode == 0)
		{
			ImGui::TextColored({0, 1, 0, 1}, "%s", m_errorMessage.data());
		}
		else
		{
			ImGui::TextColored({1, 0, 0, 1}, "%s", m_errorMessage.data());
		}
	}
	ImGui::PopStyleColor(2);

	return 0;
}

}  // namespace DataGraph::Forms
