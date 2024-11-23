#pragma once
#include "common.hpp"
#include "UI/option/AbstractOption.hpp"

namespace big::UserInterface {
	class abstract_submenu {
	public:
		virtual ~abstract_submenu() noexcept = default;
		virtual std::string get_name() = 0;
		virtual std::uint32_t get_id() = 0;
		virtual void Execute() = 0;
		virtual void Reset() = 0;
		virtual abstract_option* get_option(std::size_t i) = 0;
		virtual std::size_t get_num_options() = 0;
		virtual std::size_t get_num_unscrollable_options() = 0;
		virtual std::size_t get_selected_option() = 0;
		virtual void set_selected_option(std::size_t) = 0;
	public:
		void scroll_forward() {
			if (get_selected_option() < get_num_options() - 1)
				set_selected_option(get_selected_option() + 1);
			else
				set_selected_option(0);
		}
		void scroll_backward() {
			if (get_selected_option() > 0)
				set_selected_option(get_selected_option() - 1);
			else
				set_selected_option(get_num_options() - 1);
		}
	};
	inline std::vector<std::unique_ptr<abstract_option>> g_options;
}
