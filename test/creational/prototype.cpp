#include <functional>
#include <iostream>

#include "design_patterns/creational/prototype.hpp"

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

	class Shape
	{
		int color;
	protected:
		Shape(int color)
			: color{color}
		{}
	public:
		Shape(const Shape&) = default;
		virtual ~Shape() = default;
		bool operator==(const Shape& other) const
		{
			return this->color == other.color;
		}
	};

	class Rectangle
		: public Shape
		, public design_patterns::creational::prototype::with_prototype<Shape, Rectangle>
	{
	private:
		int width, height;
	public:
		Rectangle(const Rectangle&) = default;
		Rectangle(int color, int width, int height)
			: Shape{color}
			, width{width}
			, height{height}
		{}
		bool operator==(const Rectangle& other) const
		{
			return Shape::operator==(other)
				&& this->width == other.width
				&& this->height == other.height;
		}
	};

	void proof_of_concept()
	{
		Rectangle original{1,2,3};

		auto* shape = original(design_patterns::creational::prototype::clone{});
		assert_false(shape == nullptr);
		assert_false(shape == &original);
		
		auto prototype = dynamic_cast<Rectangle*>(shape);
		assert_false(prototype == nullptr);

		assert_true(*prototype == original);
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
