#ifndef UNET_RANGE_H
#define UNET_RANGE_H

namespace unet
{
	class range
	{
		unsigned off;
		unsigned len;

	public:
		range(unsigned off = 0, unsigned len = 0)
			: off(off), len(len) {}

		range(range &&other) noexcept :
			off(other.off),
			len(other.len)
		{
			other.off = 0;
			other.len = 0;
		}

		range &operator=(range &&other) noexcept
		{
			off = other.off;
			len = other.len;
			other.off = 0;
			other.len = 0;
			return *this;
		}

		range(const range &other) noexcept :
			off(other.off),
			len(other.len) {}

		range &operator=(const range &other) noexcept
		{
			off = other.off;
			len = other.len;
			return *this;
		}

		unsigned offset() const { return off; }
		unsigned length() const { return len; }
		unsigned end() const { return off+len; }
		bool is_empty() const { return len == 0; }

		int compare(unsigned o, unsigned l) const
		{
			if (off < o) { return -1; }
			if (off > o) { return 1; }
			if (len < l) { return -1; }
			if (len > l) { return 1; }
			return 0;
		}

		int compare(const range &to) const
		{
			return compare(to.off, to.len);
		}

		bool operator==(const range &to) const
		{
			return off == to.off && len == to.len;
		}
		bool operator!=(const range &to) const { return !(*this == to); }
		bool operator<(const range &to) const { return compare(to) < 0; }
		bool operator<=(const range &to) const { return compare(to) <= 0; }
		bool operator>(const range &to) const { return compare(to) > 0; }
		bool operator>=(const range &to) const { return compare(to) >= 0; }
	};
}

#endif

