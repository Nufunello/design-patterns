#pragma once

#include <utility>
#include <exception>
#include <functional>

namespace design_patterns::structural
{
	namespace decorator_util
	{
		struct set_child {};

		template <typename Interface, template <typename> typename Owner, typename Method = decltype(&Interface::operator())>
		class decorator_t;

		template <typename Interface, template <typename> typename Owner, typename Return, typename ...Args>
		class decorator_t<Interface, Owner, Return (Interface::*)(Args...)> final
			: public Interface
		{
			Owner<Interface> current;
			Owner<Interface> child;
		public:
			decorator_t(Owner<Interface> current, Owner<Interface> child = nullptr)
				: current{std::move(current)}, child{std::move(child)}
			{}

			void operator()(set_child, Owner<Interface> child)
			{
				this->child = std::move(child);
			}

			Return operator()(Args ...args) override
			{
				if (child != nullptr)
				{
					std::invoke(*current, args...);
					std::invoke(*child, std::forward<Args>(args)...);
				}
				else
				{
					std::invoke(*current, std::forward<Args>(args)...);
				}
			}
		};

		template <typename Interface, template <typename> typename Owner, typename Return, typename ...Args>
		class decorator_t<Interface, Owner, Return (Interface::*)(Args...) const> final
			: public Interface
		{
			Owner<Interface> current;
			Owner<Interface> child;
		public:
			decorator_t(Owner<Interface> current, Owner<Interface> child = nullptr)
				: current{std::move(current)}, child{std::move(child)}
			{}

			void operator()(set_child, Owner<Interface> child)
			{
				this->child = std::move(child);
			}

			Return operator()(Args ...args) const override
			{
				if (child != nullptr)
				{
					std::invoke(*current, args...);
					std::invoke(*child, std::forward<Args>(args)...);
				}
				else
				{
					std::invoke(*current, std::forward<Args>(args)...);
				}
			}
		};
	}

	namespace decorator
	{
		constexpr auto set_child = decorator_util::set_child{};

		template <typename Interface, template <typename> typename Owner, typename ...Types>
		auto decorator(Owner<Types>... interfaces)
		{
			return decorator_util::decorator_t<Interface, Owner>{std::move(interfaces)...};
		}
	}
} 
