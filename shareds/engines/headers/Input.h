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
		Vector2 upPos{ 0, 0 };
	};

	class Input
	{
	protected:
		static const int _maxInputKeyField;
		static const int _maxInputMouseField;
		InputKeyData _inputKeyboardField[Input::_maxInputKeyField];
		InputKeyData _inputMouseField[Input::_maxInputMouseField];

		Vector2 mousePos{ 0, 0 };
	public:
		bool GetKeyDown(int keycode);
		bool GetKey(int keycode);
		bool GetKeyUp(int keycode);

		bool GetMouseDown(int keycode);
		bool GetMouse(int keycode);
		bool GetMouseUp(int keycode);

		bool DataUpdate(InputEvent& event);
		bool DataBeginUpdate();


		Vector2 GetMousePosition();
		Vector2 GetMouseDownPosition(int keycode);
		Vector2 GetMouseUpPosition(int keycode);

	};
}

