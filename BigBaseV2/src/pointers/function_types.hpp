#pragma once
#include "common.hpp"
#include "gta/fwddec.hpp"
#include "gta/natives.hpp"
#include "hooking/hooking.hpp"
#include "gta/pgDictionary.hpp"
#include "gta/pgDictionaryPool.hpp"
#include "gta/grcTextureStore.hpp"
#include "gta/grcTexture.hpp"

namespace big::functions
{
	using run_script_threads_t = bool(*)(std::uint32_t ops_to_execute);
	using get_native_handler_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, rage::scrNativeHash);
	using fix_vectors_t = void(*)(rage::scrNativeCallContext*);

	//Texture
	using add_texture_to_dictionary_t = void (*)(rage::pgDictionary* dict, uint32_t hash, rage::grcTexture* texture);
	using setup_store_item_t = uint32_t * (*)(rage::grcTextureStore* store, uint32_t* out, uint32_t* dictHash);
	using add_dictionary_to_pool_t = void (*)(rage::grcTextureStore* store, uint32_t slot, rage::pgDictionary* dict);
	using setup_dictionary_t = rage::pgDictionary* (*)(rage::pgDictionary* dict, int size);
}
