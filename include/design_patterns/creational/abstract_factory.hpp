#pragma once

#include <functional>

#include "design_patterns/creational/factory_method.hpp"

namespace design_patterns::creational 
{
	namespace abstract_factory
	{
		template <typename Code, typename Interface>
		struct creator;

		template <typename Condition, typename Creator>
		struct if_factory;
		
		template <typename ...Creators>
		class factory;
	}

	namespace abstract_factory_util
	{
		template <typename ...Types>
		class i_type_adapter;

		template <typename Type>
		class i_type_adapter<Type>
		{
		public:
			virtual operator Type*() const = 0; 
		
		protected:
			i_type_adapter() = default;
			i_type_adapter(const i_type_adapter&) = delete;
			~i_type_adapter() = default;
		};

		template <typename T1, typename T2, typename ...Types>
		class i_type_adapter<T1, T2, Types...>
			: public i_type_adapter<T1>
			, public i_type_adapter<T2>
			, public i_type_adapter<Types>...
		{
		public:
			~i_type_adapter() = default;
		protected:
			i_type_adapter() = default;
			i_type_adapter(const i_type_adapter&) = delete;
		};

		template <typename T>
		struct type_holder {};

		template <typename T>
		constexpr auto type_holder_v = type_holder<T>{};

		template <typename...>
		class type_adapter_overrider_t;

		template <typename TypeAdapter>
		class type_adapter_overrider_t<type_holder<TypeAdapter>> : public TypeAdapter {};

		template <typename TypeProducer, typename Interface, typename ...Rest>
		class type_adapter_overrider_t<abstract_factory::creator<TypeProducer, Interface>, Rest...>
			: public type_adapter_overrider_t<Rest...>
		{
		protected:
			~type_adapter_overrider_t() = default;

			template <typename ...Args>
			type_adapter_overrider_t(const abstract_factory::creator<TypeProducer, Interface>& producer, const Args& ...rest)
				: type_adapter_overrider_t<Rest...>{rest...}
				, producer{producer.code}
			{}

		public:
			operator Interface*() const final
			{
				return std::invoke(producer);
			}

		private:
			Interface* (*producer)();
		};

		template <typename... Types>
		class final_type_adapter_overrider_t final
			: public type_adapter_overrider_t<Types...>
		{
		public:
			template <typename... Args>
			final_type_adapter_overrider_t(const Args& ...args)
				: type_adapter_overrider_t<Types...>(args...)
			{}
		};

		template <typename ...Interfaces>
		struct interfaces_holder {};
		template <typename ...Interfaces>
		constexpr auto interfaces_holder_v = interfaces_holder<Interfaces...>{};

		template <typename AbstractFactoryAllocator, typename ...InterfaceOrder, typename ...Args>
		decltype(auto) type_adapter_overrider(
			AbstractFactoryAllocator&& allocator,
			interfaces_holder<InterfaceOrder...>,
			abstract_factory::factory<Args...> adapters
		)
		{
			return [allocator = std::move(allocator), adapters = std::move(adapters)]() -> i_type_adapter<InterfaceOrder...>* 
			{
				using type_adapter = final_type_adapter_overrider_t<Args..., type_holder<i_type_adapter<InterfaceOrder...>>>;
				return allocator.template operator()<type_adapter>(adapters.template get<InterfaceOrder>()...);
			};
		}

		template <typename ConditionData, typename AbstractFactoryAllocator, typename ...TypeProducers, typename ...InterfaceOrder, typename ...Conditions, typename ...Creators>
		decltype(auto) abstract_factory(
			AbstractFactoryAllocator&& allocator,
			type_holder<abstract_factory::factory<abstract_factory::creator<TypeProducers, InterfaceOrder>...>>, 
			abstract_factory::if_factory<Conditions, Creators> ...factories)
		{
			return factory_method::factory_method<i_type_adapter<InterfaceOrder...>, ConditionData>
			(
				factory_method::if_create
				{
					std::move(factories.condition),
					type_adapter_overrider(std::forward<AbstractFactoryAllocator>(allocator), interfaces_holder_v<InterfaceOrder...>, factories.factory())
				}...
			);
		}
	}

	namespace abstract_factory
	{
		template <typename Code, typename Interface = std::remove_pointer_t<std::invoke_result_t<Code>>>
		struct creator
		{
			Code code;
		};

		template <typename Code>
		creator(Code&&) -> creator<Code>;

		template <typename Condition, typename Factory>
		struct if_factory
		{
			Condition condition;
			Factory factory;
		};

		template <typename Condition, typename Factory>
		if_factory(Condition&&, Factory&&) -> if_factory<Condition, Factory>; 

		template <typename ...Creators>
		class factory
			: public Creators...
		{
		public:
			template <typename ...Args>
			factory(Args&& ...ta)
				: Creators{creator{std::forward<Args>(ta)}}...
			{}

			template <typename CreatorReturnType>
			decltype(auto) get() const
			{
				return std::move(resolve<CreatorReturnType>(*this));
			}
		private:
			template<typename Interface, typename TypeProducer>
			decltype(auto) resolve(const creator<TypeProducer, Interface>& dispatched) const
			{
				return (dispatched);
			}
		};

		template <typename ...TypeProducers>
		factory(TypeProducers&& ...) -> factory<creator<TypeProducers>...>;

		using condition_not_satisfied = typename factory_method::condition_not_satisfied;

		template <typename ConditionData, typename AbstractFactoryAllocator, typename Condition, typename Factory, typename ...InterfaceProducers, typename ...Conditions, typename ...Factories>
		decltype(auto) abstract_factory(
			AbstractFactoryAllocator&& allocator,
			if_factory<Condition, Factory> factory,
			if_factory<Conditions, Factories> ...factories)
		{
			return abstract_factory_util::abstract_factory<ConditionData>
			(
				std::forward<AbstractFactoryAllocator>(allocator),
				abstract_factory_util::type_holder_v<std::invoke_result_t<Factory>>,
				std::move(factory), std::move(factories)...
			);
		}
	}
}
