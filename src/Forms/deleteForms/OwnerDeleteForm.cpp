#include "OwnerDeleteForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <unordered_map>

#include <Core/FormHandler.hpp>
#include <utils/UIModules.hpp>

namespace DataGraph::Forms
{
int OwnerDelete::init() { return 0; }

int OwnerDelete::draw()
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

	utils::ownerComboBox(name(), m_ownerId, m_ownerNamePreview, m_ownerSearch);

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0, 0, 1});

	if (m_ownerId != -1 && ImGui::Button("Delete"))
	{
		std::string* companyName = &m_companyErrorName;
		FormHandler::getDbConn()->execute(
			 [companyName](auto&& data) {
				 using dmitigr::pgfe::to;
				 *companyName = std::move(to<std::string>(data["name"]));
			 },
			 "SELECT * FROM getcompanies(ownersearchid=>$1) LIMIT 1", m_ownerId);

		if (!m_companyErrorName.empty())
		{
			m_errorMessage = "";
			ImGui::OpenPopup("Help");
		}
		else
		{
			int* code = &m_errorCode;
			FormHandler::getDbConn()->execute(
				 [code](auto&& data) {
					 using dmitigr::pgfe::to;
					 *code = to<int>(data[0]);
				 },
				 "SELECT * FROM deleteowner($1)", m_ownerId);

			switch (m_errorCode)
			{
			case 0:
				m_errorMessage = "Value has been successfully deleted";
				break;
			case 1:
				m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
				break;
			}
			m_ownerNamePreview = "";
			m_ownerId = -1;
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

		ImGui::Text(R"(Company "%s" has current owner, please, change it on another one.)", m_companyErrorName.c_str());

		ImGui::SetCursorPosY(popupSize.y - buttonOffsetBottomY);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
			windowOpen = false;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(2);
	return 0;
}

const char* OwnerDelete::name() const { return "Owners"; }
void OwnerDelete::reset()
{
	m_companyErrorName = "";
	m_errorCode = 0;
	m_errorMessage = "";
	m_ownerSearch = "";
}
}  // namespace DataGraph::Forms
