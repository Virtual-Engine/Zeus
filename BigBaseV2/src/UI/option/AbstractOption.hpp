#pragma once
#include "common.hpp"

namespace big::UserInterface {
	enum class option_action {
		LeftPress,
		RightPress,
		EnterPress
	};
	enum class option_flag {
		Horizontal = (1 << 0),
		Enterable = (1 << 1),
		Toggle = (1 << 2),
		Break = (1 << 3),
		ToggleWithChoose = (1 << 4),
		Keyboard = (1 << 5),
		On = (1 << 6),
		ColorPicker = (1 << 7),
		VehicleButton = (1 << 8)
	};
	class abstract_option {
	public:
		virtual ~abstract_option() noexcept = default;
		virtual std::string get_left_text() = 0;
		virtual std::string get_right_text() = 0;
		virtual std::string get_centered_text() = 0;
		virtual std::string get_description() = 0;
		virtual std::string get_texture() = 0;
		virtual std::string get_arg() = 0;
		virtual bool* get_bool() = 0;
		virtual std::function<void()> get_button_action() = 0;
		virtual std::function<void()> get_toggle_action() = 0;
		virtual Vector2 get_texture_size() = 0;
		virtual void handle_action(option_action action) = 0;
		virtual bool get_flag(option_flag flag) = 0;
		virtual Color get_color() = 0;
	};
}
