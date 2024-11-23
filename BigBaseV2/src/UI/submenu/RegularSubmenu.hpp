#pragma once
#include "BaseSubmenu.hpp"

namespace big::UserInterface {
	class regular_submenu : public BaseSubmenu<regular_submenu> {
	public:
		regular_submenu() = default;
		regular_submenu(std::string name, std::uint32_t id, std::function<void(regular_submenu*)> action, bool path = false, const char* path_name = "") {
			if (path)
				set_name(path_name);
			else
				set_name(name);
			set_id(id);
			set_action(std::move(action));
		}
		regular_submenu(std::string name, std::uint32_t id) {
			set_name(name);
			set_id(id);
		}
	};
}
