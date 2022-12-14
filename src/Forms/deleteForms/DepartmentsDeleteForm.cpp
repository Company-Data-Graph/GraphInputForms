#include "DepartmentsDeleteForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

#include <Core/FormHandler.hpp>

namespace DataGraph::Forms
{
int DepartmentDelete::init() { return 0; }

int DepartmentDelete::draw()
{
	static bool windowOpen = false;
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
	static std::string preview = "";
	static int curId = -1;
	if (ImGui::BeginCombo("##DepartmentsList", preview.c_str()))
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
			 "SELECT * FROM getdepartments()");
		ImGui::EndCombo();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0, 0, 1});

	if (curId != -1 && ImGui::Button("Delete"))
	{
		int* code = &m_errorCode;
		FormHandler::getDbConn()->execute(
			 [code](auto&& data) {
				 using dmitigr::pgfe::to;
				 *code = to<int>(data[0]);
			 },
			 "SELECT * FROM deletedepartments($1)", curId);

		switch (m_errorCode)
		{
		case 0:
			preview = "";
			curId = -1;
			m_errorMessage = "Value has been successfully deleted";
			break;
		case 1:
			preview = "";
			curId = -1;
			m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
			break;
		case 2:
			ImGui::OpenPopup("Help");
			break;
		}
	}

	const ImVec2 center = ImVec2(ImGui::GetWindowWidth() / 2.0f, ImGui::GetWindowHeight() / 2.0f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	const ImVec2 popupSize = ImVec2(500.0f, 300.0f);

	ImGui::SetNextWindowSize(popupSize);

	windowOpen = true;

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	if (ImGui::BeginPopupModal("Help", &windowOpen, windowFlags))
	{
		const float buttonOffsetBottomY = 35.0f;

		ImGui::Text(R"(Some companies or products have current department, please, change it
on another one or the value will be set to unknown.)");

		ImGui::SetCursorPosY(popupSize.y - buttonOffsetBottomY);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
			windowOpen = false;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(popupSize.x - ImGui::CalcTextSize("Agree").x - ImGui::GetStyle().WindowPadding.x - 5);

		if (ImGui::Button("Agree"))
		{
			int* code = &m_errorCode;
			FormHandler::getDbConn()->execute(
				 [code](auto&& data) {
					 using dmitigr::pgfe::to;
					 *code = to<int>(data[0]);
				 },
				 "SELECT * FROM deletedepartments($1, TRUE)", curId);
			switch (m_errorCode)
			{
			case 0:
				preview = "";
				curId = -1;
				m_errorMessage = "Value has been successfully deleted";
				break;
			case 1:
				preview = "";
				curId = -1;
				m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
				break;
			}

			ImGui::CloseCurrentPopup();
			windowOpen = false;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* DepartmentDelete::name() const { return "Departments"; }

void DepartmentDelete::reset()
{
	m_errorCode = 0;
	m_errorMessage = "";
}

}  // namespace DataGraph::Forms
