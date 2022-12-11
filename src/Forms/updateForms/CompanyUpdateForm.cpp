#include "CompanyUpdateForm.hpp"

#include <Forms/imguidatechooser/imguidatechooser.hPP>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <vector>
#include <set>

#include <iostream>

namespace DataGraph::Forms
{
int CompanyUpdate::init() { return 0; }

int CompanyUpdate::draw()
{
	static bool enabled = false;
	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	{
		ImGui::Text("Company name: ");
		ImGui::SameLine();
		int curItem = 0;
		int itemCount = 2;
		const char* items[] = {"Sber", "VK", "Meta"};
		ImGui::PushItemWidth(350);
		ImGui::Combo("##CompanyNameList", &curItem, items, itemCount);

		std::string projectSearch;
		ImGui::SameLine();
		ImGui::Text("Company search by name: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(350);
		ImGui::InputText("##CompanyNameSearch", &projectSearch);
	}

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});
	if (ImGui::Button("Submit choice"))
	{
		enabled = true;
	}
	ImGui::PopStyleColor(2);

	if (!enabled)
	{
		return 0;
	}

	char nameBuf[20] = {};
	ImGui::Text("Company name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", nameBuf, 20);

	std::string iconPath;
	ImGui::Text("IconPath: ");
	ImGui::SameLine();
	ImGui::InputText("##IconPath", &iconPath);

	ImGui::Text("Owner: ");
	ImGui::SameLine();
	int curItem = 0;
	int itemCount = 2;
	const char* items[] = {"None", "German"};
	ImGui::PushItemWidth(350);
	ImGui::Combo("##OwnersList", &curItem, items, itemCount);

	std::string ownerSearch;
	ImGui::SameLine();
	ImGui::Text("Owner search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##OwnerSearch", &ownerSearch);

	static int employees = 0;
	static bool knownEmployeeCount = false;
	if (knownEmployeeCount)
	{
		ImGui::BeginDisabled();
	}
	ImGui::Text("Employee count: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputInt("##EmployeeCount", &employees);

	if (knownEmployeeCount)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	ImGui::Checkbox("##UnknownEmployeeCount", &knownEmployeeCount);

	std::string desc;
	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &desc);

	std::string addr;
	ImGui::Text("Address: ");
	ImGui::SameLine();
	ImGui::InputText("##Address", &addr);

	std::string DepartmentName;
	ImGui::Text("Department search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##DepartmentSearch", &DepartmentName);

	std::vector<const char*> Departments = {"It", "Banking", "Shop"};
	static std::set<int> selected;

	ImGui::Text("Departments: ");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##Departments", ""))
	{
		for (uint32_t i = 0; i < Departments.size(); ++i)
		{
			bool isSelected = selected.find(i) != selected.end();
			if (ImGui::Selectable(Departments[i], isSelected))
			{
				if (!isSelected)
				{
					selected.emplace(i);
				}
				else
				{
					selected.erase(i);
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("Types: ");
	auto typeTextPos = ImGui::GetCursorPos();

	auto padding = ImGui::GetStyle().WindowPadding;
	for (auto id : selected)
	{
		auto curPos = ImGui::GetCursorPos();
		if (curPos.x + padding.x + ImGui::CalcTextSize(Departments[id]).x < ImGui::GetWindowSize().x)
		{
			ImGui::SameLine();
		}
		else
		{
			ImGui::SetCursorPosX(typeTextPos.x);
		}
		auto samePos = ImGui::GetCursorPos();
		ImGui::Text(Departments[id]);
	}

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
	if (ImGui::Button("Update"))
	{
		m_errorMessage = "No database connection";
	}
	ImGui::PopStyleColor(2);
	return 0;
}
const char* CompanyUpdate::name() const { return "Companies"; }

void CompanyUpdate::reset() {}

}  // namespace DataGraph::Forms
