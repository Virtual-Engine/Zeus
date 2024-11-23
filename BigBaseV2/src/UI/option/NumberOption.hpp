#pragma once
#include "BaseOption.hpp"
#include "UI/UIManager.hpp"
#include "common.hpp"
#include "util/fiber/fiber_pool.hpp"

namespace big::UserInterface {
	template <typename T>
	struct number_struct {
		T min;
		T max;
		T step;
		T precision;
	};
	template <typename num_type>
	class number : public base_option<number<num_type>> {
	public:
		number() = default;
		number(std::string text, std::string description, num_type* number, num_type min, num_type max, num_type step = 1, std::size_t precision = 3, bool action_on_horizontal = true, std::string prefix = "", std::string suffix = "", std::function<void()> action = [] {}) :
			m_action_on_horizontal(action_on_horizontal), m_number(number), m_min(min), m_max(max), m_step(step), m_precision(precision) {
			Base::set_left_text(text);
			Base::set_description(description);
			Base::set_action(std::move(action));
			m_prefix = prefix;
			m_suffix = suffix;
		}
		number(std::string text, num_type* number, num_type min, num_type max, num_type step = 1, std::size_t precision = 3, bool action_on_horizontal = true, std::string prefix = "", std::string suffix = "", std::function<void()> action = [] {}) :
			m_action_on_horizontal(action_on_horizontal), m_number(number), m_min(min), m_max(max), m_step(step), m_precision(precision) {
			Base::set_left_text(text);
			Base::set_action(std::move(action));
			m_prefix = prefix;
			m_suffix = suffix;
		}
		number(std::string text, bool action_on_horizontal, std::string prefix = "", std::string suffix = "", std::function<void()> action = [] {}) :
			m_name(text), m_action_on_horizontal(action_on_horizontal) {
			Base::set_left_text(text);
			Base::set_action(std::move(action));
			m_prefix = prefix;
			m_suffix = suffix;
		}

		number& add_number(num_type* num, number_struct<num_type> num_struct) {
			m_number = num;
			m_min = num_struct.min;
			m_max = num_struct.max;
			m_step = num_struct.step;
			m_precision = num_struct.precision;
			return *this;
		}
		number& add_number(num_type& num, number_struct<num_type> num_struct) {
			m_number = &num;
			m_min = num_struct.min;
			m_max = num_struct.max;
			m_step = num_struct.step;
			m_precision = num_struct.precision;
			return *this;
		}

		number& add_action(std::function<void()> action) {
			std::function<void()> action2 = [=] {
				g_fiber_pool->queue_job([=] {
					action();
					});
				};
			Base::set_action(std::move(action2));
			return *this;
		}

		std::string get_right_text() override {
			char buf[128]{};
			MemoryStringStream stream(buf);
			stream << std::setprecision(m_precision) << std::fixed << m_prefix << static_cast<display_type>(*m_number) << m_suffix;
			Base::m_right_text = buf;
			return Base::get_right_text();
		}
		bool get_flag(option_flag flag) override {
			if (flag == option_flag::Horizontal)
				return true;
			return Base::get_flag(flag);
		}
		void handle_action(option_action action) override {
			switch (action) {
			case option_action::LeftPress:
				*m_number = (*m_number - m_step < m_min) ? m_max : (*m_number - m_step);
				if (m_action_on_horizontal && Base::m_Action)
					std::invoke(Base::m_Action);
				break;

			case option_action::RightPress:
				*m_number = (*m_number + m_step > m_max) ? m_min : (*m_number + m_step);
				if (m_action_on_horizontal && Base::m_Action)
					std::invoke(Base::m_Action);
				break;

			case option_action::EnterPress:
				if (strlen(g_UiManager->text_buffer) > 0 && g_UiManager->number_type_open) {
					const char* test = g_UiManager->text_buffer;
					*m_number = std::stof(test);
					if (!m_action_on_horizontal && Base::m_Action)
						std::invoke(Base::m_Action);
					g_UiManager->number_type_open = false;
					g_UiManager->m_disable_input = false;
				}
				else {
					std::snprintf(g_UiManager->text_buffer, sizeof(g_UiManager->text_buffer), "%f", *m_number);
					g_UiManager->m_disable_input = true;
					g_UiManager->number_type_open = true;
				}
				break;

			default:
				break;
			}

			Base::handle_action(action);
		}
	private:
		std::string m_prefix{};
		std::string m_suffix{};
		bool m_action_on_horizontal{};
		num_type* m_number{};
		num_type m_min{};
		num_type m_max{};
		num_type m_step{};
		std::size_t m_precision{};
	public:
		std::string m_name{};
	private:
		using Base = base_option<number<num_type>>;
		using display_type = std::conditional_t<sizeof(num_type) == 1, std::uint32_t, num_type>;
	};
}