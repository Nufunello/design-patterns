#include <iostream>
#include <memory>

#include "design_patterns/structural/decorator.hpp"

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

	struct IPoint
	{
		enum class Quarter
		{
			First,
			Second,
			Third,
			Fourth
		};
		virtual void operator()(Quarter, std::unordered_map<std::string, bool>& out) const = 0;
	};

	struct DecardPoint
		: public IPoint
	{
		int x, y;
		DecardPoint(int x, int y) : x{x}, y{y} {}
		void operator()(Quarter quarter, std::unordered_map<std::string, bool>& out) const override
		{
			const bool result = [this](Quarter quarter) -> bool {
				switch (quarter)
				{
				case Quarter::First:
					return x >= 0 && y >= 0;
				case Quarter::Second:
					return x >= 0 && y <= 0;
				case Quarter::Third:
					return x <= 0 && y <= 0;
				case Quarter::Fourth:
					return x <= 0 && y >= 0;
				}
			}(quarter);
			out["decard"] = result;
		}
	};

	struct PeriodicPoint
		: public IPoint
	{
		void operator()(Quarter quarter, std::unordered_map<std::string, bool>& out) const override
		{
			static std::unordered_map<decltype(this), bool> vals;
			try
			{
				vals.at(this);
			}
			catch(const std::out_of_range&)
			{
				vals[this] = false;
			}
			out["periodic"] = (vals[this] = !vals[this]);
		}
	};

	struct AlwaysTruePoint
		: public IPoint
	{
		void operator()(Quarter quarter, std::unordered_map<std::string, bool>& out) const override
		{
			out["true"] = true;
		}
	};

	template <typename Interface, typename ...Args>
	decltype(auto) decorator_shared(Args&& ...args)
	{
		using Decorator = decltype(design_patterns::structural::decorator::decorator<Interface>(std::forward<Args>(args)...));
		return std::make_shared<std::decay_t<Decorator>>(std::forward<Args>(args)...);
	}

	void proof_of_concept()
	{
		auto decorator = design_patterns::structural::decorator::decorator<IPoint>(std::make_shared<DecardPoint>(1,2));
		{
			{
				std::unordered_map<std::string, bool> result;
				decorator(IPoint::Quarter::First, result);
				assert_true(result.at("decard"));
			}
			{
				std::unordered_map<std::string, bool> result;
				decorator(IPoint::Quarter::Second, result);
				assert_false(result.at("decard"));
			}
		}
		{
			auto& decorator1 = decorator;
			decorator1.operator()(design_patterns::structural::decorator::set_child, std::make_shared<PeriodicPoint>());

			auto decorator2 = design_patterns::structural::decorator::decorator<IPoint>(std::make_shared<DecardPoint>(1,2), std::make_shared<PeriodicPoint>());
			{
				std::unordered_map<std::string, bool> result1;
				decorator1(IPoint::Quarter::First, result1);
				assert_true(result1.at("decard"));
				assert_true(result1.at("periodic"));

				std::unordered_map<std::string, bool> result2;
				decorator2(IPoint::Quarter::First, result2);
				assert_true(result1 == result2);
			}
			{
				std::unordered_map<std::string, bool> result1;
				decorator1(IPoint::Quarter::First, result1);
				assert_true(result1.at("decard"));
				assert_false(result1.at("periodic"));

				std::unordered_map<std::string, bool> result2;
				decorator2(IPoint::Quarter::First, result2);
				assert_true(result1 == result2);
			}
		}
		{
			auto secondDecorator = decorator_shared<IPoint>(std::make_shared<PeriodicPoint>(), std::make_shared<AlwaysTruePoint>());
			decorator.operator()(design_patterns::structural::decorator::set_child, secondDecorator);
			std::unordered_map<std::string, bool> result;
			decorator(IPoint::Quarter::First, result);
			assert_true(result.at("decard"));
			assert_true(result.contains("periodic"));
			assert_true(result.at("true"));
		}
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
