#pragma once

#include <SDKDDKVer.h>
#include <Windows.h>
#include <D3D11.h>
#include <wrl/client.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "util/logger.hpp"


#include "util/script/script.hpp"

#include "UI/settings.hpp"
#include "UI/Timer.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

using ShaderResource = ID3D11ShaderResourceView*;
union vector_coord {
	int16_t i16;
	uint16_t u16;
	int32_t i32;
	uint32_t u32;
	float f_point;
};
struct vec2 {
	vector_coord x{}, y{};
};
struct image { unsigned char* data{}; vec2 size{}; };
struct image_data { unsigned char* bytes{}; int size{}; uint32_t delay{}; };
struct shader_data { ID3D11Resource* resource{}; ID3D11ShaderResourceView* resource_view{}; };
struct frame_data { uint32_t delay{}; shader_data shader{}; vec2 size{}; };
struct gif { unsigned char* bytes{}; image_data data{}; int* delays{}; int frame_count{}; vec2 size{}; int comp{}; };
struct interfaceSprite { int m_currentFrame; std::map<int, frame_data> m_gif; bool m_animated = false; ShaderResource m_image; int m_lastFrameCount; big::Timer* timer = nullptr; };

namespace big
{
	using namespace std::chrono_literals;

	template <typename T>
	using comptr = Microsoft::WRL::ComPtr<T>;

	inline HMODULE g_hmodule{};
	inline HANDLE g_main_thread{};
	inline DWORD g_main_thread_id{};
	inline HWND m_hWindow;
	inline std::atomic_bool g_running{ true };

	inline std::uint32_t g_SelectedPlayer{};

	inline bool ToggledOn = false;

	inline bool m_switch_ui = false;

	class MemoryStreamBuf : public std::streambuf
	{
	public:
		MemoryStreamBuf(char* array, std::size_t size)
		{
			std::fill_n(array, size, '\0');
			std::streambuf::setp(array, array + size - 1);
		}

		template <std::size_t size>
		MemoryStreamBuf(char(&array)[size]) :
			MemoryStreamBuf(&array[0], size)
		{
		}
	};

	class MemoryStringStream : virtual MemoryStreamBuf, public std::iostream
	{
	public:
		MemoryStringStream(char* array, std::size_t size) :
			MemoryStreamBuf(array, size),
			std::iostream(static_cast<MemoryStreamBuf*>(this))
		{
		}

		template <std::size_t size>
		MemoryStringStream(char(&array)[size]) :
			MemoryStreamBuf(array),
			std::iostream(static_cast<MemoryStreamBuf*>(this))
		{
		}
	};

	template <std::size_t N>
	class MemoryStringStreamWithBuffer : public MemoryStringStream
	{
	public:
		MemoryStringStreamWithBuffer() :
			MemoryStringStream(m_Buffer)
		{}
	private:
		char m_Buffer[N];
	};
}
