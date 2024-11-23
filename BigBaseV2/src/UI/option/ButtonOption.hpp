#pragma once
#include "BaseOption.hpp"

namespace big::UserInterface {
	class button : public base_option<button> {
	public:
		button() = default;
		button(std::string text, std::string description, std::function<void()> action = [] {}) :
			m_action(action) {
			set_left_text(text);
			set_description(description);
			set_action(std::move(action));
		}
		button(std::string text) {
			set_left_text(text);
		}
	public:
		button& add_action(std::function<void()> action) {
			set_action(std::move(action));
			return *this;
		}
		button& add_texture(const char* texture) {
			set_texture(texture);
			return *this;
		}
	public:
		std::function<void()> get_button_action() override {
			return m_action;
		}
	private:
		std::function<void()> m_action;
	};
}