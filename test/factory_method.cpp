#include <memory>
#include <functional>
#include <iostream>

#include "design_patterns/creational/factory_method.hpp"

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
		struct Interface
		{
			virtual ~Interface() = default;
		};

		struct Derived1 : public Interface {};
		struct Derived2 : public Interface {};
		struct Derived3 : public Interface {};

		void proof_of_concept()
		{
			const auto factoryMethod = design_patterns::creational::factory_method::factory_method<Interface, std::size_t>
			(
				design_patterns::creational::factory_method::if_create{
					[](const auto& key)
					{
						return key == 1;
					},
					[]() -> Interface*
					{
						return new Derived1{};
					}
				},
				design_patterns::creational::factory_method::if_create{
					[](const auto& key)
					{
						return key == 2;
					},
					[]() -> Interface*
					{
						return new Derived2{};
					}
				},
				design_patterns::creational::factory_method::if_create{
					[](const auto& key)
					{
						return key == 3;
					},
					[]() -> Interface*
					{
						return new Derived3{};
					}
				}
			);
			assert_true(dynamic_cast<Derived1*>(std::unique_ptr<Interface>{ std::invoke(factoryMethod, 1) }.get()));
			assert_true(dynamic_cast<Derived2*>(std::unique_ptr<Interface>{ std::invoke(factoryMethod, 2) }.get()));
			assert_true(dynamic_cast<Derived3*>(std::unique_ptr<Interface>{ std::invoke(factoryMethod, 3) }.get()));

			assert_exception_raised<design_patterns::creational::factory_method::condition_not_satisfied>([&factoryMethod] {std::invoke(factoryMethod, 0); });
		}
	}

}

int main()
{
	try
	{
		Dummy::proof_of_concept();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Failed on \"proof of concept\" due to: " << e.what() << '\n';
		return 1;
	}
	std::cout << "Tests ran successfully" << std::endl;
}
