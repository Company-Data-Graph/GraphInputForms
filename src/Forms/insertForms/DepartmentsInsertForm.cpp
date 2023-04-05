#include "DepartmentsInsertForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int DepartmentInsert::init() { return 0; }

int inputTextCallBack(ImGuiInputTextCallbackData* data) { return 0; }

int DepartmentInsert::draw()
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

	ImGui::Text("Department name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", &m_departmentName);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Submit"))
	{
		uint32_t* code = &m_errorCode;

		FormHandler::getDbConn()->execute(
			 [code](auto&& data) {
				 using dmitigr::pgfe::to;
				 *code = to<int>(data[0]);
			 },
			 "SELECT * FROM adddepartment($1)", m_departmentName);

		switch (m_errorCode)
		{
		case 0:
			m_errorMessage = "Value has been successfully added!";
			break;
		case 1:
			m_errorMessage = "Department already exists!";
			break;
		case 2:
			m_errorMessage = "No valid was given!";
			break;
		case 3:
			m_errorMessage = "Department length exceeds maximum length limit 255!";
			break;
		}
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* DepartmentInsert::name() const { return "Departments"; }

void DepartmentInsert::reset() {}

std::string_view DepartmentInsert::getStatusMessage() const { return m_errorMessage; }

int DepartmentInsert::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
