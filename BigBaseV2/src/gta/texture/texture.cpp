#include "texture.hpp"

#include "common.hpp"
#include "util/logger.hpp"
#include "invoker/natives.hpp"
#include "pointers/pointers.hpp"
#include "renderer/renderer.hpp"
#define VALID_TEXTURE_EXT(ext) \
	ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif"
namespace big {

	void add_dictionary_to_pool(rage::pgDictionary* dictionary, std::string name) {
		if (auto grcTextureStore = g_pointers->m_texture_store; grcTextureStore) {
			uint32_t hash{ rage::joaat(name) };
			uint32_t output{};
			uint32_t index{ *g_pointers->m_setup_store_item(grcTextureStore, &output, &hash) };
			g_pointers->m_add_dictionary_to_pool(grcTextureStore, index, dictionary);
		}
	}

	rage::pgDictionary* create_dictionary(size_t size) {
		rage::pgDictionary* customDictionary{ (rage::pgDictionary*)rage::sysMemAllocator::get()->Allocate(64, 8, NULL) };
		if (customDictionary) {
			customDictionary = g_pointers->m_setup_dictionary(customDictionary, size);
			return customDictionary;
		}
		return nullptr;
	}

	rage::grcTexture* create_texture(std::string filepath) {
		rage::grcTexture* texture{};
		if (auto grcTextureFactory = *g_pointers->m_texture_factory; grcTextureFactory) {
			texture = grcTextureFactory->CreateTexture(filepath.c_str(), NULL);
		}
		texture->Set(filepath, g_renderer->create_texture(filepath));
		return texture;
	}
	textures::textures() {
		m_path.append("textures");
		if (std::filesystem::exists(m_path)) {
			std::filesystem::directory_iterator it{ m_path };
			for (auto&& entry : it) {
				if (entry.is_regular_file()) {
					auto path{ entry.path() };
					if (path.has_filename() && (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg" || path.extension() == ".bmp" || path.extension() == ".gif")) {
						m_files.push_back({ path.filename().string() });
					}
				}
			}

			m_dictionary = create_dictionary(m_files.size() * 2);

			if (m_dictionary) {
				add_dictionary_to_pool(m_dictionary, "textures");
				if (!m_files.empty()) {
					for (auto& file : m_files) {
						load_image(std::filesystem::path(m_path).append(file));
						LOG(INFO) << "Loading " << file.c_str() << " ...";
					}

				}
			}
		}
	}

	void textures::tick() {
		GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("textures", FALSE);
		if (std::filesystem::exists(textures::get()->m_path)) {
			std::filesystem::directory_iterator it{ textures::get()->m_path };
			for (auto&& entry : it) {
				if (entry.is_regular_file()) {
					auto path{ entry.path() };
					if (path.has_filename() && path.has_extension()) {
						std::string name{ path.stem().string() };
						if (VALID_TEXTURE_EXT(path.extension())) {
							if (!textures::get()->exists(name)) {
								//loadImage(fs::path(m_path).append(name));
							}
						}
					}
				}
			}
		}
	}

	void textures::load_image(std::filesystem::path path) {
		if (rage::grcTextureFactory* factory{ *g_pointers->m_texture_factory }; factory) {
			std::string name{ path.stem().string() };
			rage::grcTexture* texture{ factory->CreateTexture(name.c_str(), 0) };
			texture->Set(name, g_renderer->create_texture(path));
			g_pointers->m_add_texture_to_dictionary(m_dictionary, rage::joaat(name), texture);

		}
	}

	texture_context textures::get(std::string name) {
		for (auto& texture : m_textures) {
			if (texture.m_name == name)
				return texture;
		}
		return {};
	}
}