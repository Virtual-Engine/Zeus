#include "common.hpp"
#include "renderer/shader/fonts.hpp"
#include "util/logger.hpp"
#include "gui.hpp"
#include "pointers/pointers.hpp"
#include "renderer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui_internal.h>
#include "stb_image.h"
#include "renderer/stb_image_write.h"
#include "ressources.hpp"
#include "UI/UIManager.hpp"
#include "gta/texture/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define VK_X 0x58

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer::renderer() :
		m_dxgi_swapchain(*g_pointers->m_swapchain)
	{
		void* d3d_device{};
		if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
		{
			m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
		}
		else
		{
			throw std::runtime_error("Failed to get D3D device.");
		}

		m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());

		auto file_path = std::filesystem::path(std::getenv("appdata"));
		file_path /= "BigBaseV2";
		if (!std::filesystem::exists(file_path))
		{
			std::filesystem::create_directory(file_path);
		}
		else if (!std::filesystem::is_directory(file_path))
		{
			std::filesystem::remove(file_path);
			std::filesystem::create_directory(file_path);
		}
		file_path /= "imgui.ini";

		ImGuiContext* ctx = ImGui::CreateContext();

		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename = path.c_str();

		ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;
		std::strcpy(font_cfg.Name, "Rubik");

		m_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 20.f, &font_cfg);
		m_monospace_font = ImGui::GetIO().Fonts->AddFontDefault();
		std::filesystem::path path_font{ std::filesystem::path("C:").append("Windows").append("Fonts") };
		std::filesystem::path basePath{ std::filesystem::path("C:\\BigBaseV2\\")};
		std::filesystem::path fontPath{ std::filesystem::path(basePath).append("Fonts") };
		std::filesystem::path headerPath{ std::filesystem::path(basePath).append("Headers") };
		ImFontAtlas* atlas{ ImGui::GetIO().Fonts };
		float globalSize{ 60.f };
		m_FontCfg.FontDataOwnedByAtlas = false;
		create_imfont(atlas, m_Roboto, std::filesystem::path(fontPath).append("Roboto-Regular.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_RobotoBold, std::filesystem::path(fontPath).append("Roboto-Bold.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_RobotoItalic, std::filesystem::path(fontPath).append("Roboto-Italic.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_Arial, std::filesystem::path(path).append("Arial.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_ArialSmall, std::filesystem::path(path).append("Arial.ttf"), 25.f, &m_FontCfg);
		create_imfont(atlas, m_ArialBold, std::filesystem::path(path).append("ArialBD.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_ArialBoldItalic, std::filesystem::path(path).append("ArialBI.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_ArialBlack, std::filesystem::path(path).append("AriBLK.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_ArialItalic, std::filesystem::path(path).append("ArialI.ttf"), globalSize, &m_FontCfg);
		create_imfont(atlas, m_Tahoma, std::filesystem::path(path).append("Tahoma.ttf"), globalSize, &m_FontCfg);
		m_Arial = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 19.f, &m_FontCfg);
		m_notification_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 19.f, &m_FontCfg);
		m_ArialItalic = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\ArialI.ttf", 19.f, &m_FontCfg);
		m_ArialBold = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\ArialBD.ttf", 19.f, &m_FontCfg);
		m_smaller_arial = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 12.0f, &m_FontCfg);
		m_title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 11.5f, &m_FontCfg);
		m_icon_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 19.f, &m_FontCfg);
		m_titlebar_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 19.f, &m_FontCfg);
		ressource();
		g_gui.dx_init();
		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_renderer = nullptr;
	}

	bool renderer::AddDXCallback(dx_callback callback, uint32_t priority) {
		if (!m_dx_callbacks.insert({ priority, callback }).second) {
			LOG(INFO) << "Duplicate priority given on DX Callback!";

			return false;
		}
		return true;
	}

	void renderer::on_present()
	{
		if (g_gui.m_opened)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		for (auto&& [_, callback] : m_dx_callbacks) {
			callback();
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (m_switch_ui)
		{
			if (msg == WM_KEYUP && wparam == VK_INSERT)
			{
				//Persist and restore the cursor position between menu instances.
				static POINT cursor_coords{};
				if (g_gui.m_opened)
				{
					GetCursorPos(&cursor_coords);
				}
				else if (cursor_coords.x + cursor_coords.y != 0)
				{
					SetCursorPos(cursor_coords.x, cursor_coords.y);
				}

				g_gui.m_opened ^= true;
			}
		}
		else
		{
			if (msg == WM_KEYUP && wparam == 'X')
			{
				static POINT cursor_coords{};
				if (g_gui.m_opened)
				{
					GetCursorPos(&cursor_coords);
				}
				else if (cursor_coords.x + cursor_coords.y != 0)
				{
					SetCursorPos(cursor_coords.x, cursor_coords.y);
				}
				g_gui.m_opened ^= true;
			}
		}


		if (g_gui.m_opened)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
		}
	}

	void renderer::ressource()
	{
		std::filesystem::path resourcePath{ std::filesystem::path(std::getenv("appdata")).append("BigBaseV2").append("Ressources") };
		m_resources.add({ "Header", create_texture(std::filesystem::path(resourcePath).append("Header.png")).first });
		m_resources.add({ "ArrowRight", create_texture(std::filesystem::path(resourcePath).append("ArrowRight.png")).first });
		m_resources.add({ "ArrowUp", create_texture(std::filesystem::path(resourcePath).append("ArrowUp.png")).first });
		m_resources.add({ "Arrows", create_texture(std::filesystem::path(resourcePath).append("Arrows.png")).first });
		m_resources.add({ "ArrowDown", create_texture(std::filesystem::path(resourcePath).append("ArrowDown.png")).first });
		m_resources.add({ "ArrowsLeftRight", create_texture(std::filesystem::path(resourcePath).append("ArrowsLeftRight.png")).first });
		m_resources.add({ "On", create_texture(std::filesystem::path(resourcePath).append("On.png")).first });
		m_resources.add({ "Off", create_texture(std::filesystem::path(resourcePath).append("Off.png")).first });
		m_resources.add({ "Keyboard", create_texture(std::filesystem::path(resourcePath).append("Keyboard.png")).first });
	}
	namespace stb {
		namespace memory {
			image_data write(vec2& size, int comp, const void* data, int strideBytes, uint32_t delay) {
				int imageSize;
				stbi_uc* image{ stbi_write_png_to_mem((const unsigned char*)data, strideBytes, size.x.i32, size.y.i32, comp, &imageSize) };
				if (!image) {
					// equivalent to RETURN_DEFAULT
					return {};
				}
				else {
					// equivalent to RETURN(...)
					return { image, imageSize, delay };
				}
			}
			image read(image_data data) {
				vec2 size{};
				stbi_uc* image{ stbi_load_from_memory(data.bytes, data.size, &size.x.i32, &size.y.i32, NULL, 0) };
				if (!image) {
					// equivalent to RETURN_DEFAULT
					return {};
				}
				else {
					// equivalent to RETURN(...)
					return { image, size };
				}
			}
			gif read_gif(image_data data) {
				vec2 size{};
				int* delays{};
				int frameCount{}, comp{};
				stbi_uc* image{ stbi_load_gif_from_memory(data.bytes, data.size, &delays, &size.x.i32, &size.y.i32, &frameCount, &comp, NULL) };
				if (!image) {
					// equivalent to RETURN_DEFAULT
					return {};
				}
				else {
					// equivalent to RETURN(...)
					return { image, data, delays,frameCount,size,comp };
				}
			}
		}

		namespace file {
			image read_image(std::filesystem::path path) {
				vec2 size{};
				stbi_uc* image{ stbi_load(path.string().c_str(), &size.x.i32, &size.y.i32, NULL, 4) };
				if (!image) {
					// equivalent to RETURN_DEFAULT
					return {};
				}
				else {
					// equivalent to RETURN(...)
					return { image, size };
				}
			}

			gif read_gif(std::filesystem::path path) {
				FILE* f{ stbi__fopen(path.string().c_str(), "rb") };
				if (!f) {
					// equivalent to RETURN_DEFAULT
					return {};
				}

				image_data data{};
				fseek(f, 0L, SEEK_END);
				data.size = ftell(f);
				fseek(f, 0L, SEEK_SET);
				data.bytes = decltype(data.bytes)(malloc(data.size));
				fread(data.bytes, data.size, 1, f);
				fclose(f);

				gif gif_result = memory::read_gif(data);
				free(data.bytes);

				if (!gif_result.bytes) {
					return {};
				}
				else {
					return gif_result;
				}

			}
		}

		std::vector<image_data> split_gif(std::filesystem::path path) {
			gif gif{ file::read_gif(path) };
			std::vector<image_data> frames{};
			size_t bytes{ size_t(gif.size.x.i32 * gif.comp) };
			for (size_t i{}; i != gif.frame_count; ++i)
				frames.push_back(memory::write(gif.size, gif.comp, gif.bytes + bytes * gif.size.y.i32 * i, 0, gif.delays[i]));
			stbi_image_free(gif.bytes);
			return frames;
		}
	}

	std::pair<shader_data, vec2> renderer::create_texture(std::filesystem::path path) {
		image image{ stb::file::read_image(path) };
		LOG(INFO) << "Loading " << path.string().c_str() << " ...";
		return std::make_pair(renderer::create_shader_data(image), image.size);
	}

	std::vector<frame_data> renderer::create_gif_texture(std::filesystem::path path) {
		std::vector<frame_data> frames{};
		std::vector<image_data> gif{ stb::split_gif(path) };
		for (auto& fr : gif) {
			image image{ stb::memory::read(fr) };
			frames.push_back({ fr.delay, renderer::create_shader_data(image) });
		}
		return frames;
	}

	shader_data renderer::create_shader_data(image image) {
		if (!image.data) {
			// equivalent to RETURN_DEFAULT
			return {};
		}

		ID3D11ShaderResourceView* resourceView{};
		ID3D11Texture2D* texture{};
		D3D11_TEXTURE2D_DESC desc{ image.size.x.u32, image.size.y.u32, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1 }, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0 };
		D3D11_SUBRESOURCE_DATA subResource{ image.data, desc.Width * 4, 0 };
		m_d3d_device->CreateTexture2D(&desc, &subResource, &texture);

		if (!texture) {
			// equivalent to RETURN_DEFAULT
			return {};
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_SRV_DIMENSION_TEXTURE2D, { 0, desc.MipLevels } };
		m_d3d_device->CreateShaderResourceView(texture, &srvDesc, std::add_pointer_t<decltype(resourceView)>(&resourceView));

		// equivalent to RETURN_IF_VALID(texture, resourceView)
		if (!texture || !resourceView) {
			return {};
		}
		else {
			return { texture, resourceView };
		}
	}
	image_data writePngToMem(int x, int y, int comp, const void* data, int strideBytes, uint32_t delay) {
		int len;
		unsigned char* png = stbi_write_png_to_mem((const unsigned char*)data, strideBytes, x, y, comp, &len);
		if (png == NULL)
			return image_data();
		return image_data({ png, len, delay });
	}
	std::map<int, image_data> loadGif(std::filesystem::path path) {
		std::map<int, image_data> frames{};
		FILE* file{ fopen(path.string().c_str(), ("rb")) };
		if (!file)
			return frames;
		fseek(file, 0L, SEEK_END);
		long size{ ftell(file) };
		fseek(file, 0L, SEEK_SET);
		void* buffer{ malloc(size) };
		fread(buffer, size, 1, file);
		fclose(file);
		int* delays{};
		int width{}, height{}, frameCount{}, comp{};
		auto data = stbi_load_gif_from_memory((stbi_uc*)buffer, size, &delays, &width, &height, &frameCount, &comp, NULL);
		free(buffer);
		size_t bytes = width * comp;
		for (size_t i{}; i != frameCount; ++i)
			frames.insert(std::make_pair(i, writePngToMem(width, height, comp, data + bytes * height * i, 0, delays[i])));
		stbi_image_free(data);
		return frames;
	}
	image loadImgFromMem(image_data data) {
		vec2 vec{};
		auto image1 = stbi_load_from_memory(data.bytes, data.size, (int*)&vec.x, (int*)&vec.y, NULL, 0);
		if (image1 == nullptr)
			return image({});
		return image({ image1, vec });
	}
	std::map<int, frame_data> renderer::createGifTexture(std::filesystem::path path) {
		auto gifData = loadGif(path);
		std::map<int, frame_data> tmpArr{};
		for (auto& fr : gifData) {
			auto img = loadImgFromMem(fr.second);
			tmpArr.insert(std::make_pair(fr.first, frame_data({ fr.second.delay, g_renderer->create_shader_data(img), img.size })));
		}
		return tmpArr;
	}
	interfaceSprite loadTexture(std::filesystem::path path) {
		if (path.extension().string() == ".gif") {
			LOG(INFO) << "Loading Textures " << path.filename().stem().string() << path.extension().string();
			auto texture = g_renderer->createGifTexture(path);
			return { 0, texture, true, nullptr, 0, nullptr };
		}
		else {
			LOG(INFO) << "Loading Static Textures ";
		}
		return { 0, {}, false, nullptr, 0, nullptr };
	}
	void loadTextures() {
		auto path = std::filesystem::path(std::getenv("appdata")).append("BigBaseV2").append("Ressources");
		if (std::filesystem::exists(path)) {
			std::filesystem::directory_iterator it{ path };
			for (auto&& entry : it) {
				if (entry.is_regular_file()) {
					auto path{ entry.path() };
					if (path.stem().string() == "header") {
						//ui::g_header.m_texture = loadTexture(path);
						//ui::g_header.m_textureEnabled = { true };
					}
					if (path.stem().string() == "options") {
						//ui::g_options.m_texture = loadTexture(path);
						//ui::g_options.m_textureEnabled = { true };
					}
					if (path.stem().string() == "footer") {
						//ui::g_footer.m_texture = loadTexture(path);
						//ui::g_footer.m_textureEnabled = { true };
					}
					if (path.stem().string() == "description") {
						//ui::g_description.m_texture = loadTexture(path);
						//ui::g_description.m_textureEnabled = { true };
					}
					if (path.stem().string() == "subtitle") {
						//ui::g_subtitle.m_texture = loadTexture(path);
						//ui::g_subtitle.m_textureEnabled = { true };
					}
				}
			}
		}
	}
}
