#include "ProjectInsertForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <set>
#include <list>
#include <vector>

#include <pgfe/transaction_guard.hpp>

#include <Forms/imguidatechooser/imguidatechooser.hPP>

#include <core/FormHandler.hpp>
#include <core/Networking.hpp>

#include <utils/FileManagerDialoge.hpp>
#include <utils/UIModules.hpp>
#include <utils/StringParse.hpp>

namespace DataGraph::Forms
{
int ProjectInsert::init() { return 0; }

int ProjectInsert::draw()
{
	constexpr const uint16_t NewProjectNameWidth = 200;
	constexpr const uint16_t ProjectVersionWidth = 100;
	constexpr const uint16_t ProjectXPosInputWidth = 120;
	constexpr const uint16_t ProjectYPosInputWidth = 120;

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

	if (!m_warningMessage.empty())
	{
		ImGui::TextColored({0.9, 0.9, 0, 1}, "%s", m_warningMessage.data());
	}

	static bool newProject = false;

	ImGui::Text("New project");
	ImGui::Checkbox("##NewProject", &newProject);

	if (!newProject)
	{
		uint32_t prevProjectId = m_projectId;
		utils::inputProjectName(name(), m_projectId, m_projectName, m_projectNameSearch);
		if (m_projectNewName.empty() && prevProjectId != m_projectId)
		{
			m_projectNewName = m_projectName;
		}
	}
	else
	{
		m_projectId = -1;
	}

	ImGui::Text("Project name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(NewProjectNameWidth);
	ImGui::InputText("##ProjectNewName", &m_projectNewName);
	ImGui::PopItemWidth();

	ImGui::Text("Version: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectVersionWidth);
	ImGui::InputText("##Version", &m_projectVersion);
	ImGui::PopItemWidth();

	utils::inputDate(name(), m_date);

	utils::companyComboBox(name(), m_companyId, m_companyPreview, m_companySearch);

	utils::previousProjectVersionCombo(name(), m_projectId, m_projectListName, m_projectListNameSearch, m_nodeId, m_projectListVersion,
		 m_projectListVersionSearch, m_previousVersionsList);

	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &m_projectDesc);

	utils::departmentComboBox(name(), m_departmentsList);

	if (!utils::inputFile(name(), m_filePath, &m_errorMessage))
	{
		m_errorCode = 1;
	}

	if (ImGui::IsItemEdited())
	{
		m_warningMessage = "";
	}

	if (!m_filePath.empty())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, {0.6f, 0.0f, 0.0f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2f, 0.0f, 0.0f, 1.0f});
		ImGui::SameLine();
		if (ImGui::Button("Clear path"))
		{
			m_filePath.clear();
			m_warningMessage = "";
		}
		ImGui::PopStyleColor(2);
	}

	ImGui::Text("Press Release URL: ");
	ImGui::SameLine();
	ImGui::InputText("##PressRelease", &m_pressReleaseURL);

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectXPosInputWidth);
	ImGui::InputInt("##XPOS", &m_x);
	ImGui::PopItemWidth();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectYPosInputWidth);
	ImGui::InputInt("##YPOS", &m_y);
	ImGui::PopItemWidth();

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (!m_projectNewName.empty() && m_companyId != -1 && !m_projectVersion.empty() && !m_projectDesc.empty() && ImGui::Button("Submit"))
	{
		std::string departments = utils::parseDepartments(m_departmentsList);
		std::string dt = utils::parseDate(m_date);
		std::string lastNodes = utils::parseLastNode(m_previousVersionsList);

		try
		{
			dmitigr::pgfe::Transaction_guard guard{*FormHandler::getDbConn()};
			guard.begin();

			uint32_t* dataPtr = &m_errorCode;
			try
			{
				FormHandler::getDbConn()->execute(
					 [dataPtr](auto&& data) {
						 using dmitigr::pgfe::to;
						 *dataPtr = to<int>(data[0]);
					 },
					 "SELECT * FROM addproject(name=>$1, version=>$2, companyid=>$3, description=>$4, iconpath=>NULL, "
					 "date=>$5, projectid=>$6, lastnodeids=>$7, projecttypeids=>$8, pressurl=>$9, posx=>$10, posy=>$11)",
					 m_projectNewName, m_projectVersion, m_companyId, m_projectDesc, dt, static_cast<int>(m_projectId), lastNodes,
					 departments, m_pressReleaseURL, m_x, m_y);
			}
			catch (const std::exception& ex)
			{
				FormHandler::logs()->error("Core", ex.what());
			}
			guard.rollback();
			switch (m_errorCode)
			{
			case 0:
				break;
			case 1:
				m_errorMessage = "Project name is invalid!";
				break;
			case 2:
				m_errorMessage = "Project version is invalid!";
				break;
			case 3:
				m_errorMessage = "Company is invalid!";
				break;
			case 4:
				m_errorMessage = "Previous version contains invalid value!";
				break;
			case 5:
				m_errorMessage = "Departments contains invalid value!";
				break;
			case 6:
				m_errorMessage = "Date is invalid!";
				break;
			case 7:
				m_errorMessage = "Project is invalid!";
				break;
			case 8:
				m_errorMessage = "Project version already exists!";
				break;
			case 9:
				m_errorMessage = "Project version length exceeds maximum length limit 255!";
				break;
			}

			if (m_errorCode == 0)
			{
				Networking::Response response;
				if (!m_filePath.empty())
				{
					auto result = Networking::loadFileToHost(m_filePath, *FormHandler::getMedServConn(), "127.0.0.1", "5555", response);
					if (result != 0 || response.returnCode == 0)
					{
						m_errorCode = 1;
						m_errorMessage = "Something went wrong with media Server, maybe it went down";
					}
				}
				if (!response.body.empty() && response.body.contains("error"))
				{
					m_warningMessage
						 = "Warning: Media server said \" File already exists with that name, please change the name on the different one "
						   "previous version of the file will be uploaded.\"";
				}

				if (response.returnCode == 401)
				{
					m_errorCode = 1;
					m_errorMessage = "Media server login credentials are wrong.";
				}
				else if (!m_filePath.empty() && (response.body.empty() || !response.body.contains("fileName")))
				{
					m_errorCode = 1;
					m_errorMessage = "Ups, something went wrong with upload.";
				}
				else
				{
					std::string filePath;
					if (!m_filePath.empty())
					{
						filePath = fmt::format("http://{}:{}/media-server/data/{}", FormHandler::getMedServConn()->ip,
							 FormHandler::getMedServConn()->port, response.body["fileName"].get<std::string>());
					}

					FormHandler::getDbConn()->execute(
						 [dataPtr](auto&& data) {
							 using dmitigr::pgfe::to;
							 *dataPtr = to<int>(data[0]);
						 },
						 "SELECT * FROM addproject(name=>$1, version=>$2, companyid=>$3, description=>$4, iconpath=>$5, "
						 "date=>$6, "
						 "projectid=>$7, lastnodeids=>$8, projecttypeids=>$9, pressurl=>$10, posx=>$11, posy=>$12)",
						 m_projectNewName, m_projectVersion, m_companyId, m_projectDesc, filePath, dt, static_cast<int>(m_projectId),
						 lastNodes, departments, m_pressReleaseURL, m_x, m_y);

					switch (m_errorCode)
					{
					case 0:
						m_errorMessage = "Value has successfully added!";
						m_projectId = -1;
						m_companyId = -1;
						m_nodeId = -1;
						m_companyPreview.clear();
						m_companySearch.clear();

						m_pressReleaseURL.clear();
						m_projectNewName.clear();
						m_projectName.clear();
						m_projectVersion.clear();
						m_projectDesc.clear();
						m_projectListName.clear();
						m_projectListVersion.clear();
						m_previousVersionsList.clear();

						m_departmentsList.clear();

						m_date.reset();
						break;
					case 1:
						m_errorMessage = "Project name is invalid!";
						break;
					case 2:
						m_errorMessage = "Project version is invalid!";
						break;
					case 3:
						m_errorMessage = "Company is invalid!";
						break;
					case 4:
						m_errorMessage = "Previous version contains invalid value!";
						break;
					case 5:
						m_errorMessage = "Departments contains invalid value!";
						break;
					case 6:
						m_errorMessage = "Date is invalid!";
						break;
					case 7:
						m_errorMessage = "Project is invalid!";
						break;
					case 8:
						m_errorMessage = "Project version already exists!";
						break;
					case 9:
						m_errorMessage = "Project version length exceeds maximum length limit 255!";
						break;
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			FormHandler::logs()->log("Core", ex.what());
		}
	}
	ImGui::PopStyleColor(2);
	return 0;
}

const char* ProjectInsert::name() const { return "Projects"; }

void ProjectInsert::reset() {}

}  // namespace DataGraph::Forms
