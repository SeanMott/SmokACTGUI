#pragma once

//defines all the functions for generating random numbers and dice rolls

#include <BTDSTD/CompilerCommands/FunctionModifiers.hpp>
#include <BTDSTD/IO/Chrono.hpp>

#include <random>

namespace TableToolkit::Util::Roll
{
	//defines a function for generating a random number inside a range (min inclusive) (max inclusive)
	BTD_Compiler_ForceInline uint8 RangeNumberGenerate_MinMax_Inclusive(const uint8& min, const uint8& max)
	{
		//seed the generator with the current time of your local machine
		BTD::Chrono::SysDateData sysDateData; BTD::Chrono::GetLocalTime(&sysDateData);
		std::minstd_rand g(sysDateData.milliseconds);

		//set the range
		std::uniform_int_distribution<> d(min, max);
		
		//random generate
		return d(g);
	}

	//defines a function for generating a random number inside a range (min exclusive) (max exclusive)

	//defines a function for generating a random number inside a range (min inclusive) (max exclusive)
	//defines a function for generating a random number inside a range (min exclusive) (max inclusive)

	//defines a function for getting the result of a D6
	BTD_Compiler_ForceInline uint8 GenerateD6Result()
	{
		return RangeNumberGenerate_MinMax_Inclusive(0, 6);
	}

	//defines a function for getting the result of a D7
	//defines a function for getting the result of a D8
	//defines a function for getting the result of a D9
	
	//defines a function for getting the result of a D10
	BTD_Compiler_ForceInline uint8 GenerateD10Result()
	{
		return RangeNumberGenerate_MinMax_Inclusive(0, 10);
	}

	//defines a function for getting the result of a D20
}