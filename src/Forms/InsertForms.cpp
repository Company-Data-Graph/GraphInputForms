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
			m_activeForm = form->name();
			form->draw();
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

	return 0;
}

const char* InsertForm::name() const { return "Insert"; }
void InsertForm::reset() {}

std::string_view InsertForm::getStatusMessage() const
{
	for (auto form : subForms)
	{
		if (form->name() != m_activeForm)
		{
			continue;
		}

		if (form->getStatusCode() != -1)
		{
			return form->getStatusMessage();
		}
	}

	return "";
}

int InsertForm::getStatusCode() const
{
	for (auto form : subForms)
	{
		if (form->name() != m_activeForm)
		{
			continue;
		}

		if (auto code = form->getStatusCode(); code != -1)
		{
			return code;
		}
	}

	return -1;
}
}  // namespace DataGraph::Forms
