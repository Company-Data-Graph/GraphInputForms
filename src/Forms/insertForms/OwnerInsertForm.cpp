#include "OwnerInsertForm.hpp"

#include <string>
#include <Core/FormHandler.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace DataGraph::Forms
{
int OwnerInsert::init() { return 0; }

int OwnerInsert::draw()
{
	if (!m_errorMessage.empty())
	{
		if (m_errorCode == 0)
		{
			ImGui::TextColored({0, 0.9, 0, 1}, "%s", m_errorMessage.data());
		}
		else
		{
			ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
		}
	}

	ImGui::Text("Owner name: ");
	ImGui::SameLine();
	ImGui::InputText("##Name", &m_ownerName);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Submit"))
	{
		int* dataPtr = &m_errorCode;
		FormHandler::getDbConn()->execute(
			 [dataPtr](auto&& data) {
				 using dmitigr::pgfe::to;
				 *dataPtr = to<int>(data[0]);
			 },
			 "SELECT * FROM addowner($1)", m_ownerName);
		switch (m_errorCode)
		{
		case 0:
			m_ownerName = "";
			m_errorMessage = "Value has successfully added";
			break;
		case 1:
			m_errorMessage = "Owner already exists!";
			break;
		case 2:
			m_errorMessage = "Name is invalid";
			break;
		case 3:
			m_errorMessage = "Name length exceeds maximum length limit 255";
			break;
		}
	}

	ImGui::PopStyleColor(2);

	return 0;
}

const char* OwnerInsert::name() const { return "Owners"; }

void OwnerInsert::reset() {}

std::string_view OwnerInsert::getStatusMessage() const { return m_errorMessage; }

int OwnerInsert::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
