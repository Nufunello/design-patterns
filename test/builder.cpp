#include <iostream>

#include "design_patterns/creational/builder.hpp"

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

	template <typename Exception, typename Code>
	void assert_exception_raised(const Code& code)
	{
		try
		{
			code();
		}
		catch(const Exception&)
		{
			return;
		}
		catch(const std::exception& ex)
		{
			throw std::runtime_error(std::string{"Unexpected exception raised: "} + ex.what());
		}
		throw std::runtime_error("No exception raised");
	}

	void proof_of_concept()
	{
		auto d = design_patterns::creational::builder::builder(int{0}, [](int a){return a;})
				.withSetter<"add1">([](int& a) { a += 1; })
				.withSetter<"add2">([](int& a) { a += 2; })
				.withSetter<"add3">([](int& a) { a += 3; })
				.withSetter<"add4">([](int& a) { a += 4; });

		d.template operator()<"add1">();
		assert_true(d.perform_build() == 1);
		d.template operator()<"add1">();
		assert_true(d.perform_build() == 2);
		d.template operator()<"add2">();
		assert_true(d.perform_build() == 4);
		d.template operator()<"add3">();
		assert_true(d.perform_build() == 7);
		d.template operator()<"add4">();
		assert_true(d.perform_build() == 11);
		d.template operator()<"add4">();
		assert_true(d.perform_build() == 15);
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
