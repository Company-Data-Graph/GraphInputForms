#include "UpdateForms.hpp"

#include <imgui.h>

#include "UpdateForms/CompanyUpdateForm.hpp"
#include "UpdateForms/DepartmentsUpdateForm.hpp"
#include "UpdateForms/OwnerUpdateForm.hpp"
#include "UpdateForms/ProjectUpdateForm.hpp"

namespace DataGraph::Forms
{
int UpdateForm::init()
{
	subForms.emplace_back(new CompanyUpdate);
	subForms.emplace_back(new DepartmentUpdate);
	subForms.emplace_back(new OwnerUpdate);
	subForms.emplace_back(new ProjectUpdate);
	return 0;
}

int UpdateForm::draw()
{
	ImGui::BeginTabBar("Update");
	for (auto form : subForms)
	{
		if (ImGui::BeginTabItem(form->name()))
		{
			form->draw();
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

	return 0;
}

const char* UpdateForm::name() const { return "Update"; }
void UpdateForm::reset()
{}
}  // namespace DataGraph::Forms
