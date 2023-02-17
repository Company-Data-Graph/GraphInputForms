#include "CompanyUpdateForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <vector>
#include <time.h>

#include <Core/Networking.hpp>
#include <Core/FormHandler.hpp>

#include <Forms/imguidatechooser/imguidatechooser.hpp>

#include <utils/FileManagerDialoge.hpp>
#include <utils/StringParse.hpp>
#include <utils/UIModules.hpp>
#include <utils/Time.hpp>


namespace DataGraph::Forms
{
int CompanyUpdate::init() { return 0; }

int CompanyUpdate::draw()
{
	constexpr const uint16_t CompanyXPosInputWidth = 120;
	constexpr const uint16_t CompanyYPosInputWidth = 120;

	if (!m_errorMessage.empty())
	{
		if (m_errorCode != 0)
		{
			ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
		}
		else
		{
			ImGui::TextColored({0.0, 0.9, 0, 1}, "%s", m_errorMessage.data());
		}
	}

	static uint32_t prevcompanyId = -1;
	utils::companyComboBox(name(), m_companyId, m_companyNamePreview, m_companySearch);
	if (m_companyId == -1)
	{
		return 0;
	}

	if (m_companyId != prevcompanyId)
	{
		prevcompanyId = m_companyId;

		auto ownerIdPtr = &m_ownerId;
		auto ownerNamePreview = &m_ownerPreview;
		auto companyNamePtr = &m_companyName;
		auto companyDescPtr = &m_companyDesc;
		auto companyAddrPtr = &m_companyAddr;
		auto companyIconPtr = &m_filePath;
		auto departmentsList = &m_chosenDepartments;
		auto employeesPtr = &m_employees;
		auto x = &m_x;
		auto y = &m_y;
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 *companyNamePtr = std::move(to<std::string>(data["name"]));
				 *ownerIdPtr = std::move(to<int>(data["ownerid"]));
				 *companyDescPtr = std::move(to<std::string>(data["description"]));
				 *companyAddrPtr = std::move(to<std::string>(data["address"]));
				 *employeesPtr = std::move(to<std::optional<int>>(data["employeesnum"]));
				 *companyIconPtr = std::move(to<std::string>(data["iconpath"]));
				 *ownerNamePreview = std::move(to<std::string>(data["ownername"]));
				 *x = std::move(to<int>(data["posy"]));
				 *y = std::move(to<int>(data["posx"]));
				 departmentsList->clear();

				 std::vector<short> departments = std::move(to<std::vector<short>>(data["companytypeenum"]));
				 std::vector<std::string> departmentsNames = std::move(to<std::vector<std::string>>(data["companytypename"]));
				 for (uint32_t i = 0; i < departments.size(); ++i)
				 {
					 departmentsList->emplace(std::move(departmentsNames[i]), departments[i]);
				 }

				 std::optional<std::string> incDate = std::move(to<std::optional<std::string>>(data["foundationyear"]));
				 if (incDate.has_value())
				 {
					 tm date;
					 utils::strptime(incDate.value().c_str(), "%Y-%m-%d", &date);
					 m_date = date;
				 }
			 },
			 "SELECT * FROM getcompanies(companyid=>$1)", m_companyId);
	}

	ImGui::Text("Company name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", &m_companyName);

	static std::string loadedFile;
	if (!utils::inputFile(name(), loadedFile, &m_errorMessage))
	{
		m_errorCode = 1;
	}

	utils::ownerComboBox(name(), m_ownerId, m_ownerPreview, m_ownerSearch);

	utils::inputEmployeeCount(name(), m_employees);

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

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});
	if (ImGui::Button("Update"))
	{
		int employeeCount = m_employees.value_or(-1);
		std::string departmentsList = utils::parseDepartments(m_chosenDepartments);
		std::string dt = utils::parseDate(m_date);
		uint32_t* dataPtr = &m_errorCode;

		std::string_view iconPath;
		if (loadedFile.empty())
		{
			iconPath = m_filePath;
		}
		try
		{
			FormHandler::getDbConn()->execute(
				 [dataPtr](auto&& data) {
					 using dmitigr::pgfe::to;
					 *dataPtr = to<int>(data[0]);
				 },
				 "SELECT * FROM updatecompany(id=>$1, name=>$2, ownerid=>$3, foundationyear=>$4, employeescount=>$5, description=>$6, "
				 "address=>$7, iconpath=>$8, typeids=>$9)",
				 m_companyId, m_companyName, m_ownerId, dt, employeeCount, m_companyDesc, m_companyAddr, iconPath, departmentsList);
		}
		catch (const std::exception& ex)
		{
			FormHandler::logs()->error("Core", ex.what());
		}

		switch (m_errorCode)
		{
		case 0:
			m_errorMessage = "Value has successfully updated!";

			loadedFile.clear();
			break;
		case 1:
			m_errorMessage = "Company does not exist!";
			break;
		case 2:
			m_errorMessage = "Name is invalid or already exist!";
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
			auto result = Networking::loadFileToHost(loadedFile, *FormHandler::getMedServConn(), "127.0.0.1", "5555", response);
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
					 "SELECT * FROM updatecompany(id=>$1, name=>$2, ownerid=>$3, foundationyear=>$4, employeescount=>$5, description=>$6, "
					 "address=>$7, iconpath=>$8, typeids=>$9, posx=>$10, posy=>$11)",
					 m_companyId, m_companyName, m_ownerId, dt, employeeCount, m_companyDesc, m_companyAddr,
					 fmt::format("http://{}:{}/media-server/data/{}", FormHandler::getMedServConn()->ip,
						  FormHandler::getMedServConn()->port, response.body["fileName"].get<std::string>()),
					 departmentsList, m_x, m_y);

				switch (m_errorCode)
				{
				case 0:
					m_errorMessage = "Value has successfully added!";
					loadedFile.clear();

					break;
				case 1:
					m_errorMessage = "Company name value is invalid!";
					break;
				case 2:
					m_errorMessage = "Name is already exist!";
					break;
				case 3:
					m_errorMessage = "Owner is invalid!";
					break;
				case 4:
					m_errorMessage = "Department list has invalid value!";
					break;
				case 5:
					m_errorMessage = "Employee count is invalid!";
					break;
				case 6:
					m_errorMessage = "Name length exceeds maximum length limit 255!";
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
const char* CompanyUpdate::name() const { return "Companies"; }

void CompanyUpdate::reset() {}

}  // namespace DataGraph::Forms
