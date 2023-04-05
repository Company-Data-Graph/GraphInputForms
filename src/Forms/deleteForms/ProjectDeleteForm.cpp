#include "ProjectDeleteForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <set>
#include <vector>

#include <Core/FormHandler.hpp>
#include <utils/UIModules.hpp>

namespace DataGraph::Forms
{
int ProjectDelete::init() { return 0; }

int ProjectDelete::draw()
{
	if (!m_errorMessage.empty())
	{
		if (m_errorCode > 0)
		{
			ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
		}
		else
		{
			ImGui::TextColored({0, 0.9, 0, 1}, "%s", m_errorMessage.data());
		}
	}

	uint32_t prevFindProjectId = m_projectId;
	utils::inputProjectName(name(), m_projectId, m_projectName, m_projectNameSearch);
	if (prevFindProjectId != -1 && m_projectId != prevFindProjectId)
	{
		m_nodeId = -1;
		m_projectVersion.clear();
	}

	if (!m_projectName.empty())
	{
		utils::inputProjectVersion(name(), m_projectId, m_projectName, m_nodeId, m_projectVersion, m_projectVersionSearch);
	}

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0, 0, 1});
	if (!m_projectVersion.empty() && ImGui::Button("Submit"))
	{
		uint32_t* errorCode = &m_errorCode;
		FormHandler::getDbConn()->execute(
			 [errorCode](auto&& data) {
				 using dmitigr::pgfe::to;
				 *errorCode = to<int>(data[0]);
			 },
			 "SELECT * FROM deleteproject(nodeid=>$1) LIMIT 1", m_nodeId);

		switch (m_errorCode)
		{
		case 0:
			reset();
			m_errorMessage = "Value has been successfully deleted";
			break;
		case 1:
			reset();
			m_errorMessage = "Invalid value selected, maybe it was removed or updated earlier, please try again.";
			break;
		case 2:
			ImGui::OpenPopup("Help");
			break;
		}
	}

	bool windowOpen = true;

	const ImVec2 center = ImVec2(ImGui::GetWindowWidth() / 2.0f, ImGui::GetWindowHeight() / 2.0f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	const ImVec2 popupSize = ImVec2(500.0f, 300.0f);

	ImGui::SetNextWindowSize(popupSize);

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	if (ImGui::BeginPopupModal("Help", &windowOpen, windowFlags))
	{
		const float buttonOffsetBottomY = 35.0f;
		ImGui::Text(R"(Project had some connections. Are you sure you want to delete it?)");

		ImGui::SetCursorPosY(popupSize.y - buttonOffsetBottomY);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
			windowOpen = false;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(popupSize.x - ImGui::CalcTextSize("Agree").x - ImGui::GetStyle().WindowPadding.x - 5);

		if (ImGui::Button("Agree"))
		{
			try
			{
				uint32_t* errorCode = &m_errorCode;
				FormHandler::getDbConn()->execute(
					 [errorCode](auto&& data) {
						 using dmitigr::pgfe::to;
						 *errorCode = to<int>(data[0]);
					 },
					 "SELECT * FROM deleteproject(nodeid=>$1, force=>TRUE) LIMIT 1", m_nodeId);
			}
			catch (std::exception& ex)
			{
				FormHandler::logs()->error("Core", ex.what());
			}

			reset();
			switch (m_errorCode)
			{
			case 0:
				m_errorMessage = "Value has been successfully deleted";
				break;
			case 1:
				m_errorMessage = "Invalid value selected, maybe it was removed or updated earlier, please try again.";
				break;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(2);
	return 0;
}

const char* ProjectDelete::name() const { return "Projects"; }

void ProjectDelete::reset()
{
	m_projectName = "";
	m_projectNameSearch.clear();
	m_projectVersion = "";
	m_projectVersionSearch.clear();
	m_nodeId = -1;
	m_projectId = -1;
}

std::string_view ProjectDelete::getStatusMessage() const { return m_errorMessage; }

int ProjectDelete::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
