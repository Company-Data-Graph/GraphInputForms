#include "InsertForms.hpp"

#include <imgui.h>

#include "insertForms/CompanyInsertForm.hpp"
#include "insertForms/DepartmentsInsertForm.hpp"
#include "insertForms/OwnerInsertForm.hpp"
#include "insertForms/ProjectInsertForm.hpp"

namespace DataGraph::Forms
{
int InsertForm::init()
{
	subForms.emplace_back(new CompanyInsert);
	subForms.emplace_back(new DepartmentInsert);
	subForms.emplace_back(new OwnerInsert);
	subForms.emplace_back(new ProjectInsert);
	return 0;
}

int InsertForm::draw()
{
	ImGui::BeginTabBar("Insert");
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

const char* InsertForm::name() const { return "Insert"; }
void InsertForm::reset()
{}
}  // namespace DataGraph::Forms
