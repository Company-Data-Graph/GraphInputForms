#include "DepartmentsInsertForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

namespace DataGraph::Forms
{
int DepartmentInsert::init() { return 0; }

int inputTextCallBack(ImGuiInputTextCallbackData* data) { return 0; }

int DepartmentInsert::draw()
{
	static char nameBuf[20] = {};
	int maxSize = 10;

	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	ImGui::Text("Department name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", nameBuf, 20, ImGuiInputTextFlags_CallbackCharFilter, inputTextCallBack, &maxSize);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Submit"))
	{
		m_errorMessage = "Department already exists !";
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* DepartmentInsert::name() const { return "Departments"; }

void DepartmentInsert::reset() {}

}  // namespace DataGraph::Forms
