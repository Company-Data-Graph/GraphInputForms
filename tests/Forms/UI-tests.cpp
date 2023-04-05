#include "UI-tests.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <imgui_te_engine.h>
#include <imgui_te_ui.h>
#include <imgui_te_context.h>
#include <imgui_te_internal.h>

#include <Core/FormHandler.hpp>


namespace DataGraph::tests
{
static bool isWindowVisible = false;
static bool isTestsEnabled = false;
static bool isAutoTestEnabled = false;

void setWindowVisibility(bool isVisible) { isWindowVisible = isVisible; }

void registerImguiTests(ImGuiTestEngine* testEngine)
{
	if (!isTestsEnabled)
	{
		return;
	}

	auto test = IM_REGISTER_TEST(testEngine, "Login", "IncorrectCredentials");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("AuthForm", ImGuiMouseButton_Left);
		baseRef += "AuthForm";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DatabaseIP", "127.0.0.1");
		ctx->ItemInputValue("##MediaServerIP", "127.0.0.1");
		ctx->ItemInputValue("##DatabasePort", "127.0.0.1");
		ctx->ItemInputValue("##MediaServerPort", "127.0.0.1");
		ctx->ItemInputValue("##DatabaseLogin", "127.0.0.1");
		ctx->ItemInputValue("##MediaServerLogin", "127.0.0.1");
		ctx->ItemInputValue("##DatabasePassword", "127.0.0.1");
		ctx->ItemInputValue("##MediaServerPassword", "127.0.0.1");

		ctx->ItemClick("Connect", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_NE(ui->getStatusCode(), 0);
	};

	test = IM_REGISTER_TEST(testEngine, "Login", "SuccessfulLogin");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("AuthForm", ImGuiMouseButton_Left);
		baseRef += "/AuthForm";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DatabaseIP", "127.0.0.1");
		ctx->ItemInputValue("##MediaServerIP", "127.0.0.1");
		ctx->ItemInputValue("##DatabasePort", "5435");
		ctx->ItemInputValue("##MediaServerPort", "8082");
		ctx->ItemInputValue("##DatabaseLogin", "postgres");
		ctx->ItemInputValue("##MediaServerLogin", "admin");
		ctx->ItemInputValue("##DatabasePassword", "example");
		ctx->ItemInputValue("##MediaServerPassword", "example");

		ctx->ItemClick("Connect", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_EQ(ui->getStatusCode(), 0);
	};

	test = IM_REGISTER_TEST(testEngine, "Search", "Departments");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Departments", ImGuiMouseButton_Left);
		baseRef += "/Departments";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DepartmentsSearch", u8"Транспорт");
		ctx->ItemClick("##DepartmentsList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->ItemClick(u8"//##Combo_00//Транспорт", ImGuiMouseButton_Left);
	};

	test = IM_REGISTER_TEST(testEngine, "Search", "Company");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Companies", ImGuiMouseButton_Left);
		baseRef += "/Companies/Companies";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##CompanyNameSearch", u8"СБЕР");
		ctx->ItemClick("##CompaniesList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->ItemClick(u8"//##Combo_00//СБЕР", ImGuiMouseButton_Left);
	};

	test = IM_REGISTER_TEST(testEngine, "Search", "Owner");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Owners", ImGuiMouseButton_Left);
		baseRef += "/Owners/Owners";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##OwnerSearch", u8"Сергей Паньков");
		ctx->ItemClick("##OwnersList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->ItemClick(u8"//##Combo_00//Сергей Паньков", ImGuiMouseButton_Left);
	};

	test = IM_REGISTER_TEST(testEngine, "OwnerUpdate", "SuccessfulChangeName");
	test->UserData = FormHandler::getUI().get();
	struct UpdateTestData
	{
		bool firstRun = true;
		std::string_view nameFrom;
		std::string_view nameTo;
	};

	test->SetVarsDataType<UpdateTestData>();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		auto& metaData = ctx->GetVars<UpdateTestData>();
		if (metaData.firstRun)
		{
			metaData.nameFrom = u8"Сергей Паньков";
			metaData.nameTo = u8"Сергей ПаньковUpdated";
			ctx->WindowFocus("GraphFormWindow");
		}
		else
		{
			metaData.nameFrom = u8"Сергей ПаньковUpdated";
			metaData.nameTo = u8"Сергей Паньков";
		}

		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Owners", ImGuiMouseButton_Left);
		baseRef += "/Owners/Owners";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##OwnerSearch", metaData.nameFrom.data());
		ctx->ItemClick("##OwnersList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->SetRef("//$FOCUSED");
		ctx->ItemClick(metaData.nameFrom.data(), ImGuiMouseButton_Left);

		baseRef = baseRef.substr(0, baseRef.rfind("/Owners"));
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##NewOwnerName", metaData.nameTo.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_EQ(ui->getStatusCode(), 0);

		if (metaData.firstRun)
		{
			metaData.firstRun = false;
			ctx->Test->TestFunc(ctx);
		}
	};

	test = IM_REGISTER_TEST(testEngine, "OwnerUpdate", "NameAlreadyExists");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->WindowFocus("GraphFormWindow");
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Owners", ImGuiMouseButton_Left);
		baseRef += "/Owners/Owners";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##OwnerSearch", u8"Сергей Паньков");
		ctx->ItemClick("##OwnersList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->ItemClick(u8"//##Combo_00//Сергей Паньков", ImGuiMouseButton_Left);

		baseRef = baseRef.substr(0, baseRef.rfind("/Owners"));
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##NewOwnerName", u8"Герман Оскарович Греф");

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_NE(ui->getStatusCode(), 0);
	};

	test = IM_REGISTER_TEST(testEngine, "DepartmentUpdate", "SuccessfulChangeName");
	test->UserData = FormHandler::getUI().get();

	test->SetVarsDataType<UpdateTestData>();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		auto& metaData = ctx->GetVars<UpdateTestData>();
		if (metaData.firstRun)
		{
			metaData.nameFrom = u8"Транспорт";
			metaData.nameTo = u8"ТранспортUpdated";
			ctx->WindowFocus("GraphFormWindow");
		}
		else
		{
			metaData.nameFrom = u8"ТранспортUpdated";
			metaData.nameTo = u8"Транспорт";
		}

		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Departments", ImGuiMouseButton_Left);
		baseRef += "/Departments";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DepartmentsSearch", metaData.nameFrom.data());
		ctx->ItemClick("##DepartmentsList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->SetRef("//$FOCUSED");
		ctx->ItemClick(metaData.nameFrom.data(), ImGuiMouseButton_Left);

		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DepartmentsNewName", metaData.nameTo.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_EQ(ui->getStatusCode(), 0);

		if (metaData.firstRun)
		{
			metaData.firstRun = false;
			ctx->Test->TestFunc(ctx);
		}
	};

	test = IM_REGISTER_TEST(testEngine, "DepartmentUpdate", "NameAlreadyExists");
	test->UserData = FormHandler::getUI().get();
	test->TestFunc = [](ImGuiTestContext* ctx) {
		std::string baseRef = "//$FOCUSED/GraphForms";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		baseRef += "Update/Update/Update";
		ctx->SetRef(baseRef.data());

		ctx->ItemClick("Departments", ImGuiMouseButton_Left);
		baseRef += "/Departments";
		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DepartmentsSearch", u8"Транспорт");
		ctx->ItemClick("##DepartmentsList", ImGuiMouseButton_Left);

		ctx->WindowFocus("//##Combo_00");
		ctx->SetRef("//$FOCUSED");
		ctx->ItemClick(u8"Транспорт", ImGuiMouseButton_Left);

		ctx->SetRef(baseRef.data());
		ctx->ItemInputValue("##DepartmentsNewName", "IT");

		ctx->ItemClick("Update", ImGuiMouseButton_Left);
		auto ui = (UI*)(ctx->Test->UserData);
		IM_CHECK_NE(ui->getStatusCode(), 0);
	};
}

void enableAutoTests() { isAutoTestEnabled = true; }

bool isAutoTestsEnabled() { return isAutoTestEnabled; }

void setupWindowParams() { glfwWindowHint(GLFW_VISIBLE, isWindowVisible); }

void enableUITests() { isTestsEnabled = true; }
bool isUITestsEnabled() { return isTestsEnabled; }

std::vector<ImguiTestStatus> parseUITests()
{
	auto engine = FormHandler::getUI()->m_engine;
	std::vector<ImguiTestStatus> result(engine->TestsAll.Size);
	for (uint32_t test_n = 0; test_n < engine->TestsAll.Size; ++test_n)
	{
		ImGuiTest* test = engine->TestsAll[test_n];
		result[test_n] = {test->Category, test->Name, test->Status, (test->EndTime - test->StartTime) / 1000000, &test->TestLog};
	}

	return result;
}
}  // namespace DataGraph::tests

class ImguiTest: public testing::TestWithParam<DataGraph::tests::ImguiTestStatus>
{
protected:
	decltype(DataGraph::tests::ImguiTestStatus::duration) duration;

	void SetUp() override
	{
		const auto& test_data = GetParam();
		duration = test_data.duration;
		if (test_data.status == ImGuiTestStatus::ImGuiTestStatus_Success)
		{
			SUCCEED();
		}
		else
		{
			auto message = "Failed to Run test.";
			if (!test_data.logBuffer->IsEmpty())
			{
				message = test_data.logBuffer->Buffer.c_str();
			}

			FAIL() << message;
		}
	}

public:
	static std::string PrintToStringParamName(const ::testing::TestParamInfo<DataGraph::tests::ImguiTestStatus>& info)
	{
		return fmt::format("{}_{}", info.param.category, info.param.name);
	}
};

TEST_P(ImguiTest, Test) {}

INSTANTIATE_TEST_CASE_P(UItests, ImguiTest, testing::ValuesIn(DataGraph::tests::parseUITests()), ImguiTest::PrintToStringParamName);
