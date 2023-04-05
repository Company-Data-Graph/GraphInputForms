#include "ProjectUpdateForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <Forms/imguidatechooser/imguidatechooser.hPP>

#include <Core/FormHandler.hpp>
#include <Networking/Networking.hpp>
#include <utils/FileManagerDialoge.hpp>
#include <utils/UIModules.hpp>
#include <utils/StringParse.hpp>

#include <utils/Time.hpp>

namespace DataGraph::Forms
{
int ProjectUpdate::init() { return 0; }

int ProjectUpdate::draw()
{
	constexpr const uint16_t ProjectNameInputWidth = 250;
	constexpr const uint16_t ProjectVersionInputWidth = 100;
	constexpr const uint16_t ProjectXPosInputWidth = 120;
	constexpr const uint16_t ProjectYPosInputWidth = 120;
	constexpr const uint16_t SetVersionPosWidth = 120;

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

	uint32_t prevFindProjectId = m_findProjectId;
	utils::inputProjectName(name(), m_findProjectId, m_findProjectName, m_findProjectNameSearch);
	if (prevFindProjectId != -1 && m_findProjectId != prevFindProjectId)
	{
		m_findProjectVersionId = -1;
		m_findProjectVersion.clear();
	}

	uint32_t prevFindProjectVersionId = m_findProjectVersionId;
	if (!m_findProjectName.empty())
	{
		utils::inputProjectVersion(
			 name(), m_findProjectId, m_findProjectName, m_findProjectVersionId, m_findProjectVersion, m_findProjectVersionSearch);
	}

	if (m_findProjectVersionId == -1)
	{
		return 0;
	}

	if (m_findProjectVersionId != -1 && prevFindProjectVersionId != m_findProjectVersionId)
	{
		auto projectNamePtr = &m_projectName;
		auto projectNewVersionPtr = &m_projectNewVersion;
		auto projectDescPtr = &m_projectDesc;
		auto companyIdPtr = &m_companyId;
		auto pressReleaseURLPtr = &m_pressReleaseURL;
		auto iconPathPtr = &m_iconPath;
		auto x = &m_x;
		auto y = &m_y;
		auto departmentsPtr = &m_departmentsList;
		auto previousVersionsPtr = &m_previousVersionsList;
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 *projectNamePtr = std::move(to<std::string>(data["name"]));
				 *projectNewVersionPtr = std::move(to<std::string>(data["version"]));
				 *projectDescPtr = std::move(to<std::string>(data["description"]));
				 *companyIdPtr = std::move(to<int>(data["companyid"]));
				 *iconPathPtr = std::move(to<std::optional<std::string>>(data["url"]));
				 *pressReleaseURLPtr = std::move(to<std::string>(data["pressurl"]));
				 m_parentProjectId = to<int>(data["projectid"]);
				 *x = to<int>(data["posx"]);
				 *y = to<int>(data["posy"]);

				 previousVersionsPtr->clear();
				 std::vector<short> prevVers = std::move(to<std::vector<short>>(data["previousversions"]));
				 for (auto& it : prevVers)
				 {
					 previousVersionsPtr->emplace(it, "");
				 }

				 departmentsPtr->clear();
				 std::vector<short> departments = std::move(to<std::vector<short>>(data["projecttypes"]));
				 std::vector<std::string> departmentsNames = std::move(to<std::vector<std::string>>(data["projecttypenames"]));
				 for (uint32_t i = 0; i < departments.size(); ++i)
				 {
					 departmentsPtr->emplace(std::move(departmentsNames[i]), departments[i]);
				 }

				 std::optional<std::string> incDate = std::move(to<std::optional<std::string>>(data["date"]));
				 if (incDate.has_value())
				 {
					 tm date;
					 utils::strptime(incDate.value().c_str(), "%Y-%m-%d", &date);
					 m_date = date;
				 }
			 },
			 "SELECT * FROM getprojects(searchnodeid=>$1)", m_findProjectVersionId);
		m_parentProjectName = m_projectName;

		auto companyName = &m_companyName;
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 *companyName = std::move(to<std::string>(data["name"]));
			 },
			 "SELECT * FROM getcompanies(companyid=>$1)", m_companyId);

		for (auto& it : m_previousVersionsList)
		{
			FormHandler::getDbConn()->execute(
				 [&it](auto&& data) {
					 using dmitigr::pgfe::to;
					 it.second = fmt::format("{}:{}", to<std::string>(data["name"]), to<std::string>(data["version"]));
				 },
				 "SELECT * FROM getprojects(searchnodeid=>$1)", it.first);
		}

		resetVersionOrderList();
	}

	static bool newProject = false;
	ImGui::Text("Mark as new project");
	ImGui::Checkbox("##NewProject", &newProject);

	if (!newProject)
	{
		ImGui::Text("Parent");
		ImGui::SameLine(0, -2);

		uint32_t prevId = m_parentProjectId;
		utils::inputProjectName("Parent project", m_parentProjectId, m_parentProjectName, m_parentProjectNameSearch);
		if (prevId != m_parentProjectId)
		{
			resetVersionOrderList();
			m_versionOrderList.emplace_back(m_findProjectVersion);
			m_projectVersionIndex = m_versionOrderList.size();
			m_projectVersionOrderIndex = m_projectVersionIndex;
		}
	}

	ImGui::Text("Project Name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectNameInputWidth);
	ImGui::InputText("##ProjectName", &m_projectName);
	ImGui::PopItemWidth();

	ImGui::Text("Version: ");
	ImGui::PushItemWidth(ProjectVersionInputWidth);
	ImGui::SameLine();
	ImGui::InputText("##Version", &m_projectNewVersion);
	ImGui::PopItemWidth();

	utils::inputDate(name(), m_date);

	utils::companyComboBox(name(), m_companyId, m_companyName, m_companySearchName);

	utils::previousProjectVersionCombo("Previous project versions", m_projectListId, m_projectListName, m_projectListNameSearch,
		 m_projectListVersionId, m_projectListVersion, m_projectListVersionSearch, m_previousVersionsList);

	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &m_projectDesc);

	utils::departmentComboBox(name(), m_departmentsList);

	static bool iconDisabled = false;
	if (iconDisabled)
	{
		ImGui::BeginDisabled();
	}

	if (!utils::inputFile(name(), m_newIconPath, &m_errorMessage))
	{
		m_errorCode = 1;
	}

	if (ImGui::IsItemEdited())
	{
		m_warningMessage = "";
	}

	if (!m_newIconPath.empty())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, {0.6f, 0.0f, 0.0f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2f, 0.0f, 0.0f, 1.0f});
		ImGui::SameLine();
		if (ImGui::Button("Undo path change"))
		{
			m_newIconPath.clear();
			m_warningMessage = "";
		}
		ImGui::PopStyleColor(2);
	}

	if (iconDisabled)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	ImGui::Checkbox("##UnknownIconPath", &iconDisabled);

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

	if (!newProject && m_versionOrderList.size() > 1
		 && ImGui::BeginTable("Project Version List", 2,
			  ImGuiTableFlags_Borders | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit
				   | ImGuiTableFlags_NoHostExtendX,
			  {500, 0}))
	{
		ImGui::TableSetupColumn("Pos");
		ImGui::TableSetupColumn("Project name");
		ImGui::TableHeadersRow();

		for (uint32_t i = 0; i < m_versionOrderList.size(); ++i)
		{
			auto& version = m_versionOrderList[i];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%d", i + 1);
			if (m_projectVersionIndex - 1 == i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32({0.5f, 0.1f, 0.1f, 0.65f}));
			}
			else if (m_projectVersionOrderIndex - 1 == i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32({0.5f, 0.5f, 0.1f, 0.65f}));
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::Text(version.c_str());
			if (m_projectVersionIndex - 1 == i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32({0.5f, 0.1f, 0.1f, 0.65f}));
			}
			else if (m_projectVersionOrderIndex - 1 == i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32({0.5f, 0.5f, 0.1f, 0.65f}));
			}
		}
		ImGui::EndTable();

		ImGui::SameLine();
		ImGui::Text("Move to");

		int tempIndex = m_projectVersionOrderIndex;
		ImGui::PushItemWidth(SetVersionPosWidth);
		ImGui::SameLine();
		ImGui::InputInt("##SetPos", &tempIndex, 0);
		m_projectVersionOrderIndex = std::clamp(tempIndex, 0, (int)m_versionOrderList.size());
		ImGui::PopItemWidth();

		ImGui::PushStyleColor(ImGuiCol_Button, {0.0f, 0.0f, 0.6f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.0f, 0.0f, 0.2f, 1.0f});
		if (m_projectVersionOrderIndex != m_projectVersionIndex)
		{
			ImGui::SameLine();
			if (ImGui::Button("Move"))
			{
				for (uint32_t i = 0; i < m_versionOrderList.size(); ++i)
				{
					if (i == m_projectVersionIndex - 1)
					{
						int8_t iteration = m_projectVersionIndex > m_projectVersionOrderIndex ? -1 : 1;
						uint32_t j = i;
						uint32_t nextJ = j + iteration;
						while (nextJ != m_projectVersionOrderIndex - 1)
						{
							std::swap(m_versionOrderList[j], m_versionOrderList[nextJ]);

							j += iteration;
							nextJ += iteration;
						}
						std::swap(m_versionOrderList[j], m_versionOrderList[nextJ]);

						m_projectVersionIndex = m_projectVersionOrderIndex;
						m_projectVersionIndexChanged = true;
						break;
					}
				}
			}
		}
		ImGui::PopStyleColor(2);

		if (m_projectVersionIndexChanged)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, {0.6f, 0.0f, 0.0f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2f, 0.0f, 0.0f, 1.0f});
			if (ImGui::Button("Reset"))
			{
				resetVersionOrderList();
			}

			ImGui::PopStyleColor(2);
		}
	}


	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});
	if (ImGui::Button("Update"))
	{
		m_warningMessage = "";

		std::string departments = utils::parseDepartments(m_departmentsList);
		std::string lastNodes = utils::parseLastNode(m_previousVersionsList);
		std::string dt = utils::parseDate(m_date);
		int prjId = newProject ? -1 : m_parentProjectId;
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
					 "SELECT * FROM updateproject(id=>$1, name=>$2, description=>$3, version=>$4, companyid=>$5, projectid=>$6, "
					 "projectversionindex=>$7, date=>$8, lastnodeids=>$9, projecttypeids=>$10, url=>$11, pressurl=>$12, posX=>$13, "
					 "posy=>$14)",
					 m_findProjectVersionId, m_projectName, m_projectDesc, m_projectNewVersion, m_companyId, prjId, m_projectVersionIndex,
					 dt, lastNodes, departments, m_iconPath.value_or(""), m_pressReleaseURL, m_x, m_y);
			}
			catch (const std::exception& ex)
			{
				FormHandler::logs()->error("Core", ex.what());
			}

			switch (m_errorCode)
			{
			case 0:
				m_findProjectVersion = m_projectNewVersion;
				m_findProjectName = m_projectName;
				m_errorMessage = "Value has successfully updated!";
				break;
			case 1:
				m_errorMessage = "Project is invalid!";
				break;
			case 2:
				m_errorMessage = "Project name is invalid!";
				break;
			case 3:
				m_errorMessage = "Company is invalid!";
				break;
			case 4:
				m_errorMessage = "Parent project is invalid!";
				break;
			case 5:
				m_errorMessage = "Version already exists!";
				break;
			case 7:
				m_errorMessage = "Project can't have it's self as previous project!";
				break;
			case 8:
				m_errorMessage = "Project order is invalid!";
				break;
			case 9:
				m_errorMessage = "Department list has invalid value!";
				break;
			case 11:
				m_errorMessage = "Project version length exceeds maximum length limit 255!";
				break;
			case 12:
				m_errorMessage = "Date is invalid!";
				break;
			}

			if (m_errorCode != 0)
			{
				guard.rollback();
			}
			else if (m_errorCode == 0 && m_newIconPath.empty())
			{
				guard.commit();
				resetVersionOrderList();
			}
			else if (m_errorCode == 0 && !m_newIconPath.empty())
			{
				guard.rollback();
				Networking::Response response;

				auto result = Networking::loadFileToHost(m_newIconPath, *FormHandler::getMedServConn(), response);
				if (result != 0 || response.returnCode == 0)
				{
					m_errorCode = 1;
					m_errorMessage = "Something went wrong with media Server, maybe it went down";
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
				else if (!m_newIconPath.empty() && (response.body.empty() || !response.body.contains("fileName")))
				{
					m_errorCode = 1;
					m_errorMessage = "Ups, something went wrong with upload.";
				}
				else
				{
					std::string filePath = filePath = fmt::format("http://{}:{}/media-server/data/{}", FormHandler::getMedServConn()->ip,
						 FormHandler::getMedServConn()->port, response.body["fileName"].get<std::string>());

					FormHandler::getDbConn()->execute(
						 [dataPtr](auto&& data) {
							 using dmitigr::pgfe::to;
							 *dataPtr = to<int>(data[0]);
						 },
						 "SELECT * FROM updateproject(id=>$1, name=>$2, description=>$3, version=>$4, companyid=>$5, projectid=>$6, "
						 "projectversionindex=>$7, date=>$8, lastnodeids=>$9, projecttypeids=>$10, url=>$11, pressurl=>$12, posx=>$13, "
						 "posy=>$14)",
						 m_findProjectVersionId, m_projectName, m_projectDesc, m_projectNewVersion, m_companyId, prjId,
						 m_projectVersionIndex, dt, lastNodes, departments, filePath, m_pressReleaseURL, m_x, m_y);

					switch (m_errorCode)
					{
					case 0:
						m_findProjectVersion = m_projectNewVersion;
						m_findProjectName = m_projectName;
						m_errorMessage = "Value has successfully updated!";
						break;
					case 1:
						m_errorMessage = "Project is invalid!";
						break;
					case 2:
						m_errorMessage = "Project name is invalid!";
						break;
					case 3:
						m_errorMessage = "Company is invalid!";
						break;
					case 4:
						m_errorMessage = "Parent project is invalid!";
						break;
					case 5:
						m_errorMessage = "Version already exists!";
						break;
					case 7:
						m_errorMessage = "Project can't have it's self as previous project!";
						break;
					case 8:
						m_errorMessage = "Project order is invalid!";
						break;
					case 9:
						m_errorMessage = "Department list has invalid value!";
						break;
					case 11:
						m_errorMessage = "Project version length exceeds maximum length limit 255!";
						break;
					case 12:
						m_errorMessage = "Date is invalid!";
						break;
					case 13:
						m_errorMessage = "Date is invalid!";
						break;
					}

					guard.commit();
					resetVersionOrderList();
				}
			}
		}
		catch (const std::exception& ex)
		{
			FormHandler::logs()->error("Core", ex.what());
		}
	}

	ImGui::PopStyleColor(2);
	return 0;
}

const char* ProjectUpdate::name() const { return "Projects"; }

void ProjectUpdate::reset() {}

std::string_view ProjectUpdate::getStatusMessage() const { return m_errorMessage; }

int ProjectUpdate::getStatusCode() const { return m_errorCode; }

void ProjectUpdate::resetVersionOrderList()
{
	m_versionOrderList.clear();
	FormHandler::getDbConn()->execute(
		 [&](auto&& data) {
			 using dmitigr::pgfe::to;
			 auto version = std::move(to<std::string>(data[1]));
			 if (version == m_findProjectVersion)
			 {
				 m_projectVersionIndex = to<uint32_t>(data[0]);
			 }

			 m_versionOrderList.emplace_back(std::move(version));
		 },
		 "SELECT * FROM getProjectVersions($1)", m_parentProjectId);

	m_projectVersionOrderIndex = m_projectVersionIndex;
	m_projectVersionIndexChanged = false;
}
}  // namespace DataGraph::Forms
