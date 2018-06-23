#ifndef UNET_BASE_H
#define UNET_BASE_H

namespace unet
{
	class nocopy
	{
	protected:
		constexpr nocopy() = default;
		~nocopy() = default;

		nocopy(const nocopy &) = delete;
		nocopy &operator=(const nocopy &) = delete;
	};

	class nomove
	{
	protected:
		constexpr nomove() = default;
		~nomove() = default;

		nomove(nomove &&) = delete;
		nomove &operator=(nomove &&) = delete;
	};
}

#endif

