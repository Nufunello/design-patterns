#include <functional>
#include <iostream>

#include "design_patterns/structural/adapter.hpp"

namespace
{
	void assert_true(bool condition)
	{
		if (!condition)
		{
			throw std::runtime_error("Condition failed");
		}
	}

	void assert_false(bool condition)
	{
		return assert_true(!condition);
	}

	void simpleInterface()
	{
		struct Interface
		{
			virtual int operator()(float) = 0;
		};
		
		auto adapter = design_patterns::structural::adapter::adapter<Interface>([f = 0](float d) mutable -> int { return d * (f++); });
		size_t f = 0;
		for (; f < 5; ++f)
		{
			assert_true(adapter(0) == 0); 
		}
		for (; f < 10; ++f)
		{
			assert_true(adapter(1) == f); 
		}
		for (; f < 15; ++f)
		{
			assert_true(adapter(2) == f * 2); 
		}
	}

	void constInterface()
	{
		struct ConstInterface
		{
			virtual int operator()(float, double) const = 0;
		};
		
		auto adapter = design_patterns::structural::adapter::adapter<ConstInterface>([](float d1, double d2) -> int { return d1 * d2; });
		for (size_t i = 0; i < 10; ++i)
		{
			assert_true(adapter(i, i + 1) == (i * (i + 1)));
		}
	}

	void proof_of_concept()
	{
		simpleInterface();
		constInterface();
	}
}

int main()
{
	try
	{
		proof_of_concept();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Failed on \"proof of concept\" due to: " << e.what() << '\n';
		return 1;
	}
	std::cout << "Tests ran successfully" << std::endl;
}
