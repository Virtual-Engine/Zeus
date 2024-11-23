#pragma once
#include "BaseOption.hpp"
#include "UI/UIManager.hpp"

namespace big::UserInterface {
    template <typename NumberType = float>
    class toggle_with_number : public base_option<toggle_with_number<NumberType>>
    {
    public:
        explicit toggle_with_number() = default;
        explicit toggle_with_number(std::string text, std::string description = "", bool* b00l = nullptr, NumberType* number = nullptr, NumberType min = 0, NumberType max = 0, NumberType step = 1, std::size_t precision = 3, bool actionOnHorizontal = true, const char* prefix = "", const char* suffix = "", std::function<void()> action = [] {}) :
            m_Text(text),
            m_ActionOnHorizontal(actionOnHorizontal),
            m_Bool(b00l),
            m_Number(number),
            m_Min(min),
            m_Max(max),
            m_Step(step),
            m_Precision(precision),
            m_action(action) {
            Base::set_left_text(text);
            Base::set_description(description);
            Base::set_action(std::move(action));
            Base::handle_action(*m_Bool ? option_action::EnterPress : option_action::EnterPress);
            std::strncpy(&m_Prefix[0], prefix, sizeof(m_Prefix) - 1);
            std::strncpy(&m_Suffix[0], suffix, sizeof(m_Suffix) - 1);
        }

    public:
        ~toggle_with_number() noexcept = default;
        toggle_with_number(toggle_with_number const&) = default;
        toggle_with_number& operator=(toggle_with_number const&) = default;
        toggle_with_number(toggle_with_number&&) = default;
        toggle_with_number& operator=(toggle_with_number&&) = default;
    public:
        bool get_flag(option_flag flag) override {
            if (flag == option_flag::ToggleWithChoose) {
                g_UiManager->m_toggle = *m_Bool;
                return true;
            }
            return Base::get_flag(flag);
        }
        std::string get_right_text() override {
            char buf[128]{};
            MemoryStringStream stream(buf);
            stream
                << std::setprecision(m_Precision)
                << std::fixed
                << m_Prefix
                << static_cast<DisplayType>(*m_Number)
                << "/"
                << std::setprecision(m_Precision)
                << std::fixed
                << static_cast<DisplayType>(m_Max)
                << m_Suffix;
            Base::m_right_text = buf;
            return Base::get_right_text();
        }
        void handle_action(option_action action) override {
            if (action == option_action::EnterPress)
                *m_Bool = !*m_Bool;
            if (action == option_action::LeftPress) {
                if (*m_Number - m_Step < m_Min)
                    *m_Number = m_Max;
                else
                    *m_Number -= m_Step;
                if (m_ActionOnHorizontal && Base::m_Action)
                    std::invoke(Base::m_Action);
            }
            else if (action == option_action::RightPress) {
                if (*m_Number + m_Step > m_Max)
                    *m_Number = m_Min;
                else
                    *m_Number += m_Step;
                if (m_ActionOnHorizontal && Base::m_Action)
                    std::invoke(Base::m_Action);
                Base::handle_action(action);
            }
        }
        bool* get_bool() override {
            return m_Bool;
        }
        std::function<void()> get_toggle_action() override {
            return m_action;
        }
    public:
        std::string m_Text;
    private:
        char m_Prefix[32] = {};
        char m_Suffix[32] = {};
        bool m_ActionOnHorizontal{};
        bool* m_Bool;
        NumberType* m_Number{};
        NumberType m_Min{};
        NumberType m_Max{};
        NumberType m_Step{};
        std::size_t m_Precision{};
        std::function<void()> m_action{};
    private:
        using Base = base_option<toggle_with_number<NumberType>>;
        using DisplayType = std::conditional_t<sizeof(NumberType) == 1, std::uint32_t, NumberType>;
    };
}