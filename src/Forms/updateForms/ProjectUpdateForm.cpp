#include "ProjectUpdateForm.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <string>
#include <list>
#include <set>
#include <vector>
#include <Forms/imguidatechooser/imguidatechooser.hPP>

namespace DataGraph::Forms
{
int ProjectUpdate::init() { return 0; }

int ProjectUpdate::draw()
{
	static bool enabled = false;
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

	std::string searchProjectByVersion;
	ImGui::SameLine();
	ImGui::Text("Project version search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##ProjectVersionSearch", &searchProjectByVersion);

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


	static bool newProject = false;
	ImGui::Text("Mark as new project");
	ImGui::Checkbox("##NewProject", &newProject);

	if (!newProject)
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

	std::string version;
	ImGui::Text("Version: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Version", &version);

	static bool knownDate = true;

	ImGui::Text("Release date: ");
	ImGui::SameLine();
	if (knownDate)
	{
		ImGui::BeginDisabled();
	}

	tm date;
	ImGui::SetDateToday(&date);
	ImGui::DateChooser("##Date", date);
	if (knownDate)
	{
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	ImGui::Text("Set to Unknown");
	ImGui::SameLine();
	ImGui::Checkbox("##UnknownDate", &knownDate);
	{
		ImGui::Text("Company name: ");
		ImGui::SameLine();
		int curItem = 0;
		int itemCount = 2;
		const char* items[] = {"Sber", "VK", "Meta"};
		ImGui::PushItemWidth(350);
		ImGui::Combo("##CompanyNameList", &curItem, items, itemCount);

		std::string companyName;
		ImGui::SameLine();
		ImGui::Text("Company search by name: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(350);
		ImGui::InputText("##CompanyNameSearch", &companyName);
	}

	std::list<std::pair<int, std::pair<const char*, const char*>>> previousVersions;
	ImGui::Text("Previous project versions list:");

	if (ImGui::BeginCombo("##ProjectPreviousVersions", ""))
	{
		auto eraseIt = previousVersions.end();
		for (auto it = previousVersions.begin(); it != previousVersions.end(); ++it)
		{
			bool isSelected = true;
			if (ImGui::Selectable(it->second.first, isSelected))
			{
				if (!isSelected)
				{
					eraseIt = it;
				}
			}
		}
		ImGui::EndCombo();
		if (eraseIt != previousVersions.end())
		{
			previousVersions.erase(eraseIt);
		}
	}
	ImGui::SameLine();
	auto zoneAfterVersionList = ImGui::GetCursorPos();
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

	ImGui::PushStyleColor(ImGuiCol_Button, {0.6, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2, 0.2, 0, 1});
	if (ImGui::Button("Submit search"))
	{}

	ImGui::PopStyleColor(2);

	ImGui::SameLine();
	ImGui::SetCursorPosX(zoneAfterVersionList.x);

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

	std::string desc;
	ImGui::Text("Description: ");
	ImGui::SameLine();
	ImGui::InputTextMultiline("##Description", &desc);

	std::vector<const char*> Departments = {"It", "Banking", "Shop"};
	static std::set<int> selected;

	std::string projectTypeName;
	ImGui::Text("Project Type search: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(350);
	ImGui::InputText("##ProjectTypeSearch", &projectTypeName);

	ImGui::Text("Project types: ");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##ProjectTypes", ""))
	{
		for (uint32_t i = 0; i < Departments.size(); ++i)
		{
			bool isSelected = selected.find(i) != selected.end();
			if (ImGui::Selectable(Departments[i], isSelected))
			{
				if (!isSelected)
				{
					selected.emplace(i);
				}
				else
				{
					selected.erase(i);
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("Types: ");
	auto typeTextPos = ImGui::GetCursorPos();

	auto padding = ImGui::GetStyle().WindowPadding;
	for (auto id : selected)
	{
		auto curPos = ImGui::GetCursorPos();
		if (curPos.x + padding.x + ImGui::CalcTextSize(Departments[id]).x < ImGui::GetWindowSize().x)
		{
			ImGui::SameLine();
		}
		else
		{
			ImGui::SetCursorPosX(typeTextPos.x);
		}
		auto samePos = ImGui::GetCursorPos();
		ImGui::Text(Departments[id]);
	}

	std::string iconPath;
	ImGui::Text("IconPath: ");
	ImGui::SameLine();
	ImGui::InputText("##IconPath", &iconPath);

	std::string pressReleaseURL;
	ImGui::Text("Press Release URL: ");
	ImGui::SameLine();
	ImGui::InputText("##PressRelease", &pressReleaseURL);

	ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.6, 0, 1});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0, 1});
	if (ImGui::Button("Submit"))
	{
		m_errorMessage = "No database connection";
	}
	ImGui::PopStyleColor(2);
	return 0;
}

const char* ProjectUpdate::name() const { return "Projects"; }

void ProjectUpdate::reset() {}

}  // namespace DataGraph::Forms
