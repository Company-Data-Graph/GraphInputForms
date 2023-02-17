#include "OwnerUpdateForm.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <iostream>

#include <Core/FormHandler.hpp>
#include <utils/UIModules.hpp>

namespace DataGraph::Forms
{
int OwnerUpdate::init() { return 0; }

int OwnerUpdate::draw()
{
	constexpr const uint16_t NewOwnerNameInputWidth = 250;
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

	static uint32_t prevOwnerId = -1;
	utils::ownerComboBox(name(), m_ownerId, m_ownerNamePreview, m_ownerSearch);
	if (m_ownerId == -1)
	{
		return 0;
	}

	if (m_ownerId != prevOwnerId)
	{
		prevOwnerId = m_ownerId;
		m_ownerNewName = m_ownerNamePreview;
	}

	ImGui::Text("New Owner name: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(NewOwnerNameInputWidth);
	ImGui::InputText("##NewOwnerName", &m_ownerNewName);
	ImGui::PopItemWidth();

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});

	if (m_ownerId != -1 && ImGui::Button("Update"))
	{
		uint32_t* code = &m_errorCode;
		FormHandler::getDbConn()->execute(
			 [code](auto&& data) {
				 using dmitigr::pgfe::to;
				 *code = to<int>(data[0]);
			 },
			 "SELECT * FROM updatecompanyowner($1, $2) LIMIT 1", m_ownerId, m_ownerNewName);

		switch (m_errorCode)
		{
		case 0:
			m_errorMessage = "Value has been successfully updated!";

			m_ownerNamePreview = "";
			m_ownerId = -1;

			break;
		case 1:
			m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again!";
			break;
		case 2:
			m_errorMessage = "Name is invalid!";
			break;
		case 3:
			m_errorMessage = "Name already exists!";
			break;
		case 4:
			m_errorMessage = "Name length exceeds maximum length limit 255!";
			break;
		}
	}
	ImGui::PopStyleColor(2);

	return 0;
}

const char* OwnerUpdate::name() const { return "Owners"; }
void OwnerUpdate::reset() {}
}  // namespace DataGraph::Forms
