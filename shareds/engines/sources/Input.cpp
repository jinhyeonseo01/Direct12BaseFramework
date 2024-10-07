#include "Input.h"


const int Input::_maxInputKeyField = 2048;
const int Input::_maxInputMouseField = 256;

bool Input::GetKeyDown(int keycode)
{

}

bool Input::GetKey(int keycode)
{

}

bool Input::GetKeyUp(int keycode)
{

}

bool Input::GetMouseDown(int keycode)
{

}

bool Input::GetMouse(int keycode)
{

}

bool Input::GetMouseUp(int keycode)
{
	_inputKeyboardField[e.keyCode]
}

bool Input::DataUpdate(InputEvent& e)
{
	switch (e.type)
	{
	case InputType::Keyboard:
		_inputKeyboardField[e.keyCode].down = e.keyboard.isDown;
		_inputKeyboardField[e.keyCode].up = e.keyboard.isUp;

		if (_inputKeyboardField[e.keyCode].down)
			_inputKeyboardField[e.keyCode].stay = true;
		break;
	case InputType::Mouse:

		break;
	}
}

bool Input::DataBeginUpdate()
{
	for(int i=0;i<Input::_maxInputKeyField;i++)
	{
		if (_inputKeyboardField[i].down)
			_inputKeyboardField[i].down = false;
		if (_inputKeyboardField[i].up)
		{
			_inputKeyboardField[i].stay = false;
			_inputKeyboardField[i].down = false;
			_inputKeyboardField[i].up = false;

		}
	}
	for (int i = 0; i < Input::_maxInputMouseField; i++)
	{
		if (_inputMouseField[i].down)
			_inputMouseField[i].down = false;
		if (_inputMouseField[i].up)
		{
			_inputMouseField[i].stay = false;
			_inputMouseField[i].down = false;
			_inputMouseField[i].up = false;

		}
	}
}

Vector2 Input::GetMousePosition()
{

}

Vector2 Input::GetMouseDownPosition(int keycode)
{

}

Vector2 Input::GetMouseUpPosition(int keycode)
{

}
