#include "CompanyInsertForm.hpp"

#include <Forms/imguidatechooser/imguidatechooser.hPP>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <vector>
#include <set>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int CompanyInsert::init() { return 0; }

int CompanyInsert::draw()
{
	if (!m_errorMessage.empty())
	{
		if (m_errorCode != 0)
		{
			ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
		}
		else
		{
			ImGui::TextColored({0, 0.9, 0, 1}, "%s", m_errorMessage.data());
		}
	}

	ImGui::Text("Company name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", &m_companyName);

	std::string iconPath;
	ImGui::Text("IconPath: ");
	ImGui::SameLine();
	ImGui::InputText("##IconPath", &iconPath);

	ImGui::Text("Owner name: ");
	ImGui::SameLine();
	static std::string preview = "";
	static int curId = -1;
	if (ImGui::BeginCombo("##OwnersList", preview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 curId = dt.first;
					 preview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getownerslist($1)", m_ownerSearch);
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	ImGui::Text("Owner search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(250);
	ImGui::InputText("##OwnerSearch", &m_ownerSearch);

	static bool knownEmployeeCount = false;
	if (knownEmployeeCount)
	{
		ImGui::BeginDisabled();
	}
	ImGui::Text("Employee count: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputInt("##EmployeeCount", &m_employees);

	if (knownEmployeeCount)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	ImGui::Checkbox("##UnknownEmployeeCount", &knownEmployeeCount);

	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &m_companyDesc);

	ImGui::Text("Address: ");
	ImGui::SameLine();
	ImGui::InputText("##Address", &m_companyAddr);

	ImGui::Text("Department name: ");
	ImGui::SameLine();
	static std::string departmentPreview = "";
	static int departmentId = -1;
	if (ImGui::BeginCombo("##DepartmentsList", departmentPreview.c_str()))
	{
		FormHandler::getDbConn()->execute(
			 [](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 departmentId = dt.first;
					 departmentPreview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getdepartments()");
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	ImGui::Text("Chosen Departments: ");
	auto typeTextPos = ImGui::GetCursorPos();
	// auto padding = ImGui::GetStyle().WindowPadding;
	// for (auto id : selected)
	//{
	//	auto curPos = ImGui::GetCursorPos();
	//	if (curPos.x + padding.x + ImGui::CalcTextSize(Departments[id]).x < ImGui::GetWindowSize().x)
	//	{
	//		ImGui::SameLine();
	//	}
	//	else
	//	{
	//		ImGui::SetCursorPosX(typeTextPos.x);
	//	}
	//	auto samePos = ImGui::GetCursorPos();
	//	ImGui::Text(Departments[id]);
	// }

	static bool knownDate = true;

	ImGui::Text("Foundation year: ");
	ImGui::SameLine();
	if (knownDate)
	{
		ImGui::BeginDisabled();
	}

	tm date;
	ImGui::SetDateToday(&date);
	ImGui::DateChooser("##Date", date);
	if (knownDate)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	ImGui::Checkbox("##UnknownDate", &knownDate);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Submit"))
	{
		m_errorMessage = "No database connection";
	}
	ImGui::PopStyleColor(2);
	return 0;
}

const char* CompanyInsert::name() const { return "Companies"; }

void CompanyInsert::reset() {}

}  // namespace DataGraph::Forms
