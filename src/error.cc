#include "error.h"

class error_category_impl : public std::error_category
{
public:
	virtual const char *name() const noexcept override final
	{
		return "unet";
	}

	virtual std::string message(int c) const override final
	{
		switch (static_cast<unet::error>(c)) {
		case unet::error::already_open: return "Device is already open";
		case unet::error::invalid_hwaddr: return "Invalid hardware address";
		case unet::error::invalid_ipaddr: return "Invalid IP address";
		default: return "Unknown error";
		}
	}
};

const std::error_category &unet::error_category()
{
	static const error_category_impl instance;
	return instance;
}

std::error_code unet::make_error_code(error e)
{
	return std::error_code(static_cast<int>(e), error_category());
}

