#include "common.hpp"
#include "base_tab.h"
#include "imgui.h"
#include "util/script/script.hpp"
#include "util/fiber/fiber_pool.hpp"
#include "invoker/natives.hpp"
#include "util/gta_util.hpp"

namespace big
{
	void base_tab::render_base_tab()
	{
		bool o_bool{};
		const double min = 0., max = 10.;
		int slider = 0;
		const char* const demo_combo[]
		{
			"One",
			"Two",
			"Three"
		};

		if (ImGui::Begin("BigBase"))
		{
			ImGui::Checkbox("List UI", &m_switch_ui);
			ImGui::SliderInt("Int", &slider, 0, 10);

			ImGui::SameLine();

			if (ImGui::Button("Unload"))
			{
				g_running = false;
			}
		}
		ImGui::End();
	}
}
