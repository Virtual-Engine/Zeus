#include "UIManager.hpp"
#include "Timer.hpp"
#include "invoker/natives.hpp"
#include "util/gta_util.hpp"
#include "util/fiber/fiber_pool.hpp"
#include "renderer/gui.hpp"


namespace big::UserInterface
{
	UIManager::UIManager()
	{
		g_renderer->AddDXCallback(
			[this] {
				if (m_switch_ui)
				{
					if (g_gui.m_opened)
					{
						g_gui.dx_on_tick();
					}
				}
				else
				{
					this->OnTick();
				}
			},
			-2);
	}
	void UIManager::OnTick() {

		if (m_fly_in) {
			if (m_menu_closing_animation) {
				smooth_lerp(&m_lerped_pos_x, 0, m_menu_animation_speed);
				smooth_lerp(&m_lerped_pos_y, 0, m_menu_animation_speed);
			}
		}
		if (m_opened) {
			if (!m_menu_closing_animation) {
				smooth_lerp(&m_lerped_pos_x, m_pos_x, m_menu_animation_speed);
				smooth_lerp(&m_lerped_pos_y, m_pos_y, m_menu_animation_speed);
			}
			m_draw_base_y = m_lerped_pos_y;

			draw_header();
			if (!submenu_stack.empty()) {
				auto sub{ submenu_stack.top() };
				sub->Reset();
				sub->Execute();
				draw_submenu_bar(sub);
				draw_background(sub);

				if (m_scrollbar.m_enabled) {
					draw_scrollbar(sub);
				}
				if (sub->get_num_options() != 0) {
					std::size_t start_point{ 0 };
					std::size_t end_point{ sub->get_num_options() > m_options_per_page ? m_options_per_page : sub->get_num_options() };
					if (sub->get_num_options() > m_options_per_page && sub->get_selected_option() >= m_options_per_page - 6) {
						for (int i = 0; i < 6; i++) {
							if (sub->get_selected_option() < sub->get_num_options() - i) {
								start_point = sub->get_selected_option() - m_options_per_page + i + 1;
								end_point = sub->get_selected_option() + i + 1;
							}
						}
					}
					for (std::size_t i = start_point, j = 0; i < end_point; ++i, ++j) {
						bool option_is_selected = (i == sub->get_selected_option());
						bool option_changed = (i == sub->get_selected_option()) && (last_selected_option != i);
						if (option_is_selected) {
							move_text_option(i, true, option_changed);
						}
						else {
							m_option_text_size = 0.88f;
						}
						if (option_is_selected) {
							smooth_lerp(&m_current_coord, m_draw_base_y + (m_option_height / 2.f), m_smooth_scroll_speed);
							draw_rect(m_lerped_pos_x, m_current_coord, m_width, m_option_height, m_option_selected_background_color, ImGui::GetBackgroundDrawList(), m_rounded_options ? m_rounded_options_mult : 0);
						}
						if (m_bg_current >= m_option_height * i || (sub->get_num_options() > m_options_per_page && sub->get_selected_option() >= m_options_per_page - 6)) {
							draw_option(sub->get_option(i), sub, i == sub->get_selected_option());
						}
					}
					last_selected_option = sub->get_selected_option();
				}
				if (m_footer.m_enabled) {
					if ((m_footer.m_dynamic && sub->get_num_options() > m_options_per_page)) {
						draw_footer();
					}

					if (!m_footer.m_dynamic) {
						draw_footer();
					}
				}
				draw_description();
				if (m_lines) {
					draw_lines();
				}
			}
		}
		//draw_overlay();
	}
	void UIManager::handle_input() {
		if (!get_og_values) {
			delays[0] = m_vertical_delay;
			delays[1] = m_horizontal_delay;
			get_og_values = true;
		}
		static Timer open_timer{ 0ms }, back_timer{ 0ms }, enter_timer{ 0ms }, vertical_timer{ 0ms }, horizontal_timer{ 0ms };
		open_timer.SetDelay(std::chrono::milliseconds(m_open_delay));
		back_timer.SetDelay(std::chrono::milliseconds(m_back_delay));
		enter_timer.SetDelay(std::chrono::milliseconds(m_enter_delay));
		vertical_timer.SetDelay(std::chrono::milliseconds(m_vertical_delay));
		horizontal_timer.SetDelay(std::chrono::milliseconds(m_horizontal_delay));
		if (vertical_count > 20)
			m_vertical_delay--;
		else
			m_vertical_delay = delays[0];

		if (horizontal_count > 20)
			m_horizontal_delay--;
		else
			m_horizontal_delay = delays[1];

		if (m_open_key_pressed && open_timer.Update()) {
			m_opened ^= true;
			queue_sound(m_opened ? "SELECT" : "BACK");
		}
		if (m_opened) {
			static Timer hotkey_timer(0ms);
			hotkey_timer.SetDelay(std::chrono::milliseconds(m_back_delay));
			if (m_hotkey_key_pressed && hotkey_timer.Update()) {
				queue_sound("BACK");
				m_hotkey_pressed = !m_hotkey_pressed;
			}
			if (m_back_key_pressed && back_timer.Update() && !m_disable_input) {
				queue_sound("BACK");
				if (submenu_stack.size() <= 1) {
					m_opened = false;
				}
				else {
					submenu_stack.pop();
				}
			}
			if (!submenu_stack.empty()) {
				auto sub{ submenu_stack.top() };
				if (sub->get_num_options()) {
					if (auto opt = sub->get_option(sub->get_selected_option()); opt) {
						if (m_enter_key_pressed && back_timer.Update()) {
							queue_sound("SELECT");
							opt->handle_action(option_action::EnterPress);
						}
						if (vertical_timer.Update() && !m_disable_input) {
							if (m_up_key_pressed || m_down_key_pressed) {
								queue_sound("NAV_UP_DOWN");
								vertical_count++;
							}
							if (!m_up_key_pressed && !m_down_key_pressed)
								vertical_count = 0;

							if (m_up_key_pressed) {
								sub->scroll_backward();
								if (auto opt = sub->get_option(sub->get_selected_option())) {
									if (opt->get_flag(option_flag::Break)) {
										sub->scroll_backward();
									}
								}
							}
							if (m_down_key_pressed) {
								sub->scroll_forward();
								if (auto opt = sub->get_option(sub->get_selected_option())) {
									if (opt->get_flag(option_flag::Break)) {
										sub->scroll_forward();
									}
								}
							}
						}
						if (horizontal_timer.Update() && !m_disable_input && !m_open_key_pressed) {
							if (m_left_key_pressed || m_right_key_pressed)
								horizontal_count++;

							if (!m_left_key_pressed && !m_right_key_pressed)
								horizontal_count = 0;

							if (m_sounds && !opt->get_flag(option_flag::Break))
								if (m_left_key_pressed || m_right_key_pressed)
									queue_sound("NAV_UP_DOWN");
							if (m_left_key_pressed)
								opt->handle_action(option_action::LeftPress);
							if (m_right_key_pressed)
								opt->handle_action(option_action::RightPress);
						}
					}
				}
			}
		}
	}
	void UIManager::check_for_input()
	{
		reset_input();
		m_open_key_pressed = gta_util::IsKeyPressed(VK_F5);
		m_back_key_pressed = gta_util::IsKeyPressed(VK_BACK);
		m_enter_key_pressed = gta_util::IsKeyPressed(VK_RETURN);
		m_up_key_pressed = gta_util::IsKeyPressed(VK_UP);
		m_down_key_pressed = gta_util::IsKeyPressed(VK_DOWN);
		m_left_key_pressed = gta_util::IsKeyPressed(VK_LEFT);
		m_right_key_pressed = gta_util::IsKeyPressed(VK_RIGHT);
	}
	void UIManager::reset_input() {
		m_open_key_pressed = false;
		m_back_key_pressed = false;
		m_enter_key_pressed = false;
		m_up_key_pressed = false;
		m_down_key_pressed = false;
		m_left_key_pressed = false;
		m_right_key_pressed = false;
	}
	void UIManager::draw_header() {
		draw_sprite_round(g_renderer->m_resources["Header"], m_lerped_pos_x, m_draw_base_y + (m_header_height / 2.f), m_width, m_header_height, { 255,255,255,255 }, ImGui::GetBackgroundDrawList(), m_rounding.m_header, ImDrawFlags_RoundCornersTop);
		//draw_centered_text(BRANDING, m_lerped_pos_x, m_draw_base_y + (m_header_height / 2.f) - (get_text_height(m_header_font, m_header_text_size, m_width) / 2.f), m_header_text_size, m_header_font, m_header_text_color, false, true, m_width, m_header.m_uses_gradiant);
		m_draw_base_y += m_header_height;
	}
	void UIManager::draw_submenu_bar(abstract_submenu* sub) {
		std::string left_text{ sub->get_name() };
		std::string right_text{ std::format("{} / {}", sub->get_selected_option() + 1, sub->get_num_options()) };
		std::transform(left_text.begin(), left_text.end(), left_text.begin(), [](char c) { return static_cast<char>(toupper(c)); });

		draw_rect(m_lerped_pos_x, m_draw_base_y + (m_submenu_bar_height / 2.f), m_width, m_submenu_bar_height, m_submenu_bar_background_color);
		draw_left_text(left_text, m_lerped_pos_x - (m_width / m_submenu_bar_padding), m_draw_base_y + (m_submenu_bar_height / 2.f) - (get_text_height(m_submenu_bar_font, m_submenu_bar_text_size, m_width) / 2.f), m_submenu_bar_text_size, m_submenu_bar_font, m_submenu_bar_text_color, false, true);
		draw_right_text(right_text, m_lerped_pos_x + (m_width / m_submenu_bar_padding), m_draw_base_y + (m_submenu_bar_height / 2.f) - (get_text_height(m_submenu_bar_font, m_submenu_bar_text_size, m_width) / 2.f), m_submenu_bar_text_size, m_submenu_bar_font, m_submenu_bar_text_color, false, true);
		m_draw_base_y += m_submenu_bar_height;
	}
	void UIManager::draw_scrollbar(abstract_submenu* sub) {
		float background_y{ m_lerped_pos_y + m_header_height / 2 + m_submenu_bar_height + m_bg_current / 2 };
		float slider_top{ background_y - m_bg_current / 2 };
		float slider_bottom{ background_y + m_bg_current / 2 };
		float slider_position{ ((float)sub->get_selected_option() / ((float)sub->get_num_options() - 1)) * (m_bg_current - m_option_height) + m_option_height / 2 };
		if (sub->get_num_options() < 2) {
			slider_position = m_bg_current - m_option_height + m_option_height / 2;
		}

		static float position{ slider_position };
		static float offset{ m_scrollbar.m_offset };

		smooth_lerp(&position, slider_position, m_smooth_scroll_speed);
		smooth_lerp(&offset, m_scrollbar.m_offset, m_menu_animation_speed);

		draw_rect(m_lerped_pos_x - offset + (m_scrollbar.m_width / 2), m_draw_base_y + (m_bg_current / 2.f), m_scrollbar.m_width, m_bg_current, m_option_unselected_background_color, ImGui::GetBackgroundDrawList(), 5.f);
		draw_rect(m_lerped_pos_x - offset + (m_scrollbar.m_width / 2) - m_scrollbar.m_bar_offset, (m_scrollbar.m_type_pos == 0 ? slider_top + position + 0.05f : m_current_coord), m_scrollbar.m_width2, m_option_height - 0.006f, m_option_selected_background_color, ImGui::GetBackgroundDrawList(), 5.f);
	}
	void UIManager::draw_lines() {
		draw_gradiant_rect(m_lerped_pos_x, m_lerped_pos_y + (m_header_height + m_submenu_bar_height), m_width, 0.001, { m_options.m_second_color.r, m_options.m_second_color.g, m_options.m_second_color.b, m_header_text_color.a }, { m_options.m_first_color.r, m_options.m_first_color.g, m_options.m_first_color.b, m_header_text_color.a });
		draw_gradiant_rect(m_lerped_pos_x, m_lerped_pos_y + m_header_height + m_submenu_bar_height + m_bg_current - 0.001, m_width, 0.001, { m_options.m_second_color.r, m_options.m_second_color.g, m_options.m_second_color.b, m_header_text_color.a }, { m_options.m_first_color.r, m_options.m_first_color.g, m_options.m_first_color.b, m_header_text_color.a });
	}
	void UIManager::draw_option(abstract_option* opt, abstract_submenu* sub, bool selected) {
		float new_y{ m_current_coord - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f) };

		if (m_options.m_bar) {
			float width{ 0.003f };
			draw_gradiant_rect2(m_lerped_pos_x - (m_width / 2.f) + (width / 2.f), m_current_coord, width, m_option_height, { m_options.m_first_color.r, m_options.m_first_color.g, m_options.m_first_color.b, m_header_text_color.a }, { m_options.m_second_color.r, m_options.m_second_color.g, m_options.m_second_color.b, m_header_text_color.a });
		}
		draw_left_text(opt->get_left_text(), m_lerped_pos_x - (m_width / m_option_padding) + (selected && m_options.m_bar ? 0.00201f : 0), (m_smooth_scroll_moves_text && selected) ? new_y : m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f), m_option_text_size, m_option_font, selected ? m_option_selected_text_color : m_option_unselected_text_color, false, false, -1.0f, (selected && m_options.m_gradiant_selected_text) ? true : false);

		if (opt->get_flag(option_flag::Horizontal)) {
			bool keyboard_open{ (number_type_open && selected) };
			Color col{ selected ? m_option_selected_text_color : m_option_unselected_text_color };
			Color col2{ selected ? m_red_color : m_option_unselected_text_color };
			auto texture{ g_renderer->m_resources[keyboard_open ? "Keyboard" : "ArrowsLeftRight"] };
			auto sizeX{ keyboard_open ? 0.011f - m_toggle_size_x_offset : get_sprite_scale(0.033).x };
			auto sizeY{ keyboard_open ? 0.018f - m_toggle_size_y_offset : get_sprite_scale(0.030).y };
			auto offset{ selected ? 0.012 : 0 };
			Color first{ selected ? m_option_selected_text_color : m_option_unselected_text_color };
			Color second{ 255, 0, 0, 255 };

			if (selected)
				draw_sprite(texture, m_lerped_pos_x + (m_width / m_option_padding) - 0.005f, m_draw_base_y + (m_option_height / 2.f), sizeX, sizeY, keyboard_open ? col2 : col, 90.0);

			draw_right_text(opt->get_right_text(), m_lerped_pos_x + (m_width / m_option_padding) - offset, m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f) - 0.001, m_option_text_size, m_option_font, keyboard_open ? col2 : col, false, false);
		}
		else if (opt->get_flag(option_flag::ToggleWithChoose)) {
			auto size{ get_sprite_scale(m_toggle_size) };
			auto texture{ g_renderer->m_resources["ArrowsLeftRight"] };
			if (selected)
				draw_sprite(texture, m_lerped_pos_x + (m_width / m_option_padding) - 0.0173f, m_draw_base_y + (m_option_height / 2.f), get_sprite_scale(0.033).x, get_sprite_scale(0.030).y, m_option_selected_text_color, 90.0);

			draw_sprite(m_toggle ? g_renderer->m_resources["On"] : g_renderer->m_resources["Off"], m_lerped_pos_x + (m_width / m_option_padding) - (size.x / 4.f), m_draw_base_y + (m_option_height / 2.f), 0.011f - m_toggle_size_x_offset, 0.018f - m_toggle_size_y_offset, selected ? m_option_selected_text_color : m_option_unselected_text_color, 0.f);
			draw_right_text(opt->get_right_text(), m_lerped_pos_x + (m_width / m_option_padding) - (selected ? 0.024f : 0.012), m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f), m_option_text_size, m_option_font, selected ? m_option_selected_text_color : m_option_unselected_text_color, false, false);
		}
		else if (opt->get_flag(option_flag::Keyboard)) {
			Color col{ selected ? m_option_selected_text_color : m_option_unselected_text_color };
			Color col2{ selected ? m_red_color : m_option_unselected_text_color };
			auto size{ get_sprite_scale(m_toggle_size) };
			draw_sprite(g_renderer->m_resources["Keyboard"], m_lerped_pos_x + (m_width / m_option_padding) - (size.x / 4.f), m_draw_base_y + (m_option_height / 2.f), 0.011f - m_toggle_size_x_offset, 0.018f - m_toggle_size_y_offset, keyboard_open ? col2 : col, 0.f);
			draw_right_text(opt->get_right_text(), m_lerped_pos_x + (m_width / m_option_padding) - 0.012, m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f), m_option_text_size, m_option_font, keyboard_open ? col2 : col, false, false);
		}
		else
			draw_right_text(opt->get_right_text(), m_lerped_pos_x + (m_width / m_option_padding), m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_option_font, m_option_text_size, m_width) / 2.f), m_option_text_size, m_option_font, selected ? m_option_selected_text_color : m_option_unselected_text_color, false, false);

		if (opt->get_flag(option_flag::Toggle)) {
			auto size{ get_sprite_scale(m_toggle_size) };
			draw_sprite(m_toggle ? g_renderer->m_resources["On"] : g_renderer->m_resources["Off"], m_lerped_pos_x + (m_width / m_option_padding) - (size.x / 4.f), m_draw_base_y + (m_option_height / 2.f), 0.011f - m_toggle_size_x_offset, 0.018f - m_toggle_size_y_offset, selected ? m_option_selected_text_color : m_option_unselected_text_color, 0.f);
		}

		if (opt->get_flag(option_flag::On) || (opt->get_texture() == "on")) {
			auto size{ get_sprite_scale(m_toggle_size) };
			draw_sprite(g_renderer->m_resources["On"], m_lerped_pos_x + (m_width / m_option_padding) - (size.x / 4.f), m_draw_base_y + (m_option_height / 2.f), 0.011f - m_toggle_size_x_offset, 0.018f - m_toggle_size_y_offset, selected ? m_option_selected_text_color : m_option_unselected_text_color, 0.f);
		}

		if (opt->get_flag(option_flag::Enterable)) {
			auto size{ get_sprite_scale(m_submenu_arrow_size) };
			draw_sprite(g_renderer->m_resources["ArrowRight"], m_lerped_pos_x + (m_width / m_option_padding) - (size.x / 4.f), m_draw_base_y + (m_option_height / 2.f), size.x, size.y, selected ? m_option_selected_text_color : m_option_unselected_text_color, 0.f);
		}

		if (opt->get_flag(option_flag::ColorPicker)) {
			auto res{ get_sprite_scale(m_submenu_arrow_size) };
			auto res2{ get_sprite_scale(m_submenu_arrow_size) };
			//draw_sprite(g_renderer->m_resources["ArrowRight"], m_lerped_pos_x + (m_width / m_option_padding) - (res.x / 4.f), m_draw_base_y + (m_option_height / 2.f), res.x, res.y, opt->get_color().as<int>(), 0.f);
		}

		if (opt->get_flag(option_flag::VehicleButton) && selected) {
			//draw_vehicle_info(opt->get_arg());
		}

		if (opt->get_flag(option_flag::Break)) {
			draw_centered_text(opt->get_centered_text(), m_lerped_pos_x, m_draw_base_y + (m_option_height / 2.f) - (get_text_height(m_break_font, m_option_break_size, m_width) / 2.f), m_option_break_size, m_break_font, m_option_unselected_text_color, m_width, false, true);
			if (selected) {
				if (m_scroll_forward)
					sub->scroll_forward();
				else if (m_scroll_backward)
					sub->scroll_backward();
			}
		}

		m_draw_base_y += m_option_height;
	}
	void UIManager::draw_background(abstract_submenu* sub) {
		auto visible_options{ sub->get_num_options() > m_options_per_page ? m_options_per_page : sub->get_num_options() };
		float bg_height{ m_option_height * visible_options };
		m_bg_target = bg_height;

		smooth_lerp(&m_bg_current, m_bg_target, m_speed);

		draw_rect(m_lerped_pos_x, m_draw_base_y + (m_bg_current / 2.f), m_width, m_bg_current, m_option_unselected_background_color);
	}
	float get_rect_base(float rect_height, float draw_base = g_UiManager->m_draw_base_y) {
		return draw_base + (rect_height / 2.f);
	}
	void UIManager::draw_footer() {
		draw_rect(m_lerped_pos_x, m_lerped_pos_y + m_header_height + m_submenu_bar_height + m_bg_current + (m_footer_height / 2), m_width, m_footer_height, m_footer_background_color, ImGui::GetForegroundDrawList(), m_rounding.m_footer, ImDrawFlags_RoundCornersBottom);
		float size{ m_footer_sprite_size };
		float rotation{};
		auto texture{ g_renderer->m_resources["Arrows"] };
		if (!submenu_stack.empty() && !m_footer.m_freeze) {
			auto sub{ submenu_stack.top() };
			if (!sub->get_selected_option()) {
				texture = g_renderer->m_resources["ArrowDown"];
				size *= 0.8f;
			}
			else if (sub->get_selected_option() + 1 == sub->get_num_options()) {
				texture = g_renderer->m_resources["ArrowUp"];
				size *= 0.8f;
			}
		}
		if (m_footer.m_freeze) {
			switch (m_footer.m_freeze_selected) {
			case 0:
				texture = g_renderer->m_resources["Arrows"];
				break;
			case 1:
				texture = g_renderer->m_resources["ArrowUp"];
				break;
			case 2:
				texture = g_renderer->m_resources["ArrowDown"];
				break;
			}
		}
		auto scale{ get_sprite_scale(size) };
		draw_sprite(texture, m_lerped_pos_x, get_rect_base(m_footer_height, m_lerped_pos_y + m_header_height + m_bg_current + m_option_height + 0.0039), scale.x, scale.y, m_footer_sprite_color, rotation);
		m_draw_base_y += m_footer_height;
	}
	void UIManager::draw_description() {
		std::string description;
		if (!submenu_stack.empty()) {
			auto sub{ submenu_stack.top() };
			if (sub->get_num_options()) {
				if (auto opt = sub->get_option(sub->get_selected_option())) {
					description = opt->get_description();
				}
			}
		}
		if (description == "") {
			return;
		}
		auto text_size{ get_text_scale(description) };
		smooth_lerp(&adjusted_description_height, m_description_height_padding + (text_size.y * 0.45f), m_speed);

		m_draw_base_y += m_description_height_padding;
		draw_rect(m_lerped_pos_x, get_rect_base(m_footer_height, m_lerped_pos_y + m_header_height + m_bg_current + m_option_height + 0.022 + 0.005) + (adjusted_description_height / 2), m_width, adjusted_description_height, m_option_unselected_background_color, ImGui::GetForegroundDrawList(), 8.f, ImDrawFlags_RoundCornersBottom);
		draw_left_text(description, m_lerped_pos_x - (m_width / m_description_padding), get_rect_base(m_footer_height, m_lerped_pos_y + m_header_height + m_bg_current + m_option_height + 0.037 + 0.005) - (get_text_height(m_description_font, m_description_text_size, m_width) / 2.f), m_description_text_size, m_description_font, m_header_text_color, false, false);
		draw_gradiant_rect(m_lerped_pos_x, get_rect_base(m_footer_height, m_lerped_pos_y + m_header_height + m_bg_current + m_option_height + 0.022 + 0.005) + 0.0015, m_width, m_description_line_height, { m_options.m_second_color.r, m_options.m_second_color.g, m_options.m_second_color.b, m_header_text_color.a }, { m_options.m_first_color.r, m_options.m_first_color.g, m_options.m_first_color.b, m_header_text_color.a }, ImGui::GetForegroundDrawList());
		m_draw_base_y += adjusted_description_height;
	}
	void UIManager::queue_sound(std::string name) {
		g_fiber_pool->queue_job([name] {
			if (g_UiManager->m_sounds)
				AUDIO::PLAY_SOUND_FRONTEND(-1, name.c_str(), "HUD_FRONTEND_DEFAULT_SOUNDSET", false); 
		});
	}
	void UIManager::draw_ram_text(const std::string& text, ImVec2 pos, float size, ImFont* font, Color color, bool outline, bool shadow, float wrap, bool is_gradient) {
		ImDrawList* DrawList{ ImGui::GetForegroundDrawList() };
		if (is_gradient) {
			float step{ 1.0f / (strlen(text.c_str()) - 1) };

			for (int i{ 0 }; i < strlen(text.c_str()); ++i) {
				auto col{ InterpolateColor(IM_COL32(m_options.m_first_color.r, m_options.m_first_color.g, m_options.m_first_color.b, color.a), IM_COL32(m_options.m_second_color.r, m_options.m_second_color.g, m_options.m_second_color.b, color.a), step * i) };
				DrawList->AddText(font, font->FontSize * get_font_scale(size), pos, col, &text[i], &text[i + 1], wrap);

				pos.x += (ImGui::CalcTextSize(&text[i], &text[i + 1]).x + (resolution().x == 1920 ? (7.00 / resolution().x) : 7.00)); //did lazy way kys
			}
		}
		else {
			DrawList->AddText(font, font->FontSize * get_font_scale(size), pos, color.pack(), text.data(), NULL, wrap);
		}
	}
	void UIManager::draw_sprite(ImTextureID id, float x, float y, float width, float height, Color color, float rotation, ImDrawList* drawlist, float rounding, ImDrawFlags flags) {
		ImVec2 Pos{ convert_to_pixels({ x, y }) };
		ImVec2 Size{ convert_to_pixels({ width, height }) };
		ImVec2 FinalPos{ Pos - (Size / 2.f) };
		ImDrawList* DrawList{ ImGui::GetForegroundDrawList() };
		DrawList->AddImageRounded(id, FinalPos, FinalPos + Size, { 0.f, 0.f }, { 1.f, 1.f }, color.pack(), rounding, flags);
	}
	void UIManager::draw_sprite_round(ImTextureID id, float x, float y, float width, float height, Color color, ImDrawList* drawlist, float rounding, ImDrawFlags flags) {
		ImVec2 Pos{ convert_to_pixels({ x, y }) };
		ImVec2 Size{ convert_to_pixels({ width, height }) };
		ImVec2 FinalPos{ Pos - (Size / 2.f) };
		ImDrawList* DrawList{ ImGui::GetForegroundDrawList() };
		DrawList->AddImageRounded(id, FinalPos, FinalPos + Size, { 0.f, 0.f }, { 1.f, 1.f }, color.pack(), rounding, flags);
	}
	void UIManager::draw_rect(float x, float y, float width, float height, Color color, ImDrawList* drawlist, float rounding, ImDrawFlags flags) {
		auto pos{ convert_to_pixels({ x, y }) };
		auto size{ convert_to_pixels({ width, height }) };
		auto final_pos{ pos - (size / 2.f) };
		drawlist->AddRectFilled(final_pos, final_pos + size, color.pack(), rounding, flags);
	}
	void UIManager::draw_gradiant_rect(float x, float y, float width, float height, Color colorr, Color ccolor2, ImDrawList* drawlist) {
		auto pos{ convert_to_pixels({ x, y }) };
		auto size{ convert_to_pixels({ width, height }) };
		auto final_pos{ pos - (size / 2.f) };
		drawlist->AddRectFilledMultiColor(final_pos, final_pos + size, ccolor2.pack(), colorr.pack(), colorr.pack(), ccolor2.pack());
	}
	void UIManager::draw_gradiant_rect2(float x, float y, float width, float height, Color colorr, Color ccolor2, ImDrawList* drawlist) {
		auto pos{ convert_to_pixels({ x, y }) };
		auto size{ convert_to_pixels({ width, height }) };
		auto final_pos{ pos - (size / 2.f) };
		drawlist->AddRectFilledMultiColor(final_pos, final_pos + size, colorr.pack(), colorr.pack(), ccolor2.pack(), ccolor2.pack());
	}
	void UIManager::draw_left_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap, bool is_gradient) {
		ImVec2 Wrap{ convert_to_pixels({ wrap, wrap }) };
		ImFont* Font{ get_font_from_id(font) };
		ImVec2 Pos{ convert_to_pixels({ x, y }) };
		draw_ram_text(text, Pos, size, Font, color, outline, shadow, Wrap.y, is_gradient);
	}
	void UIManager::draw_centered_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap, bool is_gradient) {
		ImVec2 Wrap{ convert_to_pixels({ wrap, wrap }) };
		Vector2 TextSize{ get_text_size(font, size, text, Wrap.y) };
		draw_left_text(text, x - (TextSize.x / 2.f), y, size, font, color, outline, shadow, wrap, is_gradient);
	}
	void UIManager::draw_right_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap) {
		ImVec2 Wrap{ convert_to_pixels({ wrap, wrap }) };
		Vector2 TextSize{ get_text_size(font, size, text, Wrap.y) };
		draw_left_text(text, x - TextSize.x, y, size, font, color, outline, shadow, wrap);
	}
}
