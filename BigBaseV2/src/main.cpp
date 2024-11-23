#include "common.hpp"
#include "util/fiber/fiber_pool.hpp"
#include "renderer/gui.hpp"
#include "util/logger.hpp"
#include "hooking/hooking.hpp"
#include "pointers/pointers.hpp"
#include "renderer/renderer.hpp"
#include "util/script/script_mgr.hpp"
#include "UI/UIManager.hpp"
#include "gui/list/mainscript.hpp"
#include "UI/settings.hpp"
#include "Discord/handler.h"
#include "util/threads_pool.hpp"
#include "util/input/input.hpp"
#include "Backend/Backend.hpp"
#include "hooking/native/native_hooks.hpp"
#include "gta/texture/texture.hpp"
#include "gta/game.hpp"

bool disable_anticheat_skeleton()
{
	bool patched = false;
	for (rage::game_skeleton_update_mode* mode =
		big::g_pointers->m_game_skeleton->m_update_modes;
		mode;
		mode = mode->m_next)
	{
		for (rage::game_skeleton_update_base* update_node = mode->m_head; update_node;
			update_node = update_node->m_next)
		{
			if (update_node->m_hash != RAGE_JOAAT("Common Main"))
				continue;
			rage::game_skeleton_update_group* group = reinterpret_cast<rage::game_skeleton_update_group*>(update_node);
			for (rage::game_skeleton_update_base* group_child_node = group->m_head; group_child_node;
				group_child_node = group_child_node->m_next)
			{
				// TamperActions is a leftover from the old AC, but still useful to block anyway
				if (group_child_node->m_hash != 0xA0F39FB6 && group_child_node->m_hash != RAGE_JOAAT("TamperActions"))
					continue;
				patched = true;
				// LOG(INFO) << "Patching problematic skeleton update";
				reinterpret_cast<rage::game_skeleton_update_element*>(group_child_node)
					->m_function = big::g_pointers->m_nullsub;
			}
			break;
		}
	}

	for (rage::skeleton_data& i : big::g_pointers->m_game_skeleton->m_sys_data)
	{
		if (i.m_hash != 0xA0F39FB6 && i.m_hash != RAGE_JOAAT("TamperActions"))
			continue;
		i.m_init_func = reinterpret_cast<uint64_t>(big::g_pointers->m_nullsub);
		i.m_shutdown_func = reinterpret_cast<uint64_t>(big::g_pointers->m_nullsub);
	}
	LOG(INFO) << "Successfuly patched game skeleton";
	return patched;
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{

		DisableThreadLibraryCalls(hmod);

		g_hmodule = hmod;
		g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
		{
			while (!m_hWindow)m_hWindow = FindWindow(L"grcWindow", nullptr);
				std::this_thread::sleep_for(20ms);

			auto logger_instance = std::make_unique<logger>();
			try
			{
				LOG(RAW) << R"kek(
 ______  _       ______                         ______  
(____  \(_)     (____  \                       (_____ \ 
 ____)  )_  ____ ____)  )_____  ___ _____ _   _ _____) )
|  __  (| |/ _  |  __  ((____ |/___) ___ | | | (_____ ( 
| |__)  ) ( (_| | |__)  ) ___ |___ | ____|\ V / _____) )
|______/|_|\___ |______/\_____(___/|_____) \_/ (______/ 
          (_____|              
)kek";
				auto thread_pool_instance = std::make_unique<thread_pool>();
				auto pointers_instance = std::make_unique<pointers>();
				auto renderer_instance = std::make_unique<renderer>();
				auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
				auto hooking_instance = std::make_unique<hooking>();
				LOG(INFO) << "Components initialized.";

				while (!disable_anticheat_skeleton())
				{
					LOG(INFO) << "Failed to patch game skeleton";
					std::this_thread::sleep_for(100ms);
				}

				g_UiManager = std::make_unique<UserInterface::UIManager>();
				g_MainScript = std::make_shared<MainScript>();
				LOG(INFO) << "UI Components Initialized";

				g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));
				g_script_mgr.add_script(std::make_unique<script>(&MainScript::Tick));
				g_script_mgr.add_script(std::make_unique<script>(&Backend::texture));
				g_script_mgr.add_script(std::make_unique<script>(&Backend::globals));
				LOG(INFO) << "Scripts registered.";


				g_hooking->enable();
				LOG(INFO) << "Hooking enabled.";

				g_Settings.Initialize();
				LOG(INFO) << "Settings Loaded.";

				if (!Input::Initialize()) return false;

				if (*g_pointers->m_loading_screen_state != eLoadingScreenState::Finished)
				{
					LOG(INFO) << "Waiting Loading Screen";
					while (*g_pointers->m_loading_screen_state != eLoadingScreenState::Finished)
					{
						std::this_thread::sleep_for(1000ms);
					}
				}

				auto native_hooks_instance = std::make_unique<native_hooks>();
				LOG(INFO) << "Native Hooks Loaded";

				g_Discord->Init();

				while (g_running)
				{
				     if (GetAsyncKeyState(VK_END)) g_running = false;
				     std::this_thread::sleep_for(200ms);
				}

				g_hooking->disable();
				LOG(INFO) << "Hooking disabled.";

				std::this_thread::sleep_for(200ms);

				g_script_mgr.remove_all_scripts();
				LOG(INFO) << "Scripts unregistered.";

				thread_pool_instance->destroy();
				thread_pool_instance.reset();
				native_hooks_instance.release();
				hooking_instance.reset();
				fiber_pool_instance.reset();
				renderer_instance.reset();
				pointers_instance.reset();
				g_MainScript.reset();
				g_UiManager.reset();
				g_Discord->Shutdown();
				LOG(INFO) << "Components uninitialized.";
			}
			catch (std::exception const &ex)
			{
				LOG(FATAL) << ex.what();
				MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			}

			LOG(INFO) << "Farewell!";
			logger_instance.reset();

			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_hmodule, 0);
		}, nullptr, 0, &g_main_thread_id);
	}

	return true;
}
