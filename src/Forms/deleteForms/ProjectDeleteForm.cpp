#include "ProjectDeleteForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <set>
#include <vector>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int ProjectDelete::init() { return 0; }

int ProjectDelete::draw()
{
	static int curProjectId = -1;
	static int curNodeId = -1;

	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	{
		ImGui::Text("Project name: ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##ProjectNameList", m_projectName.c_str()))
		{
			auto* prjName = &m_projectName;
			FormHandler::getDbConn()->execute(
				 [prjName](auto&& data) {
					 using dmitigr::pgfe::to;
					 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
					 if (ImGui::Selectable(dt.second.c_str()))
					 {
						 curProjectId = dt.first;
						 *prjName = std::move(dt.second);
					 }
				 },
				 "SELECT * FROM getProjectListNames($1)", m_projectNameSearch);
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::Text("Project search by name: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(250);
		ImGui::InputText("##ProjectNameSearch", &m_projectNameSearch);
	}

	if (!m_projectName.empty())
	{
		ImGui::Text("Project Version: ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##ProjectVersionList", m_projectVersion.c_str()))
		{
			auto* prjVersion = &m_projectVersion;
			try
			{
				FormHandler::getDbConn()->execute(
					 [prjVersion](auto&& data) {
						 using dmitigr::pgfe::to;
						 std::pair dt{to<int>(data["nodeid"]), std::move(to<std::string>(data["version"]))};
						 if (ImGui::Selectable(dt.second.c_str()))
						 {
							 curNodeId = dt.first;
							 *prjVersion = std::move(dt.second);
						 }
					 },
					 "SELECT * FROM getprojects2(searchprojectid=>$1, namesearch=>$2, searchversion=>$3)", curProjectId, m_projectName,
					 m_projectVersionSearch);
			}
			catch (std::exception& ex)
			{
				std::cout << ex.what();
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::Text("Project version search: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(350);
		ImGui::InputText("##ProjectVersionSearch", &m_projectVersionSearch);
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
			 "SELECT * FROM deleteproject(nodeid=>$1) LIMIT 1", curNodeId);

		switch (m_errorCode)
		{
		case 0:
			m_projectName = "";
			m_projectVersion = "";
			curNodeId = -1;
			curProjectId = -1;
			m_errorMessage = "Value has been successfully deleted";
			break;
		case 1:
			m_projectName = "";
			m_projectVersion = "";
			curNodeId = -1;
			curProjectId = -1;
			m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
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
			uint32_t* errorCode = &m_errorCode;
			FormHandler::getDbConn()->execute(
				 [errorCode](auto&& data) {
					 using dmitigr::pgfe::to;
					 *errorCode = to<int>(data[0]);
				 },
				 "SELECT * FROM deleteproject(nodeid=>$1, force=>TRUE) LIMIT 1", curNodeId);

			switch (m_errorCode)
			{
			case 0:
				m_projectName = "";
				m_projectVersion = "";
				curNodeId = -1;
				curProjectId = -1;
				m_errorMessage = "Value has been successfully deleted";
				break;
			case 1:
				m_projectName = "";
				m_projectVersion = "";
				curNodeId = -1;
				curProjectId = -1;
				m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
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

void ProjectDelete::reset() {}

}  // namespace DataGraph::Forms
