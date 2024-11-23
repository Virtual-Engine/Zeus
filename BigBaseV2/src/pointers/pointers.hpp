#pragma once
#include "common.hpp"
#include "gta/fwddec.hpp"
#include "gta/enums.hpp"
#include "function_types.hpp"
#include "gta/game.hpp"
#include "gta/grcTexture.hpp"
#include "gta/grcTextureFactory.hpp"
#include "gta/grcTextureStore.hpp"

namespace big
{
	enum class eLoadingScreenState : int32_t
	{
		Invalid = -1,
		Finished,
		PreLegal,
		Unknown_2,
		Legals,
		Unknown_4,
		LandingPage,
		Transition,
		Unknown_7,
		Unknown_8,
		Unknown_9,
		SessionStartLeave
	};

	class pointers
	{
	public:
		explicit pointers();
		~pointers();
	public:
		HWND m_hwnd{};

		eLoadingScreenState* m_loading_screen_state{};

		bool *m_is_session_started{};

		CPedFactory **m_ped_factory{};
		CNetworkPlayerMgr **m_network_player_mgr{};

		PVOID m_init_native_tables;
		rage::scrNativeRegistrationTable *m_native_registration_table{};
		functions::get_native_handler_t m_get_native_handler{};
		functions::fix_vectors_t m_fix_vectors{};

		rage::atArray<GtaThread*> *m_script_threads{};
		rage::scrProgramTable *m_script_program_table{};
		functions::run_script_threads_t m_run_script_threads{};
		std::int64_t **m_script_globals{};

		CGameScriptHandlerMgr **m_script_handler_mgr{};

		IDXGISwapChain **m_swapchain{};

		PVOID m_model_spawn_bypass;

		functions::setup_dictionary_t m_setup_dictionary;
		functions::add_texture_to_dictionary_t m_add_texture_to_dictionary;
		functions::setup_store_item_t m_setup_store_item;
		functions::add_dictionary_to_pool_t m_add_dictionary_to_pool;
		rage::grcTextureStore* m_texture_store;
		rage::grcTextureFactory** m_texture_factory;

		rage::game_skeleton* m_game_skeleton;
		void (*m_nullsub)();
	};

	inline pointers *g_pointers{};
}
