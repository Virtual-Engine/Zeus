#pragma once
#include "AbstractSubmenu.hpp"
#include "UI/option/AbstractOption.hpp"

namespace big::UserInterface {
	template <typename T>
	class BaseSubmenu : public abstract_submenu {
	public:
		std::string get_name() override {
			return m_name;
		}
		uint32_t get_id() override {
			return m_id;
		}
		void Execute() override {
			if (m_Action)
				std::invoke(m_Action, static_cast<T*>(this));
			if (m_selected_option >= g_options.size())
				m_selected_option = 0;
		}
		void Reset() override {
			g_options.clear();
		}
		abstract_option* get_option(size_t i) override {
			if (i > g_options.size())
				return nullptr;
			return g_options[i].get();
		}

		size_t get_num_options() override {
			return g_options.size();
		}

		size_t get_num_unscrollable_options() override {
			size_t count{};
			if (!get_num_options() || g_options.empty())
				return count;
			for (size_t i{}; i != get_num_options(); ++i) {
				if (g_options[i]->get_flag(option_flag::Break)) {
					count++;
				}
			}
			return count;
		}

		size_t get_selected_option() override {
			return m_selected_option;
		}
	public:
		void set_selected_option(std::size_t n) override {
			m_selected_option = n;
		}
		template <typename OptionType, typename ...TArgs>
		void add_option(TArgs&&... args) {
			g_options.push_back(std::make_unique<OptionType>(std::forward<TArgs>(args)...));
		}
		T& set_name(std::string name) {
			m_name = name;
			return static_cast<T&>(*this);
		}
		T& set_id(std::uint32_t id) {
			m_id = id;
			return static_cast<T&>(*this);
		}
		T& set_action(std::function<void(T*)> action) {
			m_Action = std::move(action);
			return static_cast<T&>(*this);
		}
	protected:
		std::string m_name{};
		uint32_t m_id{};
		std::function<void(T*)> m_Action;

		size_t m_selected_option{};
	};

}
