#pragma once
#include "../Common.hpp"
#include "Types.hpp"
#include "TextBox.hpp"
#include "UI/submenu/AbstractSubmenu.hpp"

#include "invoker/natives.hpp"
#include "nlohmann/json.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include "renderer/renderer.hpp"

namespace big::UserInterface
{
	enum class Font : int32_t {
		RobotoRegular = 0,
		RobotoBold = 1,
		RobotoItalic = 2,
		Arial = 3,
		ArialBold = 4,
		ArialItalic = 5,
		ArialBoldItalic = 6,
		ArialBlack = 7,
		Tahoma = 8,
		Notifcation = 9,
	};
	struct rect {
		float m_top{};
		float m_bottom{};
		float m_left{};
		float m_right{};
	};

	class UIManager
	{
	public:
		void switch_to_submenu(std::uint32_t id) {
			for (auto&& sub : all_submenus) {
				if (sub->get_id() == id) {
					submenu_stack.push(sub.get());
					return;
				}
			}
		}
		template <typename submenu_type, typename ...targs>
		void add(targs&&... args) {
			auto sub{ std::make_unique<submenu_type>(std::forward<targs>(args)...) };
			if (submenu_stack.empty()) {
				submenu_stack.push(sub.get());
			}
			all_submenus.push_back(std::move(sub));
		}

		template <typename t>
		t* get(std::uint32_t id) {
			for (auto&& sub : all_submenus) {
				if (sub->get_id() == id) {
					return (t*)sub.get();
				}
			}
			return (t*)nullptr;
		}
	public:
		bool m_opened{};
		float m_pos_x{ 0.12f };
		float m_pos_y{ 0.15f };
		float m_width = 0.20f;
		int m_options_per_page{ 11 };
		bool m_sounds{ true };
		bool m_overlay_on{ true };
		bool m_using_keyboard{};
		bool m_toggle{ false };
		int m_current_option{};
		float m_current_coord{ m_lerped_pos_y };

		float m_lerped_pos_x{ 0 };
		float m_lerped_pos_y{ 0 };
	public:
		// Input
		std::int32_t m_open_delay{ 200 };
		std::int32_t m_back_delay{ 300 };
		std::int32_t m_enter_delay{ 300 };
		std::int32_t m_vertical_delay{ 120 };
		std::int32_t m_horizontal_delay{ 120 };
	public:
		// Header
		bool m_has_header_loaded{ false };
		std::string m_header_image{};
		float m_header_height{ 0.124f };
		float m_header_text_size{ 1.f };
		Font m_header_font{ Font::RobotoBold };
		Color m_header_background_color{ 17, 17, 17, 255 };
		Color m_header_image_color{ 255, 255, 255, 255 };
		Color m_header_text_color{ 255, 255, 255, 255 };
	public:
		// Smooth Scroll
		float m_smooth_scroll_speed{ 0.13f };
		bool m_smooth_scroll_moves_text{ true };

		//Class
		bool m_scroll_forward{ false };
		bool m_scroll_backward{ false };
	public:
		// Submenu bar
		float m_submenu_bar_height{ 0.035f };
		float m_submenu_bar_text_size{ 1 };
		Font m_submenu_bar_font{ Font::ArialBold };
		float m_submenu_bar_padding{ 2.1f };
		Color m_submenu_bar_background_color{ 20, 20, 20, 255 };
		Color m_submenu_bar_text_color{ 255, 255, 255, 255 };
	public:
		// Notifications
		Font m_notifications_font{ Font::ArialBold };
		Font m_notifications2_font{ Font::Notifcation };
		float m_notifications_text_size{ 0.81f };
		float m_notifications_text_size2{ 0.67f };
	public:
		// Options
		bool m_rounded_options{ false };
		float m_rounded_options_mult{ 10 };
		Font m_break_font{ Font::ArialItalic };
		float m_toggle_size{ 0.035f };
		float m_option_height{ 0.033 };
		float m_option_text_size{ 0.88f };
		float m_submenu_arrow_size{ 0.02f };
		float m_option_break_size{ 0.90f };
		float m_option_icon_size{ 0.027f };
		float m_submenu_rect_width{ 10.f };
		Color m_toggle_on_color{ 0, 0, 0, 255 };
		Color m_toggle_off_color{ 255, 255, 255, 255 };
		Color m_submenu_rect_color{ 94, 94, 94, 180 };
		Font m_option_font{ Font::Arial };
		float m_option_padding{ 2.1f };
		Color m_option_selected_sprite_color{ 10, 10, 10, 255 };
		Color m_option_unselected_sprite_color{ 255, 255, 255, 255 };
		Color m_option_selected_text_color{ 255, 255, 255, 255 };
		Color m_option_unselected_text_color{ 255, 255, 255, 255 };
		Color m_option_selected_background_color{ 100, 100, 100, 230 };
		Color m_option_unselected_background_color{ 14, 14, 16, 225 };
		Color m_red_color{ 255, 0, 0, 255 };
		float m_toggle_size_x_offset{ 0 };
		float m_toggle_size_y_offset{ 0 };
	public:
		// Footer
		float m_footer_sprite_size{ 0.00f };
		float m_footer_height{ 0.015f };
		Color m_footer_background_color{ 0, 0, 0, 255 };
		Color m_footer_sprite_color{ 20, 20, 20, 255 };
	public:
		// Description
		float m_description_height_padding{ 0.01f };
		float m_description_line_height = 0.004f;
		float m_description_height{ 0.030f };
		float m_description_text_size{ 0.93f };
		Color m_description_line_color{ 255, 255, 255, 255 };
		Font m_description_font{ Font::Arial };
		Color m_description_background_color{ 20, 20, 20, 255 };
		Color m_description_text_color{ 255, 255, 255, 255 };
		float m_description_sprite_size{ 0.025f };
		float m_description_padding{ 2.1f };
		Color m_description_sprite_color{ 255, 255, 255, 255 };
		std::string m_default_theme_name;
		std::string m_theme_name;
		struct rounding {
			float m_footer{ 12.f };
			float m_header{ 12.f };
		} m_rounding;
		struct header {
			bool m_uses_gradiant{ true };
		} m_header;

		struct footer {
			bool m_enabled{ true };
			bool m_dynamic{};

			bool m_freeze{};
			std::vector<const char*> m_freeze_state{ "Both", "Up", "Down" };
			int m_freeze_selected{ 0 };
		} m_footer;

		struct chooses {
			bool m_show_count{};

			std::vector<const char*> m_anchor{ "Left", "Right" };
			int m_anchor_selected{ 0 };
			bool m_show_brackets{};
		} m_chooses;
		std::vector<const char*> m_choose_vector;
		std::string m_choose_name{ "None" };
		int* m_choose_selected;

		struct scrollbar {
			bool m_enabled{ true };

			float m_width{ 0.007 };
			float m_width2{ 0.005 };
			float m_offset{ -0.105 };
			float m_bar_offset{ 0 };

			std::vector<const char*> m_type{ "Option Count", "Selected Option" };
			int m_type_pos{ 1 };
		} m_scrollbar;

		struct options {
			bool m_bar{ true };

			Color m_first_color{ 0, 255, 255, 255 };
			Color m_second_color{ 107, 5, 255, 255 };

			bool m_gradiant_selected_text{ false };
		} m_options;

		bool m_open_key_pressed;
		bool m_back_key_pressed;
		bool m_enter_key_pressed;
		bool m_up_key_pressed;
		bool m_down_key_pressed;
		bool m_left_key_pressed;
		bool m_right_key_pressed;

		bool m_hotkey_key_pressed;
		bool m_hotkey_pressed;
		int m_last_key_pressed{ 86 };

		float m_draw_base_y;
	public:
		bool m_disable_input{ false };
		float m_target = { 0.f };
		float m_current = { 0.f };
		float m_speed = { 0.10f };
		float m_menu_animation_speed{ 0.10f };

		float m_fadespeed = { 0.4281f };

		float m_bg_target = { 0.f };
		float m_bg_current = { 0.f };
		bool number_type_open{ false };
		bool keyboard_open{ false };
		char keyboard_text_buffer[256] = "";
		char text_buffer[256] = "";
		int vertical_count{ 0 };
		int horizontal_count{ 0 };
		bool get_og_values{ false };
		float delays[2];
		bool m_lines{ true };
		bool m_fly_in{ false };
		bool m_menu_opening_animation = false;
		bool m_menu_closing_animation = false;
		int m_menu_alpha = 255;
		int m_g_menu_opacity = m_option_unselected_background_color.a;
		float adjusted_description_height{ 0 };
	public:
		UIManager();
		void OnTick();
		void draw_header();
		void draw_submenu_bar(abstract_submenu* sub);
		void draw_option(abstract_option* opt, abstract_submenu* sub, bool selected);
		void draw_background(abstract_submenu* sub);
		void draw_footer();
		void draw_description();
		void draw_overlay();
		void draw_lines();
		void draw_scrollbar(abstract_submenu* sub);
	public:
		void draw_sprite_round(ImTextureID id, float x, float y, float width, float height, Color color, ImDrawList* drawlist, float rounding, ImDrawFlags flags);
		void draw_sprite(ImTextureID id, float x, float y, float width, float height, Color color, float rotation, ImDrawList* drawlist = ImGui::GetBackgroundDrawList(), float rounding = 0.f, ImDrawFlags flags = ImDrawFlags_None);
		void draw_rect(float x, float y, float width, float height, Color color, ImDrawList* drawlist = ImGui::GetBackgroundDrawList(), float rounding = 0.f, ImDrawFlags flags = ImDrawFlags_None);
		void draw_gradiant_rect(float x, float y, float width, float height, Color colorr, Color ccolor2, ImDrawList* drawlist = ImGui::GetBackgroundDrawList());
		void draw_gradiant_rect2(float x, float y, float width, float height, Color colorr, Color ccolor2, ImDrawList* drawlist = ImGui::GetBackgroundDrawList());

		void draw_ram_text(const std::string& text, ImVec2 pos, float size, ImFont* font, Color color, bool outline, bool shadow, float wrap = -1.f, bool is_gradient = false);
		void draw_left_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap = -1.f, bool is_gradient = false);
		void draw_centered_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap = -1.f, bool is_gradient = false);
		void draw_right_text(std::string text, float x, float y, float size, Font font, Color color, bool outline, bool shadow, float wrap = -1.f);

		void queue_sound(std::string name);
		void check_for_input();
		void handle_input();
		void reset_input();
	public:
		void smooth_lerp(float* current, float target, float speed) {
			if (*current != target)
				*current = *current * (1 - speed) + target * speed;
			if ((*current > target - 0.0005) && (*current < target + 0.0005))
				*current = target;
		}
		rect get_rect_bounds(float base_x, float base_y, float height, float width) {
			return { base_y, base_y + height, base_x - (width / 2.f), base_x + (width / 2.f) };
		}
		ImU32 InterpolateColor(ImU32 color1, ImU32 color2, float t) {
			ImVec4 col1 = ImGui::ColorConvertU32ToFloat4(color1);
			ImVec4 col2 = ImGui::ColorConvertU32ToFloat4(color2);

			ImVec4 result;
			result.x = col1.x + (col2.x - col1.x) * t;
			result.y = col1.y + (col2.y - col1.y) * t;
			result.z = col1.z + (col2.z - col1.z) * t;
			result.w = col1.w + (col2.w - col1.w) * t;
			return ImGui::ColorConvertFloat4ToU32(result);
		}
		ImVec2 resolution() {
			return ImGui::GetIO().DisplaySize;
		}
		ImFont* get_font_from_id(Font font) {
			switch (font) {
			case Font::RobotoRegular: return g_renderer->m_Roboto; break;
			case Font::RobotoBold: return g_renderer->m_RobotoBold; break;
			case Font::RobotoItalic: return g_renderer->m_RobotoItalic; break;
			case Font::Arial: return g_renderer->m_Arial; break;
			case Font::ArialBold: return g_renderer->m_ArialBold; break;
			case Font::ArialItalic: return g_renderer->m_ArialItalic; break;
			case Font::ArialBoldItalic: return g_renderer->m_ArialBoldItalic; break;
			case Font::ArialBlack: return g_renderer->m_ArialBlack; break;
			case Font::Tahoma: return g_renderer->m_Tahoma; break;
			case Font::Notifcation: return g_renderer->m_notification_font; break;
			}
			return g_renderer->m_Arial;
		}
		Vector2 get_text_size(Font font, float size, std::string text, float wrap) {
			ImVec2 Res{ resolution() };
			ImFont* Font{ get_font_from_id(font) };
			ImVec2 TextSize{ Font->CalcTextSizeA(Font->FontSize * get_font_scale(size), FLT_MAX, wrap, text.c_str(), NULL) };
			TextSize.x = IM_FLOOR(TextSize.x + 0.99999999999f);
			return { TextSize.x / Res.x, TextSize.y / Res.y };
		}
		float get_text_height(Font font, float size, float wrap) {
			Vector2 FontSize{ get_text_size(font, size * 2.f, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", wrap) };
			return FontSize.x;
		}
		Vector2 get_sprite_scale(float size) {
			ImVec2 Res{ resolution() };
			return { (Res.y / Res.x) * size, size };
		}
		ImVec2 get_text_scale(std::string text) {
			return { ImGui::CalcTextSize(text.c_str()).x / resolution().x, ImGui::CalcTextSize(text.c_str()).y / resolution().y };
		}
		ImVec2 get_scale_sprite(float size) {
			return { (static_cast<float>(resolution().y) / static_cast<float>(resolution().x)) * size, size };
		}
		float get_font_scale(float size) {
			return size * (resolution().x / 1920.f);
		}
		ImVec2 convert_to_pixels(ImVec2 value) {
			return resolution() * value;
		}
		ImVec2 convert_to_floating_point(ImVec2 value) {
			return resolution() / value;
		}
		void smooth_lerp_text_move(float* current, float target, float speed) {
			if (*current != target) {
				*current = *current * (1 - speed) + target * speed;
			}
			if ((*current > target - 0.01f) && (*current < target + 0.01f)) {
				*current = target;
			}
		}
		std::size_t last_selected_option = -1;
		void move_text_option(std::size_t option_id, bool is_selected, bool option_changed) {
			const float target_max_size = 0.92f;
			const float target_min_size = 0.88f;
			const float smooth_speed = 0.03f;

			static std::unordered_map<std::size_t, float> size_deltas;
			static std::unordered_map<std::size_t, bool> anim_directions;

			if (size_deltas.find(option_id) == size_deltas.end()) {
				size_deltas[option_id] = target_min_size;
				anim_directions[option_id] = true;
			}

			if (option_changed) {
				size_deltas[option_id] = target_min_size;
				anim_directions[option_id] = true;
			}

			float target_size = anim_directions[option_id] ? target_max_size : target_min_size;

			smooth_lerp_text_move(&size_deltas[option_id], target_size, smooth_speed);

			if (size_deltas[option_id] >= target_max_size - 0.01f) {
				anim_directions[option_id] = false;
			}
			if (size_deltas[option_id] <= target_min_size + 0.01f) {
				anim_directions[option_id] = true;
			}

			m_option_text_size = size_deltas[option_id];
		}
	public:
		std::vector<std::unique_ptr<abstract_submenu>> all_submenus;
		std::stack<abstract_submenu*, std::vector<abstract_submenu*>> submenu_stack;

	};
};

namespace big {
	inline std::unique_ptr<UserInterface::UIManager> g_UiManager{};
	template <typename OptionType, typename ...TArgs>
	void add_option(TArgs&&... args) {
		UserInterface::g_options.push_back(std::make_unique<OptionType>(std::forward<TArgs>(args)...));
	}
}