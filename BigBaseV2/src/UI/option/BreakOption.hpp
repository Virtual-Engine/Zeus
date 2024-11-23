#pragma once
#include "BaseOption.hpp"
#include "UI/UIManager.hpp"
namespace big::UserInterface {
	class seperator : public base_option<seperator> {
	public:
		explicit seperator() = default;
		explicit seperator(std::string text) {
			set_center_text(text);
		}
	public:
		bool get_flag(option_flag flag) override {
			if (flag == option_flag::Break)
				return true;
			return base_option::get_flag(flag);
		}
	public:
		~seperator() noexcept = default;
		seperator(seperator const&) = default;
		seperator& operator=(seperator const&) = default;
		seperator(seperator&&) = default;
		seperator& operator=(seperator&&) = default;
	};
}