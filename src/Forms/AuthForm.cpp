#include "AuthForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Networking/Networking.hpp>
#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int AuthForm::init() { return 0; }

int AuthForm::draw()
{
	constexpr const uint16_t inputWidth = 200;

	auto& style = ImGui::GetStyle();
	float itemIdent = style.ColumnsMinSpacing + style.ItemSpacing.x;
	float databaseInputOffset = itemIdent + ImGui::CalcTextSize("Database Password").x;
	float mediaServerTextOffset = databaseInputOffset + inputWidth + itemIdent;
	float mediaServerInputOffset = mediaServerTextOffset + itemIdent + ImGui::CalcTextSize("MediaServer Password").x;

	ImGui::PushItemWidth(inputWidth);

	ImGui::Text("Database ip");
	ImGui::SameLine(databaseInputOffset);
	ImGui::InputText("##DatabaseIP", &m_database.ip);

	ImGui::SameLine(mediaServerTextOffset);
	ImGui::Text("MediaServer ip");
	ImGui::SameLine(mediaServerInputOffset);
	ImGui::InputText("##MediaServerIP", &m_mediaServer.ip);

	ImGui::Text("Database port");
	ImGui::SameLine(databaseInputOffset);
	ImGui::InputText("##DatabasePort", &m_database.port);

	ImGui::SameLine(mediaServerTextOffset);
	ImGui::Text("MediaServer port");
	ImGui::SameLine(mediaServerInputOffset);
	ImGui::InputText("##MediaServerPort", &m_mediaServer.port);

	ImGui::Text("Database login");
	ImGui::SameLine(databaseInputOffset);
	ImGui::InputText("##DatabaseLogin", &m_database.login);

	ImGui::SameLine(mediaServerTextOffset);
	ImGui::Text("MediaServer login");
	ImGui::SameLine(mediaServerInputOffset);
	ImGui::InputText("##MediaServerLogin", &m_mediaServer.login);

	ImGui::Text("Database Password");
	ImGui::SameLine(databaseInputOffset);
	ImGui::InputText("##DatabasePassword", &m_database.password, ImGuiInputTextFlags_Password);

	ImGui::SameLine(mediaServerTextOffset);
	ImGui::Text("MediaServer Password");
	ImGui::SameLine(mediaServerInputOffset);
	ImGui::InputText("##MediaServerPassword", &m_mediaServer.password, ImGuiInputTextFlags_Password);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0.6, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0, 0.2, 1});
	if (ImGui::Button("Connect"))
	{
		try
		{
			FormHandler::getDbConn().release();

			Networking::Response response;
			bool success = Networking::connectToMediaServer(m_mediaServer, response);
			if (!success && response.returnCode == 0)
			{
				m_errorMessage = "Invalid Media Server ip or port!";
				m_errorCode = 2;
			}
			else if (response.returnCode != 200)
			{
				m_errorMessage = "Media Server credentials are invalid!";
				m_errorCode = 3;
			}
			else
			{
				dmitigr::pgfe::Connection_options options;
				if (m_database.port.empty())
				{
					throw std::invalid_argument("No database port was given");
				}

				options.set(dmitigr::pgfe::Communication_mode::net)
					 .set_hostname(m_database.ip)
					 .set_port(std::stoi(m_database.port))
					 .set_database("datagraph")
					 .set_username(m_database.login)
					 .set_password(m_database.password);
				FormHandler::getDbConn() = std::make_unique<dmitigr::pgfe::Connection>(options);
				FormHandler::getDbConn()->connect();
				FormHandler::getMedServConn() = std::make_unique<Networking::ConnectionData>(m_mediaServer);

				m_errorMessage = "Connected";
				m_errorCode = 0;
			}
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
	ImGui::PopItemWidth();

	return 0;
}

}  // namespace DataGraph::Forms
