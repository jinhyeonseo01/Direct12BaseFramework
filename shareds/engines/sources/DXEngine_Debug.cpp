
#include "stdafx.h"
#include "DXEngine.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"

void Engine::DebugInit()
{
	DebugCommand command;

	command.command = "help";
	command.detail = L"명령어 리스트를 보여줌.";
	command.func = std::function([&](std::vector<std::wstring>& args){
		auto engine = GetEngine(_mainEngineIndex);
		auto scene = SceneManager::_currentScene;
		for (int i = 0; i < _debugCommandList.size(); i++)
		{
			Debug::log << _debugCommandList[i].command << ":";
			for (int j = _debugCommandList[i].command.size(); j < 20; j++)
				Debug::log << " ";
			Debug::log << "\t"<< _debugCommandList[i].detail << "\n";
		}
	});
	_debugCommandList.push_back(command);
	command.command = "Help";
	_debugCommandList.push_back(command);
	command.command = "h";
	_debugCommandList.push_back(command);
	command.command = "?";
	_debugCommandList.push_back(command);


	command.command = "framelock";
	command.detail = L"프레임 제한 온/오프. arg[1]:on/off";
	command.func = std::function([&](std::vector<std::wstring>& args) {
		auto engine = GetEngine(_mainEngineIndex);
		auto scene = SceneManager::_currentScene;
		if (args.size() < 2)
		{
			Debug::log << "매개변수가 부족합니다.\n";
			return;
		}
		engine->isFrameLock = args[1] == L"on";
	});
	_debugCommandList.push_back(command);


	command.command = "targetframe";
	command.detail = L"타겟 프레임 설정. arg[1]:value(int)";
	command.func = std::function([&](std::vector<std::wstring>& args) {
		auto engine = GetEngine(_mainEngineIndex);
		auto scene = SceneManager::_currentScene;
		if (args.size() < 2)
		{
			Debug::log << "매개변수가 부족합니다.\n";
			return;
		}
		engine->targetFrame = std::stoi(std::to_string(args[1]));
		});
	_debugCommandList.push_back(command);


	command.command = "quit";
	command.detail = L"디버그 모드 종료.";
	command.func = std::function([&](std::vector<std::wstring>& args) {
		auto engine = GetEngine(_mainEngineIndex);
		auto scene = SceneManager::_currentScene;
		engine->_debugCommandMode = false;
		});
	_debugCommandList.push_back(command);
	command.command = "q";
	_debugCommandList.push_back(command);
	command.command = "exit";
	_debugCommandList.push_back(command);
	command.command = "end";
	_debugCommandList.push_back(command);

	command.command = "scenedebug";
	command.detail = L"씬 오브젝트 표시";
	command.func = std::function([&](std::vector<std::wstring>& args) {
		auto engine = GetEngine(_mainEngineIndex);
		auto scene = SceneManager::_currentScene;
		scene->Debug();
		});
	_debugCommandList.push_back(command);
}

void Engine::DebugPipeline()
{
	auto engine = GetEngine(_mainEngineIndex);
	auto scene = SceneManager::_currentScene;
	if (engine->input->GetKeyUp(KeyCode::BackQoute))
	{
		_debugCommandMode = true;
		UpdateTitleName(std::to_wstring(std::format("{} ({})", std::to_string(_titleName), "Console Read Waiting...")));
		Debug::log << std::format("{} : 커맨드 모드 활성화.", std::to_string(_titleName)) << "\n";

		while (_debugCommandMode)
		{
			Debug::log << "입력 : ";
			std::this_thread::sleep_for(std::chrono::milliseconds(Debug::Console::debugDeltaTime * 2));
			std::string command;
			std::vector<std::wstring> args;
			Debug::log >> command;
			std::split(std::to_wstring(command), std::wstring(L" "), args);
			if (args.size() > 0)
			{
				auto iter = std::find(_debugCommandList.begin(), _debugCommandList.end(), args[0]);
				if (iter != _debugCommandList.end())
					(*iter).func(args);
				else
					Debug::log << "존재하지 않는 명령어" << "\n";
			}
		}
	}
}

bool DebugCommand::operator==(const std::string& command) const
{
	return this->command == command;
}

bool DebugCommand::operator==(const std::wstring& command) const
{
	return this->operator==(std::to_string(command));
}
