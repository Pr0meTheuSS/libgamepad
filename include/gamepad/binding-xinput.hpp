/**
 ** This file is part of the libgamepad project.
 ** Copyright 2020 univrsal <universailp@web.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as
 ** published by the Free Software Foundation, either version 3 of the
 ** License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#pragma once
#include <gamepad/binding.hpp>

namespace gamepad {
class device_xinput;
namespace cfg {
#ifdef LGP_ENABLE_JSON
    extern json11::Json dinput_default_binding;
#endif
    class binding_xinput : public binding {

        friend class gamepad::device_xinput;

    public:
        binding_xinput() = default;
#ifdef LGP_ENABLE_JSON
        binding_xinput(const json11::Json& j);
#endif
    };
}
}