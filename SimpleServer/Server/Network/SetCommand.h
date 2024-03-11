#pragma once
#include <string>

namespace server::network
{
	class SetCommand
	{
	public:

		std::string Key;
		std::string Value;
	};
}
