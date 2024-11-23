#pragma once
#include <d3d11.h>
#include "common.hpp"

namespace big {

	class renderer_queue {
	public:
		void tick() {
			for (auto& fn : m_funcs)
				fn.second();
		}
		void add(std::string id, std::function<void()> func) {
			if (m_funcs.size() == NULL)
				m_funcs.insert({ id, std::move(func) });
		}
		void remove(std::string id) {
			m_funcs.erase(id);
		}
	public:
		std::map<std::string, std::function<void()>> m_funcs;
	};
	inline renderer_queue g_renderer_queue;

	struct resource {
		std::string m_name{};
		shader_data m_data{};
		decltype(m_data.resource_view) get_shader_resource() {
			return m_data.resource_view;
		}
	};
	struct resources {
		resource get(std::string key) {
			for (auto& r : m_table) {
				if (!r.m_name.compare(key.c_str())) {
					return r;
				}
			}
			return {};
		}
		void add(resource resource) {
			m_table.push_back(resource);
		}
		ID3D11ShaderResourceView* operator[](std::string key) {
			return get(key).get_shader_resource();
		}
		std::vector<resource> m_table{};
	};
}