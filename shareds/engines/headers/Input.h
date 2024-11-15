#pragma once

#include <stdafx.h>
#include <input_module.h>

namespace dxe
{
    class InputKeyData
    {
    public:
        bool down;
        bool stay;
        bool up;

        std::chrono::time_point<std::chrono::steady_clock> _lastDown_Time;
        std::chrono::time_point<std::chrono::steady_clock> _lastUp_Time;
    };

    class InputMouseData : public InputKeyData
    {
    public:
        Vector2 downPos{0, 0};
        Vector2 upPos{0, 0};
    };

    class Input
    {
    public:
        static Input* main;
        Input();
        virtual ~Input();

    protected:
        static const int _maxInputKeyField = 2048;
        static const int _maxInputMouseField = 1100;
        InputKeyData _inputKeyboardField[Input::_maxInputKeyField];
        InputMouseData _inputMouseField[Input::_maxInputMouseField];

        Vector2 mousePos{0, 0};

    public:
        bool GetKeyDown(int keycode);
        bool GetKey(int keycode);
        bool GetKeyUp(int keycode);


        bool GetMouseDown(int keycode);
        bool GetMouse(int keycode);
        bool GetMouseUp(int keycode);

        bool DataUpdate(InputEvent& event);
        void DataBeginUpdate();


        Vector2 GetMousePosition();
        Vector2 GetMouseDownPosition(int keycode);
        Vector2 GetMouseUpPosition(int keycode);
    };
}
