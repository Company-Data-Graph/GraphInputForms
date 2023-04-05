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
	ImGui::BeginTabBar(name());
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

const char* UpdateForm::name() const { return "Update"; }
void UpdateForm::reset() {}
std::string_view UpdateForm::getStatusMessage() const
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

int UpdateForm::getStatusCode() const
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
