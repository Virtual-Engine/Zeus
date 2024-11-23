#pragma once
#include "BaseOption.hpp"
#include "UI/UIManager.hpp"

namespace big::UserInterface {
	class toggle_with_choose : public base_option<toggle_with_choose> {
	public:
		explicit toggle_with_choose() = default;
		explicit toggle_with_choose(std::string text, std::string description, bool* b00l, std::vector<const char*> vec, int* position, bool actionOnHorizontal = false, std::function<void()> action = [] {}) :
			m_Text(text),
			m_Bool(b00l),
			m_Data(vec), m_DataSize(vec.size()),
			m_Position(position),
			m_ActionOnHorizontal(actionOnHorizontal),
			m_action(action)
		{
			Base::set_left_text(text);
			Base::set_description(description);
			Base::set_action(std::move(action));
		}
	public:
		std::string  get_right_text() override {
			std::string  buf{};
			if (!m_Data.empty()) {
				if (g_UiManager->m_chooses.m_show_count) {
					switch (g_UiManager->m_chooses.m_anchor_selected) {
					case 0:
						buf = std::format("[{}/{}] {}", std::to_string(*m_Position + 1), std::to_string(m_DataSize), m_Data[*m_Position]);
						break;
					case 1:
						buf = std::format("{} [{}/{}]", m_Data[*m_Position], std::to_string(*m_Position + 1), std::to_string(m_DataSize));
						break;
					}
				}
				else {
					buf = m_Data[*m_Position];
				}
			}
			else
				buf = "No Data [0/0]";
			Base::m_right_text = buf;
			return Base::get_right_text();
		}
		bool get_flag(option_flag flag) override {
			if (flag == option_flag::ToggleWithChoose) {
				g_UiManager->m_toggle = *m_Bool;
				return true;
			}
			return Base::get_flag(flag);
		}
		void handle_action(option_action action) override {
			switch (action) {
			case option_action::EnterPress: {
				*m_Bool = !*m_Bool;
			} break;

			case option_action::LeftPress: {
				if (!m_Data.empty()) {
					if (*m_Position > 0)
						--(*m_Position);
					else
						*m_Position = static_cast<std::size_t>(m_DataSize - 1);
					if (m_ActionOnHorizontal && Base::m_Action)
						std::invoke(Base::m_Action);
				}
			} break;

			case option_action::RightPress: {
				if (!m_Data.empty()) {
					if (*m_Position < m_DataSize - 1)
						++(*m_Position);
					else
						*m_Position = 0;
					if (m_ActionOnHorizontal && Base::m_Action)
						std::invoke(Base::m_Action);
				}
			} break;
			}

			if (!m_Data.empty()) Base::handle_action(action);
		}
		bool* get_bool() override {
			return m_Bool;
		}
		std::function<void()> get_toggle_action() override {
			return m_action;
		}
	public:
		~toggle_with_choose() noexcept = default;
		toggle_with_choose(toggle_with_choose const&) = default;
		toggle_with_choose& operator=(toggle_with_choose const&) = default;
		toggle_with_choose(toggle_with_choose&&) = default;
		toggle_with_choose& operator=(toggle_with_choose&&) = default;
	private:
		std::string m_Text;
		std::vector<const char*> m_Data{};
		int m_DataSize{};
		int* m_Position{};
		bool m_ActionOnHorizontal{};

		bool* m_Bool;
		bool m_DisplayInverted = false;
		std::function<void()> m_action{};

		using Base = base_option<toggle_with_choose>;
	};
}