#include "DepartmentsUpdateForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int DepartmentUpdate::init() { return 0; }

int DepartmentUpdate::draw()
{
	static std::string preview = "";
	constexpr const uint16_t DepartmentWidth = 200;
	constexpr const uint16_t DepartmentSearchWidth = 200;

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

	ImGui::Text("Department name: ");
	ImGui::SameLine();


	ImGui::PushItemWidth(DepartmentWidth);
	if (ImGui::BeginCombo("##DepartmentsList", preview.c_str()))
	{
		auto curId = &m_departmentId;
		auto ptr = &m_newDepartmentName;
		FormHandler::getDbConn()->execute(
			 [&](auto&& data) {
				 using dmitigr::pgfe::to;
				 std::pair dt{to<int>(data[0]), std::move(to<std::string>(data[1]))};
				 if (ImGui::Selectable(dt.second.c_str()))
				 {
					 *ptr = dt.second;
					 *curId = dt.first;
					 preview = std::move(dt.second);
				 }
			 },
			 "SELECT * FROM getdepartments($1)", m_searchName);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::Text("Department search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(DepartmentSearchWidth);
	ImGui::InputText("##DepartmentsSearch", &m_searchName);
	ImGui::PopItemWidth();

	if (m_departmentId != -1)
	{
		ImGui::Text("New Department name: ");
		ImGui::SameLine();
		ImGui::InputText("##DepartmentsNewName", &m_newDepartmentName);

		ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});

		if (ImGui::Button("Update"))
		{
			uint32_t* code = &m_errorCode;

			try
			{
				FormHandler::getDbConn()->execute(
					 [code](auto&& data) {
						 using dmitigr::pgfe::to;
						 *code = to<int>(data[0]);
					 },
					 "SELECT * FROM updatedepartments($1, $2)", m_departmentId, m_newDepartmentName);
			}
			catch (const std::exception& ex)
			{
				FormHandler::logs()->error("Core", ex.what());
			}
			switch (m_errorCode)
			{
			case 0:
				m_errorMessage = "Value has been successfully added!";
				m_departmentId = -1;
				m_newDepartmentName.clear();

				break;
			case 1:
				m_errorMessage = "Chosen department is invalid, something went wrong, try again!";
				break;
			case 2:
				m_errorMessage = "Department name is invalid or it already exist!";
				break;
			}
		}
		ImGui::PopStyleColor(2);
	}

	return 0;
}

const char* DepartmentUpdate::name() const { return "Departments"; }

void DepartmentUpdate::reset() {}

std::string_view DepartmentUpdate::getStatusMessage() const { return m_errorMessage; }

int DepartmentUpdate::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
