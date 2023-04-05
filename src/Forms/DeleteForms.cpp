#include "DeleteForms.hpp"

#include <imgui.h>

#include "deleteForms/CompanyDeleteForm.hpp"
#include "deleteForms/DepartmentsDeleteForm.hpp"
#include "deleteForms/OwnerDeleteForm.hpp"
#include "deleteForms/ProjectDeleteForm.hpp"

namespace DataGraph::Forms
{
int DeleteForm::init()
{
	subForms.emplace_back(new CompanyDelete);
	subForms.emplace_back(new DepartmentDelete);
	subForms.emplace_back(new OwnerDelete);
	subForms.emplace_back(new ProjectDelete);
	return 0;
}

int DeleteForm::draw()
{
	static int lastForm = -1;
	ImGui::BeginTabBar("Delete");
	for (uint16_t i = 0; i < subForms.size(); ++i)
	{
		if (ImGui::BeginTabItem(subForms[i]->name()))
		{
			if (i != lastForm)
			{
				lastForm = i;
				subForms[lastForm]->reset();
			}
			subForms[i]->draw();
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

	return 0;
}

const char* DeleteForm::name() const { return "Delete"; }
void DeleteForm::reset() {}

std::string_view DeleteForm::getStatusMessage() const
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

int DeleteForm::getStatusCode() const
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
