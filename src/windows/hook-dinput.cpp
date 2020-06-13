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

#include "device-dinput.hpp"
#include <gamepad/binding-dinput.hpp>
#include <gamepad/hook-dinput.hpp>
#include <gamepad/log.hpp>

using namespace std;
using namespace json11;

namespace gamepad {

BOOL CALLBACK enum_callback(LPCDIDEVICEINSTANCE dev, LPVOID data)
{
    auto h = static_cast<hook_dinput*>(data);

    auto new_device = make_shared<device_dinput>(dev, h->m_dinput, h->m_hook_window);

    if (new_device->is_valid()) {
        h->m_devices.emplace_back(dynamic_pointer_cast<device>(new_device));
        auto b = h->get_binding_for_device(new_device->get_id());

        if (b) {
            new_device->set_binding(move(b));
        } else {
            auto b = make_shared<cfg::binding_dinput>(cfg::dinput_default_binding);
            new_device->set_binding(dynamic_pointer_cast<cfg::binding>(b));
        }
    }

    return DIENUM_CONTINUE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void hook_dinput::query_devices()
{
    m_mutex.lock();
    m_devices.clear();
    auto result = m_dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, enum_callback, this, DIEDFL_ATTACHEDONLY);

    if (FAILED(result)) {
        gerr("Enumeration of Direct Input devices failed");
    }

    m_mutex.unlock();
}

bool hook_dinput::start()
{
    if (m_running)
        return true;

    auto result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8W,
        reinterpret_cast<void**>(&m_dinput), nullptr);
    if (FAILED(result)) {
        gerr("Couldn't create DirectInput interface");
        return false;
    }

    /* Create window */
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"libgamepad_window_class";

    if (RegisterClass(&wc)) {
        m_hook_window = CreateWindowEx(WS_EX_CLIENTEDGE, L"libgamepad_window_class", L"libgampead_window",
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 240, 120, nullptr, nullptr,
            wc.hInstance, nullptr);
    }

    if (m_hook_window == nullptr) {
        gerr("Couldn't create hook window");
        return false;
    }

    return hook::start();
}

shared_ptr<cfg::binding> hook_dinput::make_native_binding(const Json& j)
{
    return make_shared<cfg::binding_dinput>(j);
}

void hook_dinput::on_bind(Json::object& j, uint16_t native_code, uint16_t vc, int16_t val, bool is_axis)
{
    if (is_axis && vc == axis::LEFT_TRIGGER || vc == axis::RIGHT_TRIGGER)
        j["trigger_polarity"] = val > 0 ? 1 : -1;
}
}
