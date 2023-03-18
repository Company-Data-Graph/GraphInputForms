#include <gtest/gtest.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>

#include <Core/FormHandler.hpp>

#include <imgui_te_context.h>

#include <Forms/insertForms/OwnerInsertForm.hpp>

/*
class FormsTestHandler: public ::testing::Test
{
protected:
	virtual void SetUp() { 
		engine = ImGuiTestEngine_CreateContext();
		ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
		test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
		test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;

		DataGraph::FormHandler::getUI()->beginFrame();
	}

	virtual void TearDown()
	{
		DataGraph::FormHandler::getUI()->endFrame();
		ImGuiTestEngine_DestroyContext(engine);
	}

	ImGuiTestEngine* engine;
};


TEST_F(FormsTestHandler, SuccessfulOwneruploadDatabase)
{
	auto t = IM_REGISTER_TEST(engine, "demo_tests", "test2");
	DataGraph::Forms::OwnerInsert owner;

	t->SetVarsDataType<DataGraph::Forms::OwnerInsert>();
	t->GuiFunc = [](ImGuiTestContext* ctx) 
	{
		ImGui::Begin("OwnerInsert");
		auto& vars = ctx->GetVars<DataGraph::Forms::OwnerInsert>();
		vars.draw();
		ImGui::End();
	};

	t->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->SetRef("OwnerInsert");
		ctx->ItemInputValue("##Name", "testValue");
		ctx->ItemClick("Submit");
	};
	ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
	ImGuiTestEngine_QueueTest(engine, t);
	ImGuiTestEngine_QueueTests(engine, ImGuiTestGroup_Unknown);

	ImGuiTestEngine_Stop(engine);
}
*/
