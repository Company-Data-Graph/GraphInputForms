#include "CompanyInsertForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <vector>
#include <time.h>

#include <Forms/imguidatechooser/imguidatechooser.hpp>

#include <Core/FormHandler.hpp>
#include <Networking/Networking.hpp>

#include <utils/FileManagerDialoge.hpp>
#include <utils/StringParse.hpp>
#include <utils/UIModules.hpp>

namespace DataGraph::Forms
{
int CompanyInsert::init() { return 0; }

int CompanyInsert::draw()
{
	constexpr const uint16_t CompanyXPosInputWidth = 120;
	constexpr const uint16_t CompanyYPosInputWidth = 120;

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

	ImGui::PushItemWidth(300);

	ImGui::Text("Company name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", &m_companyName);
	ImGui::PopItemWidth();

	if (!utils::inputFile(name(), m_filePath, &m_errorMessage))
	{
		m_errorCode = 1;
	}

	if (ImGui::IsItemEdited())
	{
		m_warningMessage = "";
	}

	static std::string ownerComboBoxPreview;
	utils::ownerComboBox(name(), m_ownerId, ownerComboBoxPreview, m_ownerSearch);

	utils::inputEmployeeCount(name(), m_employeeCount);

	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &m_companyDesc);

	ImGui::Text("Address: ");
	ImGui::SameLine();
	ImGui::InputText("##Address", &m_companyAddr);

	utils::departmentComboBox(name(), m_chosenDepartments);
	utils::inputDate(name(), m_date);

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::PushItemWidth(CompanyXPosInputWidth);
	ImGui::InputInt("##XPOS", &m_x);
	ImGui::PopItemWidth();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::PushItemWidth(CompanyYPosInputWidth);
	ImGui::InputInt("##YPOS", &m_y);
	ImGui::PopItemWidth();

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (!m_filePath.empty() && !m_companyName.empty() && m_ownerId != -1 && ImGui::Button("Submit"))
	{
		m_warningMessage = "";
		int employeeCount = m_employeeCount.value_or(-1);
		std::string departmentsList = utils::parseDepartments(m_chosenDepartments);
		std::string dt = utils::parseDate(m_date);

		uint32_t* dataPtr = &m_errorCode;
		try
		{
			FormHandler::getDbConn()->execute(
				 [dataPtr](auto&& data) {
					 using dmitigr::pgfe::to;
					 *dataPtr = to<int>(data[0]);
				 },
				 "SELECT * FROM addcompany(name=>$1, iconpath=>NULL, ownerid=>$2, foundationdate=>$3, employeescount=>$4, "
				 "description=>$5, "
				 "addr=>$6, typeids=>$7)",
				 m_companyName, m_ownerId, dt, employeeCount, m_companyDesc, m_companyAddr, departmentsList);
		}
		catch (const std::exception& ex)
		{
			FormHandler::logs()->error("Core", ex.what());
		}

		switch (m_errorCode)
		{
		case 0:
			m_errorMessage = "Value has successfully added!";
			break;
		case 1:
			m_errorMessage = "Company name already exists!";
			break;
		case 2:
			m_errorMessage = "Name is already exist!";
			break;
		case 3:
			m_errorMessage = "Owner is invalid";
			break;
		case 4:
			m_errorMessage = "Department list has invalid value";
			break;
		case 5:
			m_errorMessage = "Employee count is invalid";
			break;
		case 6:
			m_errorMessage = "Name length exceeds maximum length limit 255";
			break;
		case 7:
			break;
		}

		if (m_errorCode == 7)
		{
			Networking::Response response;
			auto& credentials = FormHandler::getMedServConn();
			auto result = Networking::loadFileToHost(m_filePath, *credentials, response);
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
			else if (response.body.empty() || !response.body.contains("fileName"))
			{
				m_errorCode = 1;
				m_errorMessage = "Ups, something went wrong with upload.";
			}
			else
			{
				FormHandler::getDbConn()->execute(
					 [dataPtr](auto&& data) {
						 using dmitigr::pgfe::to;
						 *dataPtr = to<int>(data[0]);
					 },
					 "SELECT * FROM addcompany(name=>$1, iconpath=>$2, ownerid=>$3, foundationdate=>$4, employeescount=>$5, "
					 "description=>$6, "
					 "addr=>$7, typeids=>$8)",
					 m_companyName,
					 fmt::format("http://{}:{}/media-server/data/{}", FormHandler::getMedServConn()->ip,
						  FormHandler::getMedServConn()->port, response.body["fileName"].get<std::string>()),
					 m_ownerId, dt, employeeCount, m_companyDesc, m_companyAddr, departmentsList);

				switch (m_errorCode)
				{
				case 0:
					m_errorMessage = "Value has successfully added!";
					break;
				case 1:
					m_errorMessage = "Company name value is invalid!";
					break;
				case 2:
					m_errorMessage = "Name is already exist!";
					break;
				case 3:
					m_errorMessage = "Owner is invalid";
					break;
				case 4:
					m_errorMessage = "Department list has invalid value";
					break;
				case 5:
					m_errorMessage = "Employee count is invalid";
					break;
				case 6:
					m_errorMessage = "Name length exceeds maximum length limit 255";
					break;
				case 7:
					m_errorMessage = "Something went wrong!";
					break;
				}
			}
		}
	}

	ImGui::PopStyleColor(2);
	return 0;
}

const char* CompanyInsert::name() const { return "Companies"; }

void CompanyInsert::reset() {}

std::string_view CompanyInsert::getStatusMessage() const { return m_errorMessage; }

int CompanyInsert::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
