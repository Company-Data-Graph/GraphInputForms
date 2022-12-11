#include "ProjectDeleteForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <set>
#include <vector>
#include <Forms/imguidatechooser/imguidatechooser.hPP>

namespace DataGraph::Forms
{
int ProjectDelete::init() { return 0; }

int ProjectDelete::draw()
{
	char nameBuf[20] = {};
	int maxSize = 10;

	if (!m_errorMessage.empty())
	{
		ImGui::TextColored({0.9, 0, 0, 1}, "Error has occurred: \"%s\"", m_errorMessage.data());
	}

	{
		ImGui::Text("Project name: ");
		ImGui::SameLine();
		int curItem = 0;
		int itemCount = 2;
		const char* items[] = {"Sberbank", "Vkontakte", "Facebook"};
		ImGui::PushItemWidth(350);
		ImGui::Combo("##ProjectNameList", &curItem, items, itemCount);

		std::string projectSearch;
		ImGui::SameLine();
		ImGui::Text("Project search by name: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(350);
		ImGui::InputText("##ProjectNameSearch", &projectSearch);
	}

	ImGui::Text("Project Version: ");
	ImGui::SameLine();
	{
		int curItem = 0;
		int itemCount = 2;
		const char* items[] = {"1.0", "1.01", "2.2f"};
		ImGui::PushItemWidth(350);
		ImGui::Combo("##ProjectVersionList", &curItem, items, itemCount);
	}

	std::string projectVersionSearch;
	ImGui::SameLine();
	ImGui::Text("Project version search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##ProjectVersionSearch", &projectVersionSearch);

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0, 0, 1});
	if (ImGui::Button("Submit"))
	{
		m_errorMessage = "No database connection";
		ImGui::OpenPopup("Help");
	}

	bool windowOpen = true;

	const ImVec2 center = ImVec2(ImGui::GetWindowWidth() / 2.0f, ImGui::GetWindowHeight() / 2.0f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	const ImVec2 popupSize = ImVec2(500.0f, 300.0f);

	ImGui::SetNextWindowSize(popupSize);

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	if (ImGui::BeginPopupModal("Help", &windowOpen, windowFlags))
	{
		const float buttonOffsetBottomY = 35.0f;
		ImGui::Text(R"(Project had some connections. Are you sure you want to delete it?)");

		ImGui::SetCursorPosY(popupSize.y - buttonOffsetBottomY);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
			windowOpen = false;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(popupSize.x - ImGui::CalcTextSize("Agree").x - ImGui::GetStyle().WindowPadding.x - 5);

		if (ImGui::Button("Agree"))
		{}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(2);
	return 0;
}

const char* ProjectDelete::name() const { return "Projects"; }

void ProjectDelete::reset()
{
}

}  // namespace DataGraph::Forms
