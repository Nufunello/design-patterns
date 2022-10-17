#pragma once

#include <functional>
#include <type_traits>

namespace design_patterns::creational 
{
	namespace builder_util
	{
		template <typename T, typename Indexes>
		struct fixed_array;

		template <typename T, std::size_t ...Indexes>
		struct fixed_array<T, std::index_sequence<Indexes...>>
		{
			constexpr static auto N = sizeof...(Indexes);
			constexpr fixed_array(const T (&array)[N])
				: array{array[Indexes]...}
			{}

			T array[N];
		};

		template <typename T, std::size_t N>
		fixed_array(const T (&)[N]) -> fixed_array<T, std::make_index_sequence<N>>; 

		template <typename T>
		struct name
		{
			template <typename... Args>
			constexpr name(Args&& ...args)
				: value{std::forward<Args>(args)...}
			{}
			constexpr name(T arg)
				: value{arg}
			{}
			std::decay_t<T> value;
		};

		template <typename T, std::size_t N>
		name(const T (&array)[N]) -> name<fixed_array<T, std::make_index_sequence<N>>>;

		template <typename T>
		name(T&&) -> name<T>;

		template <name Name, typename Setter>
		class member
		{
		public:
			Setter setter;
		};

		template <typename Data, typename Build, typename...>
		class builder;

		template <typename Data, typename Build>
		class builder<Data, Build>
		{
		public:
			constexpr builder(Data data, Build build)
				: data{std::move(data)}
				, build{std::move(build)}
			{}
			template <name Name, typename Setter>
			constexpr decltype(auto) withSetter(Setter&& setter)
			{
				return builder<Data, Build, builder, member<Name, Setter>>
				{
					std::forward<builder>(*this), 
					member<Name, Setter>{std::move(setter)}
				};
			}
			constexpr decltype(auto) perform_build()
			{
				return std::invoke(build, data);
			}
		protected:
			Data data;
		private:
			Build build;
		};

		template <typename Data, typename Build>
		builder(Data&&, Build&&) -> builder<Data, Build>;

		template <typename Data, typename Build, typename PrevBuilder, typename Member>
		class builder<Data, Build, PrevBuilder, Member>
			: public PrevBuilder
			, protected Member
		{
		public:
			template <name Name, typename... Args>
			constexpr decltype(auto) operator()(Args&& ...args)
			{
				std::invoke(resolve<Name>(*this), builder<Data, Build>::data, std::forward<Args>(args)...);
				return (*this);
			}
			template <name Name, typename Setter>
			constexpr decltype(auto) withSetter(Setter&& setter)
			{
				return builder<Data, Build, builder, member<Name, Setter>>
				{
					std::forward<builder>(*this), 
					member<Name, Setter>{std::move(setter)}
				};
			}
		private:
			friend PrevBuilder;
			template <name Name, typename Setter>
			constexpr builder(PrevBuilder prev, member<Name, Setter> member)
				: PrevBuilder{std::move(prev)}
				, Member{std::move(member)}
			{}
			template <name Name, typename Setter>
			constexpr decltype(auto) resolve(const member<Name, Setter>& resolved) const
			{
				return (resolved.setter);
			}
		};
	}

	namespace builder
	{
		template <typename Data, typename BuildPerformer>
		constexpr decltype(auto) builder(Data&& data, BuildPerformer&& buildPerformer)
		{
			return builder_util::builder{std::forward<Data>(data), std::forward<BuildPerformer>(buildPerformer)};
		}
	}
}
