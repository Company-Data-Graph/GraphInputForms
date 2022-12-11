#include "OwnerUpdateForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

namespace DataGraph::Forms
{
int OwnerUpdate::init() { return 0; }

int OwnerUpdate::draw()
{
	static bool enabled = false;

	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	ImGui::Text("Owner name: ");
	ImGui::SameLine();
	int curItem = 0;
	int itemCount = 2;
	const char* items[] = {"None", "German"};
	ImGui::PushItemWidth(350);
	ImGui::Combo("##OwnersList", &curItem, items, itemCount);

	std::string ownerSearch;
	ImGui::SameLine();
	ImGui::Text("Owner search by name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##OwnerSearch", &ownerSearch);

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

	std::string newOwnerName;
	ImGui::Text("New Owner name: ");
	ImGui::SameLine();
	ImGui::InputText("##NewOwnerName", &newOwnerName);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Update"))
	{
		m_errorMessage = "No database connection";
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* OwnerUpdate::name() const { return "Owner"; }
void OwnerUpdate::reset() {}
}  // namespace DataGraph::Forms
