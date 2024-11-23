#pragma once
#include "common.hpp"
#include "gta/grcTexture.hpp"
#include "gta/grcTextureFactory.hpp"
#include "gta/grcTextureStore.hpp"
#include "gta/pgDictionary.hpp"

#include "gta/tls_context.hpp"
#include <ShlObj_core.h>

namespace big {
	struct texture_context {
		std::string m_name;
		rage::grcTexture* m_texture;
	};

	class textures {
	public:
		textures();
		static void tick();
		void load_image(std::filesystem::path path);
		texture_context get(std::string name);
		bool exists(std::string name) {
			texture_context texture{get(name)};
			if (!texture.m_name.empty()) {
				return true;
			}
			return false;
		}

	private:
		static textures* get() {
			static textures i{};

			return &i;
		}

	public:
		static void initialize() {
			get();
		}
		std::string GetDocumentsPath() {
			wchar_t folder[1024];
			HRESULT result = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, folder);
			if (SUCCEEDED(result)) {
				char string[1024];
				wcstombs(string, folder, 1023);
				return string;
			}
			else return "";
		}
		std::string GetAppdataPath()
		{
			std::string appdata = std::getenv("appdata");
			std::string path = appdata + "\\" + "BigBaseV2";
			return path;
		}

	private:
		std::filesystem::path m_path{ GetAppdataPath() };
		rage::pgDictionary* m_dictionary{};
		std::vector<std::string> m_files{};
		std::vector<texture_context> m_textures{};
	};
}