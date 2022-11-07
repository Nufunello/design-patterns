#include <memory>

namespace design_patterns::creational 
{
	namespace prototype
	{
		struct clone {};

		template <typename PrototypeOf>
		class iprototype
		{
		protected:
			iprototype() = default;
		public:
			virtual ~iprototype() = default;
			virtual PrototypeOf* operator()(clone) const = 0;
		};

		template <typename Base, typename Derived, template <typename> typename Allocator = std::allocator>
		class with_prototype
			: public iprototype<Base>
		{
		private:
			mutable Allocator<Derived> allocator;
		protected:
			with_prototype(Allocator<Derived> allocator = {}) : allocator{std::move(allocator)} {}
			~with_prototype() override = default;
		public:
			std::invoke_result_t<iprototype<Base>, clone> operator()(clone) const override final
			{
				auto* clone = allocator.allocate(1);
				std::allocator_traits<Allocator<Derived>>::construct(allocator, clone, *static_cast<const Derived*>(this));
				return clone;
			}
		};
	}
}
