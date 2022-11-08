#include <memory>
#include <iostream>

#include "design_patterns/creational/abstract_factory.hpp"

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
	namespace Dummy
	{
		struct Interface1
		{
			virtual ~Interface1() = default;
		};
		struct Interface2
		{
			virtual ~Interface2() = default;
		};
		struct Interface3
		{
			virtual ~Interface3() = default;
		};
	}
	namespace Dummy1
	{
		struct Derived1 : public Dummy::Interface1 {};
		struct Derived2 : public Dummy::Interface2 {};
		struct Derived3 : public Dummy::Interface3 {};
	}
	
	namespace Dummy2
	{
		struct Derived1 : public Dummy::Interface1 {};
		struct Derived2 : public Dummy::Interface2 {};
		struct Derived3 : public Dummy::Interface3 {};
	}

	void proof_of_concept()
	{
		using namespace design_patterns::creational;
		auto abstractFactory = abstract_factory::abstract_factory<std::size_t>
		(
			[]<typename T, typename ...Args>(Args&& ...args) { return new T{std::forward<Args>(args)...}; },
			abstract_factory::if_factory
			{
				[](const std::size_t key) { return key == 1; },
				[]
				{
					return abstract_factory::factory
					{
						[]() -> Dummy::Interface1* { return new Dummy1::Derived1{}; },
						[]() -> Dummy::Interface2* { return new Dummy1::Derived2{}; },
						[]() -> Dummy::Interface3* { return new Dummy1::Derived3{}; }
					};
				},
			},
			abstract_factory::if_factory
			{
				[](const std::size_t key) { return key == 2; },
				[]
				{
					return abstract_factory::factory
					{
						[]() -> Dummy::Interface1* { return new Dummy2::Derived1{}; },
						[]() -> Dummy::Interface2* { return new Dummy2::Derived2{}; },
						[]() -> Dummy::Interface3* { return new Dummy2::Derived3{}; }
					};
				},
			}
		);

		{
			auto factory = std::unique_ptr<std::remove_pointer_t<decltype(abstractFactory.operator()(1))>>(abstractFactory.operator()(1));
			assert_true(dynamic_cast<Dummy1::Derived1*>(std::unique_ptr<Dummy::Interface1>{*factory}.get()));
			assert_true(dynamic_cast<Dummy1::Derived2*>(std::unique_ptr<Dummy::Interface2>{*factory}.get()));
			assert_true(dynamic_cast<Dummy1::Derived3*>(std::unique_ptr<Dummy::Interface3>{*factory}.get()));
		}

		{
			auto factory = std::unique_ptr<std::remove_pointer_t<decltype(abstractFactory.operator()(2))>>(abstractFactory.operator()(2));
			assert_true(dynamic_cast<Dummy2::Derived1*>(std::unique_ptr<Dummy::Interface1>{*factory}.get()));
			assert_true(dynamic_cast<Dummy2::Derived2*>(std::unique_ptr<Dummy::Interface2>{*factory}.get()));
			assert_true(dynamic_cast<Dummy2::Derived3*>(std::unique_ptr<Dummy::Interface3>{*factory}.get()));
		}
		assert_exception_raised<abstract_factory::condition_not_satisfied>
		(
			[&abstractFactory]{ std::unique_ptr<std::remove_pointer_t<decltype(abstractFactory.operator()(3))>>(abstractFactory.operator()(3)); }
		);
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
