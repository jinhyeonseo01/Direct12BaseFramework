#pragma once


namespace Debug
{
	class Console
	{
	protected:
		bool active = false;
	public:
		Console();
		virtual ~Console();

		bool IsActive();
		bool SetActive(bool active);
		bool ClearContext();

		std::string GetContextAll();
	};
}