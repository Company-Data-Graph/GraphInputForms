#include "DepartmentsUpdateForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

namespace DataGraph::Forms
{
int DepartmentUpdate::init() { return 0; }

int DepartmentUpdate::draw()
{
	static bool enabled = false;

	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	ImGui::Text("Department: ");
	ImGui::SameLine();
	int curItem = 0;
	int itemCount = 2;
	const char* items[] = {"It", "Banking", "Shop"};
	ImGui::PushItemWidth(350);
	ImGui::Combo("##Departments", &curItem, items, itemCount);

	std::string DepartmentName;
	ImGui::SameLine();
	ImGui::Text("Department search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##DepartmentsSearch", &DepartmentName);

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

	std::string newDepartmentName;
	ImGui::Text("New Department name: ");
	ImGui::SameLine();
	ImGui::InputText("##DepartmentsNewName", &newDepartmentName);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});

	if (ImGui::Button("Update"))
	{
		m_errorMessage = "No database connection";
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* DepartmentUpdate::name() const { return "Departments"; }

void DepartmentUpdate::reset() {}

}  // namespace DataGraph::Forms
