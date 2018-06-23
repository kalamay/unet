#ifndef UNET_ERROR_H
#define UNET_ERROR_H

#include <string>
#include <system_error>

namespace unet
{
	enum class error
	{
		already_open = 1,
		invalid_ipaddr,
		invalid_hwaddr,
	};

	const std::error_category &error_category();

	std::error_code make_error_code(error e);
}

namespace std
{
	template <>
	struct is_error_code_enum<unet::error> : std::true_type {};
}

#endif

