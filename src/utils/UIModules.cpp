#include "UIModules.hpp"

#include <corecrt_wtime.h>
#include <optional>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <pgfe/pgfe.hpp>

#include <core/FormHandler.hpp>
#include <utils/FileManagerDialoge.hpp>
#include <Forms/imguidatechooser/imguidatechooser.hpp>

namespace DataGraph::utils
{
void ownerComboBox(std::string_view name, uint32_t& currentOwnerId, std::string& comboBoxPreview, std::string& searchName)
{
	constexpr const uint16_t OwnerBoxWidth = 400;
	constexpr const uint16_t SearchInputWidth = 250;

	ImGui::PushID(name.data());
	ImGui::Text("Owner name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(OwnerBoxWidth);
	if (ImGui::BeginCombo("##OwnersList", comboBoxPreview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [&currentOwnerId, &comboBoxPreview](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 currentOwnerId = dt.first;
					 comboBoxPreview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getownerslist($1)", searchName);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("Owner search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(SearchInputWidth);
	ImGui::InputText("##OwnerSearch", &searchName);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void inputEmployeeCount(std::string_view name, std::optional<int>& employeeCount)
{
	constexpr const uint16_t employeeInputWidth = 150;

	ImGui::PushID(name.data());
	bool isUnknown = !employeeCount.has_value();
	if (isUnknown)
	{
		ImGui::BeginDisabled();
	}
	ImGui::Text("Employee count: ");

	ImGui::SameLine();
	ImGui::PushItemWidth(employeeInputWidth);
	int count = employeeCount.value_or(0);
	if (ImGui::InputInt("##EmployeeCount", &count))
	{
		if (count < 0)
		{
			count = 0;
		}

		employeeCount = count;
	}
	ImGui::PopItemWidth();

	if (isUnknown)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();

	if (ImGui::Checkbox("##UnknownEmployeeCount", &isUnknown))
	{
		if (isUnknown)
		{
			employeeCount.reset();
		}
		else
		{
			employeeCount = 0;
		}
	}
	ImGui::PopID();
}

void departmentComboBox(std::string_view name, std::unordered_map<std::string, uint32_t>& chosenDepartments)
{
	constexpr const uint16_t DepartmentComboWidth = 300;
	ImGui::PushID(name.data());
	ImGui::Text("Department name: ");
	ImGui::SameLine();

	std::string_view departmentPreview;
	if (!chosenDepartments.empty())
	{
		departmentPreview = chosenDepartments.begin()->first;
	}
	else
	{
		departmentPreview = "";
	}

	ImGui::PushItemWidth(DepartmentComboWidth);
	if (ImGui::BeginCombo("##DepartmentsList", departmentPreview.data()))
	{
		FormHandler::getDbConn()->execute(
			 [&chosenDepartments](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::string str = std::move(to<std::string>(data[1]));
				 auto it = chosenDepartments.find(str);
				 bool selectable = it != chosenDepartments.end();
				 if (ImGui::Selectable(str.c_str(), &selectable))
				 {
					 if (selectable)
					 {
						 chosenDepartments.emplace(std::move(str), to<int>(data[0]));
					 }
					 else
					 {
						 if (it != chosenDepartments.end())
						 {
							 chosenDepartments.erase(it);
						 }
					 }
				 }
			 },
			 "SELECT * FROM getdepartments()");
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();

	auto& style = ImGui::GetStyle();
	auto indent = style.ItemSpacing.x + style.ColumnsMinSpacing + style.WindowPadding.x;

	auto chosenDeparmentsPos = ImGui::GetCursorPosX() + ImGui::CalcTextSize("Chosen Departments:").x;
	ImGui::Text("Chosen Departments:");

	auto typeTextPos = chosenDeparmentsPos;
	for (auto& [name, _] : chosenDepartments)
	{
		if (typeTextPos + indent + ImGui::CalcTextSize(name.c_str()).x < ImGui::GetWindowSize().x)
		{
			ImGui::SameLine(typeTextPos + indent);
			typeTextPos += indent + ImGui::CalcTextSize(name.c_str()).x;
		}
		else
		{
			typeTextPos = ImGui::CalcTextSize(name.c_str()).x;
		}

		ImGui::Text(name.c_str());
	}
	ImGui::PopID();
}

void inputDate(std::string_view name, std::optional<tm>& date)
{
	constexpr const uint16_t DatePickerWidth = 200;
	bool isUnknown = !date.has_value();
	ImGui::PushID(name.data());

	ImGui::Text("Foundation year: ");
	ImGui::SameLine();
	if (isUnknown)
	{
		ImGui::BeginDisabled();
	}

	static tm today{};
	if (today.tm_year == 0)
	{
		ImGui::SetDateToday(&today);
	}

	tm newDay = date.value_or(today);
	ImGui::PushItemWidth(DatePickerWidth);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0.00001);
	if (ImGui::DateChooser("##Date", newDay))
	{
		date = newDay;
	}

	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	if (isUnknown)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	if (ImGui::Checkbox("##UnknownDate", &isUnknown))
	{
		if (isUnknown)
		{
			date.reset();
		}
		else
		{
			date = today;
		}
	}
	ImGui::PopID();
}

bool inputFile(std::string_view name, std::string& loadedFile, std::string_view* errorMessage)
{
	ImGui::PushID(name.data());

	std::string iconLoadText;
	ImVec4 buttonColor;
	if (!loadedFile.empty())
	{
		iconLoadText = fmt::format("{0} file selected: Press button to change it", loadedFile);
		buttonColor = {1.0f, 1.0f, 0.0f, 1.0f};
	}
	else
	{
		iconLoadText = "Press 'Load Icon' button to select Icon file";
		buttonColor = {0.0f, 1.0f, 0.0f, 1.0f};
	}
	ImGui::Text("%s", iconLoadText.data());
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, {buttonColor.x * 0.6f, buttonColor.y * 0.6f, buttonColor.z, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {buttonColor.x * 0.2f, buttonColor.y * 0.2f, buttonColor.z, 1.0f});
	if (ImGui::Button("Load Icon"))
	{
		if (!utils::openFile(loadedFile))
		{
			if (errorMessage != nullptr)
			{
				*errorMessage = "An error occurred during the upload process, the file may have weighed more than 2 MB";
			}
			ImGui::PopStyleColor(2);
			ImGui::PopID();

			return false;
		}

		ImGuiContext& g = *ImGui::GetCurrentContext();
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;
	}
	ImGui::PopStyleColor(2);
	ImGui::PopID();

	return true;
}

void inputProjectName(std::string_view name, uint32_t& projectId, std::string& projectNamePreview, std::string& projectNameSearch)
{
	ImGui::PushID(name.data());

	constexpr const uint16_t ProjectNameComboBoxWidth = 250;
	constexpr const uint16_t ProjectNameSearchWidth = 250;
	ImGui::Text("Project name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectNameComboBoxWidth);
	if (ImGui::BeginCombo("##ProjectNameList", projectNamePreview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 projectId = dt.first;
					 projectNamePreview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getProjectListNames($1)", projectNameSearch);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("Project search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectNameSearchWidth);
	ImGui::InputText("##ProjectNameSearch", &projectNameSearch);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void inputProjectVersion(std::string_view name,
	 uint32_t& projectId,
	 std::string& projectVersionName,
	 uint32_t& nodeId,
	 std::string& projectVersionNamePreview,
	 std::string& projectVersionSearch)
{
	constexpr const uint16_t ProjectVersionSearchWidth = 100;
	constexpr const uint16_t ProjectVersionComboBoxWidth = 100;
	ImGui::PushID(name.data());

	ImGui::Text("Project Version: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectVersionComboBoxWidth);
	if (ImGui::BeginCombo("##ProjectVersionList", projectVersionNamePreview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data["nodeid"]), std::move(to<std::string>(data["version"]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 nodeId = dt.first;
					 projectVersionNamePreview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getprojects(searchprojectid=>$1, namesearch=>$2, searchversion=>$3)", projectId, projectVersionName,
			 projectVersionSearch);

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("Project version search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ProjectVersionSearchWidth);
	ImGui::InputText("##ProjectVersionSearch", &projectVersionSearch);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void previousProjectVersionCombo(std::string_view name,
	 uint32_t& projectId,
	 std::string& projectNamePreview,
	 std::string& projectNameSearch,
	 uint32_t& projectVersionId,
	 std::string& projectVersionNamePreview,
	 std::string& projectVersionSearch,
	 std::map<int, std::string>& prevProjectVersionsList)
{
	constexpr const uint16_t ProjectVersionListWidth = 350;

	ImGui::PushID(name.data());

	ImGui::Text("Previous project versions list:");
	std::string_view prevVersionListPreview;
	if (prevProjectVersionsList.empty())
	{
		prevVersionListPreview = "";
	}
	else
	{
		prevVersionListPreview = prevProjectVersionsList.begin()->second.c_str();
	}
	ImGui::PushItemWidth(ProjectVersionListWidth);
	if (ImGui::BeginCombo("##ProjectPreviousVersions", prevVersionListPreview.data()))
	{
		auto eraseIt = prevProjectVersionsList.end();
		for (auto it = prevProjectVersionsList.begin(); it != prevProjectVersionsList.end(); ++it)
		{
			bool isSelected = true;
			if (ImGui::Selectable(it->second.c_str(), &isSelected))
			{
				if (!isSelected)
				{
					eraseIt = it;
				}
			}
		}

		ImGui::EndCombo();
		if (eraseIt != prevProjectVersionsList.end())
		{
			prevProjectVersionsList.erase(eraseIt);
		}
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	auto zoneAfterVersionList = ImGui::GetCursorPos();
	inputProjectName(name, projectId, projectNamePreview, projectNameSearch);

	bool enableBtn = !projectNamePreview.empty() && !projectVersionNamePreview.empty()
					 && prevProjectVersionsList.find(projectVersionId) == prevProjectVersionsList.end();
	if (!enableBtn)
	{
		ImGui::BeginDisabled();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});
	if (ImGui::Button("Submit previous project Version"))
	{
		prevProjectVersionsList.emplace(projectVersionId, fmt::format("{}:{}", projectNamePreview, projectVersionNamePreview));
	}
	ImGui::PopStyleColor(2);

	if (!enableBtn)
	{
		ImGui::EndDisabled();
	}

	if (!projectNamePreview.empty())
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(zoneAfterVersionList.x);

		inputProjectVersion(name, projectId, projectNamePreview, projectVersionId, projectVersionNamePreview, projectVersionSearch);
	}
	ImGui::PopID();
}

void companyComboBox(std::string_view name, uint32_t& companyId, std::string& companyPreview, std::string& companySearch)
{
	constexpr const uint16_t CompanySearchWidth = 250;
	constexpr const uint16_t CompanyNameWidth = 250;
	ImGui::PushID(name.data());

	ImGui::Text("Company name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(CompanyNameWidth);
	if (ImGui::BeginCombo("##CompaniesList", companyPreview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 companyId = dt.first;
					 companyPreview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getcompanies($1)", companySearch);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("Company search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(CompanySearchWidth);
	ImGui::InputText("##CompanyNameSearch", &companySearch);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

}  // namespace DataGraph::utils
