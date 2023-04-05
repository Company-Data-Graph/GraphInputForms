#include "CompanyDeleteForm.hpp"

#include <imgui.h>

#include <vector>
#include <set>

#include <Core/FormHandler.hpp>
#include <utils/UIModules.hpp>

namespace DataGraph::Forms
{
int CompanyDelete::init() { return 0; }

int CompanyDelete::draw()
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

	utils::companyComboBox(name(), m_companyId, m_companyNamePreview, m_companySearch);

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0, 0, 1});

	if (m_companyId != -1 && ImGui::Button("Delete"))
	{
		bool has = false;

		std::array<int, 1> copanies;
		copanies[0] = m_companyId;
		FormHandler::getDbConn()->execute(
			 [&has](auto&& data) { has = true; }, fmt::format("SELECT * FROM getprojects(companyids=>ARRAY[{0}]) LIMIT 1", m_companyId));
		if (has)
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
				 "SELECT * FROM deletecompany($1)", m_companyId);

			switch (m_errorCode)
			{
			case 0:
				m_errorMessage = "Value has been successfully deleted";
				break;
			case 1:
				m_errorMessage = "Ivalid value selected, maybe it was removed or updated earlier, please try again.";
				break;
			}
			m_companyNamePreview = "";
			m_companyId = -1;
		}
	}
	const ImVec2 center = ImVec2(ImGui::GetWindowWidth() / 2.0f, ImGui::GetWindowHeight() / 2.0f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	constexpr const ImVec2 PopupSize(500.0f, 300.0f);
	ImGui::SetNextWindowSize(PopupSize);

	windowOpen = true;

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	if (ImGui::BeginPopupModal("Help", &windowOpen, windowFlags))
	{
		constexpr const float ButtonOffsetBottomY = 35.0f;

		ImGui::Text(R"(Company was used in project, please, change it on another one.)");

		ImGui::SetCursorPosY(PopupSize.y - ButtonOffsetBottomY);
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

const char* CompanyDelete::name() const { return "Companies"; }

void CompanyDelete::reset()
{
	m_errorMessage = "";
	m_errorCode = -1;
}

std::string_view CompanyDelete::getStatusMessage() const { return m_errorMessage; }

int CompanyDelete::getStatusCode() const { return m_errorCode; }
}  // namespace DataGraph::Forms
