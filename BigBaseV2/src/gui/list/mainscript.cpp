#include "MainScript.hpp"
#include "invoker/natives.hpp"
#include "UI/Timer.hpp"
#include "UI/UIManager.hpp"
#include "UI/option/ToggleOption.hpp"
#include "UI/option/ToggleWithChoose.hpp"
#include "UI/option/ToggleWithNumber.hpp"
#include "UI/option/ChooseOption.hpp"
#include "UI/option/NumberOption.hpp"
#include "UI/option/ButtonOption.hpp"
#include "UI/option/SubOption.hpp"
#include "UI/submenu/RegularSubmenu.hpp"
#include "UI/submenu/PlayerSubmenu.hpp"
#include "UI/option/BreakOption.hpp"
#include "submenu.hpp"
#include "Discord/handler.h"
#include "Features/Features.hpp"
#include "Backend/Backend.hpp"
#include "gta/texture/texture.hpp"

void ScriptEventLog()
{
	HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
	HUD::BEGIN_TEXT_COMMAND_THEFEED_POST((char*)"STRING");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Open Key ~g~[F5] ~w~BigBase Loaded");
	HUD::END_TEXT_COMMAND_THEFEED_POST_MESSAGETEXT_WITH_CREW_TAG_AND_ADDITIONAL_ICON("Virtual_X_T", (char*)"notify", true, 5, (char*)"~p~BigBase V3", (char*)"", 1.f, (char*)"", 5, 0);
	HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(false, false);
}
using namespace big::UserInterface;
namespace big
{
	bool toggle_ex = false;
	std::vector<const char*> name_cs = {
		{"XIFI"},
		{"Poseidon" },
		{"Neptune"}
	};
	int name_ex = 0;
	int number_ex = 0;
	void MainScript::Initialize()
	{
		//Auth(); 

		if (true)
		{
			ScriptEventLog();
			m_Initialized = true;

			g_UiManager->add<regular_submenu>("Home", Submenu::SubmenuHome, [](regular_submenu* sub) {
				add_option<submenu>("Self", "", Submenu::SubmenuSelf);
				add_option<submenu>("Players", "", Submenu::SubmenuPlayers);
				add_option<submenu>("Weapon", "", Submenu::SubmenuSelf);
				add_option<submenu>("Vehicle", "", Submenu::SubmenuSelf);
				add_option<submenu>("Network", "", Submenu::SubmenuSelf);
				add_option<submenu>("Teleport", "", Submenu::SubmenuSelf);
				add_option<submenu>("Recovery", "", Submenu::SubmenuSelf);
				add_option<submenu>("Protection", "", Submenu::SubmenuSelf);
				add_option<submenu>("Misc", "", Submenu::SubmenuSelf);
				add_option<submenu>("Settings", "", Submenu::SubmenuSettings);
				add_option<toggle>("Click UI", "", &m_switch_ui);
				});
			g_UiManager->add<regular_submenu>("Players", Submenu::SubmenuPlayers, [](regular_submenu* sub) {
				add_option<button>("Button Option", "this button option", [] {
					PED::CLONE_PED(PLAYER::PLAYER_PED_ID(), false, false, false);
					});
				add_option<seperator>("Break Option");
				add_option<toggle>("Toggle Option", "this Toggle Option", &toggle_ex);
				add_option<choose>("Choose Option", "this Choose Option", name_cs, &name_ex);
				add_option<number<int>>("Number Option", "this Number Option", &number_ex, 0, 255);
				add_option<toggle_with_choose>("Toggle With Choose Option", "this Choose Option", &toggle_ex, name_cs, &name_ex);
				add_option<toggle_with_number<int>>("Toggle With Number Option", "This Toggle With Number Option", &toggle_ex, &number_ex, 0, 255);
				});
			g_UiManager->add<regular_submenu>("Settings", Submenu::SubmenuSettings, [](regular_submenu* sub) {
				add_option<button>("Button Option", "this button option", [] {
					PED::CLONE_PED(PLAYER::PLAYER_PED_ID(), false, false, false);
					});
				add_option<toggle>("Bar", "", &g_UiManager->m_options.m_bar);
				add_option<toggle>("Rounding", "", &g_UiManager->m_rounded_options);
				add_option<number<float>>("Header Rounding", "", &g_UiManager->m_rounding.m_header, 0, 20);
				add_option<number<float>>("Footer Rounding", "", &g_UiManager->m_rounding.m_footer, 0, 20);
				add_option<number<float>>("Bar Width", "", &g_UiManager->m_scrollbar.m_width2, 0, 255);
				add_option<seperator>("Break Option");
				add_option<choose>("Scroll Bar type", "", g_UiManager->m_scrollbar.m_type, &g_UiManager->m_scrollbar.m_type_pos);
				add_option<number<float>>("Scroll Bar Offsets", "", &g_UiManager->m_scrollbar.m_bar_offset, 0, 255);
				add_option<number<float>>("Scroll Bar Width", "", &g_UiManager->m_scrollbar.m_width, 0, 255);
				add_option<number<float>>("Bar Width", "", &g_UiManager->m_scrollbar.m_width2, 0, 255);
				add_option<toggle_with_choose>("Toggle With Choose Option", "this Choose Option", &toggle_ex, name_cs, &name_ex);
				add_option<toggle_with_number<int>>("Toggle With Number Option", "This Toggle With Number Option", &toggle_ex, &number_ex, 0, 255);
				});

		}
	}


	void MainScript::Tick()
	{
		g_MainScript->Initialize();
		while (true)
		{
			g_Discord->Tick();
			g_UiManager->check_for_input();
			g_UiManager->handle_input();
			loop();
			script::get_current()->yield();
		}
	}
}