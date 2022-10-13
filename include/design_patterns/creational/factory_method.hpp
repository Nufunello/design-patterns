#pragma once

#include <tuple>
#include <utility>
#include <stdexcept>
#include <optional>
#include <functional>

namespace design_patterns::creational 
{
	namespace factory_method
	{
		template <typename Condition, typename Creator>
		struct if_create;

		struct condition_not_satisfied;
	}

	namespace factory_method_utils
	{
		template <typename>
		struct interface_holder{};

		template <typename>
		struct condition_data_holder{};

		template <typename Interface, typename ConditionData, typename ...InterfaceCreators>
		class factory_method_t final
		{
		private:
			template <typename InterfaceCreator>
			decltype(auto) condition(const ConditionData& key) const
			{
				return std::get<InterfaceCreator>(creators).condition(key);
			}
			template <typename InterfaceCreator>
			constexpr static Interface* (*creator)(const factory_method_t& th) = [](const factory_method_t& th){ return std::get<InterfaceCreator>(th.creators).creator(); };
			
			template <typename InterfaceCreator>
			decltype(auto) condition_creator_or_nullptr(const ConditionData& key) const
			{
				return condition<InterfaceCreator>(key) ? creator<InterfaceCreator> : nullptr;
			}
		public:
			factory_method_t(interface_holder<Interface>, condition_data_holder<ConditionData>, InterfaceCreators ...creators)
				: creators{std::move(creators)...}
			{}
			Interface* operator()(const ConditionData& key) const
			{
				if (Interface* (*resultCreator)(const factory_method_t&) = nullptr; 
					((resultCreator = resultCreator ? resultCreator : condition_creator_or_nullptr<InterfaceCreators>(key)) , ...))
				{
					return std::invoke(*resultCreator, *this);
				}
				else
				{
					throw factory_method::condition_not_satisfied();
				}
			}
		private:
			std::tuple<InterfaceCreators...> creators;
		};

			
		template <typename Interface, typename ConditionData, typename ...Conditions, typename ...Creators>
		factory_method_t(
			interface_holder<Interface>, 
			condition_data_holder<ConditionData>, 
			factory_method::if_create<Conditions, Creators>...) -> factory_method_t<Interface, ConditionData, factory_method::if_create<Conditions, Creators>...>;
	}
	
	namespace factory_method
	{
		struct condition_not_satisfied
			: public std::runtime_error
		{
			condition_not_satisfied()
				: std::runtime_error{"No condition was satisfied"}
			{}
		};

		template <typename Condition, typename Creator>
		struct if_create
		{
			Condition condition;
			Creator creator;
		};

		template <typename Condition, typename Creator>
		if_create(Condition&&, Creator&&) -> if_create<Condition, Creator>;

		template <typename Interface, typename ConditionData, typename ...Conditions, typename ...Creators>
		constexpr auto factory_method(if_create<Conditions, Creators> ...if_creats) 
		{ 
			return factory_method_utils::factory_method_t
			{
				factory_method_utils::interface_holder<Interface>{}, 
				factory_method_utils::condition_data_holder<ConditionData>{}, 
				std::move(if_creats)...
			};
		};
	}
}
