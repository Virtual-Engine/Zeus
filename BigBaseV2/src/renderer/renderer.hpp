#pragma once
#include "common.hpp"
#include "gta/grcTextureStore.hpp"
#include <imgui.h>
#include "ressources.hpp"

namespace big
{
	namespace stb {
		namespace memory {
			extern image_data write(vec2& size, int comp, const void* data, int strideBytes, uint32_t delay);
			extern image read(image_data data);
			extern gif read_gif(image_data data);
		}
		namespace file {
			extern image read_image(std::filesystem::path path);
			extern gif read_gif(std::filesystem::path path);
		}
		extern std::vector<image_data> split_gif(std::filesystem::path path);
	}
	using dx_callback = std::function<void()>;
	using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;
	struct imgDimensions
	{
		int     x, y;
		imgDimensions() { x = y = 0; }
		imgDimensions(int _x, int _y) { x = _x; y = _y; }
	};
	class renderer
	{
	public:
		explicit renderer();
		~renderer();

		void on_present();

		void pre_reset();
		void post_reset();

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void ressource();

		std::pair<shader_data, vec2> create_texture(std::filesystem::path path);
		std::vector<frame_data> create_gif_texture(std::filesystem::path path);
		shader_data create_shader_data(image image);
		std::map<int, frame_data> createGifTexture(std::filesystem::path path);

		void create_imfont(ImFontAtlas* Atlas, ImFont*& font, std::filesystem::path path, float size, ImFontConfig* config) {
			font = Atlas->AddFontFromFileTTF(path.string().c_str(), size, config);
		}
		template <size_t dataSize>
		void create_imfont(ImFontAtlas* Atlas, ImFont*& font, uint8_t(&Data)[dataSize], float size, ImFontConfig* config) {
			font = Atlas->AddFontFromMemoryTTF(Data, dataSize, size, config);
		}
	public:
		ImFont* m_font;
		ImFont* m_monospace_font;

		ImFont* m_Roboto{};
		ImFont* m_RobotoBold{};
		ImFont* m_RobotoItalic{};
		ImFont* m_Arial{};
		ImFont* m_notification_font{};
		ImFont* m_ArialSmall{};
		ImFont* m_ArialItalic{};
		ImFont* m_ArialBold{};
		ImFont* m_ArialBoldItalic{};
		ImFont* m_ArialBlack{};
		ImFont* m_Tahoma{};
		ImFont* m_smaller_arial{};
		ImFont* m_title_font{};
		ImFont* m_icon_font{};
		ImFont* m_titlebar_font{};
		ImFontConfig m_FontCfg;
	public:
		resources m_resources{};
	public:
		bool AddDXCallback(dx_callback callback, uint32_t priority);
		std::map<uint32_t, dx_callback> m_dx_callbacks;
	private:
		comptr<IDXGISwapChain> m_dxgi_swapchain;
		comptr<ID3D11Device> m_d3d_device;
		comptr<ID3D11DeviceContext> m_d3d_device_context;
	};

	inline renderer* g_renderer{};
}
