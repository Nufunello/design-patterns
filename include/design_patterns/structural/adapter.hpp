namespace design_patterns::structural
{
	namespace adapter_utils
	{
		template <typename Interface, typename Implementation, typename Method = decltype(&Interface::operator())>
		class adapter_t;

		template <typename Interface, typename Implementation, typename Return, typename ...Args>
		class adapter_t<Interface, Implementation, Return (Interface::*)(Args...)>
			: public Interface
		{
			Implementation implementation;
		public:
			adapter_t(Implementation implementation)
				: implementation{std::move(implementation)}
			{}
			Return operator()(Args ...args) override
			{
				return implementation(std::forward<Args>(args)...);
			}
		};

		template <typename Interface, typename Implementation, typename Return, typename ...Args>
		class adapter_t<Interface, Implementation, Return (Interface::*)(Args...) const>
			: public Interface
		{
			Implementation implementation;
		public:
			adapter_t(Implementation implementation)
				: implementation{std::move(implementation)}
			{}
			Return operator()(Args ...args) const override
			{
				return implementation(std::forward<Args>(args)...);
			}
		};
	}

	namespace adapter
	{
		template <typename Interface, typename Implementation>
		auto adapter(Implementation&& implementation)
		{
			return adapter_utils::adapter_t<Interface, Implementation>{std::forward<Implementation>(implementation)};
		}
	}
}
