#ifndef UNET_SLICE_H
#define UNET_SLICE_H

#include <string>
#include <cstring>

namespace unet
{
	class slice
	{
		uint8_t *val = nullptr;
		size_t len = 0;

	public:
		slice() noexcept {}
		slice(uint8_t *val, size_t len) noexcept : val(val), len(len) {}
		slice(char *str) noexcept : slice(reinterpret_cast<uint8_t *>(str), std::strlen(str)) {}

		slice(slice &&other) noexcept :
			val(other.val),
			len(other.len)
		{
			other.val = nullptr;
			other.len = 0;
		}

		slice &operator=(slice &&other) noexcept
		{
			val = other.val;
			len = other.len;
			other.val = nullptr;
			other.len = 0;
			return *this;
		}

		slice(const slice &other) noexcept :
			val(other.val),
			len(other.len) {}

		slice &operator=(const slice &other) noexcept
		{
			val = other.val;
			len = other.len;
			return *this;
		}

		uint8_t *value() noexcept { return val; }
		const uint8_t *value() const noexcept { return val; }
		size_t length() const noexcept { return len; }
		bool is_empty() const noexcept { return len == 0; }

		int compare(const uint8_t *v, size_t n) const noexcept
		{
			int def, cmp;
			if (len < n) {
				def = -1;
				n = len;
			}
			else {
				def = len > n;
			}
			if (n == 0) { return def; }
			cmp = std::memcmp(val, v, n);
			return cmp == 0 ? def : cmp;
		}

		int compare(const char *str) const noexcept
		{
			return compare(reinterpret_cast<const uint8_t *>(str), std::strlen(str));
		}

		int compare(const slice &to) const noexcept
		{
			return compare(to.val, to.len);
		}

		int compare(const std::string &str) const noexcept
		{
			return compare(reinterpret_cast<const uint8_t *>(str.data()), str.size());
		}

		bool starts_with(const uint8_t *v, size_t n) const noexcept
		{
			return len >= n && std::memcmp(val, v, n) == 0;
		}

		bool starts_with(const char *str) const noexcept
		{
			return starts_with(reinterpret_cast<const uint8_t *>(str), std::strlen(str));
		}

		bool starts_with(const slice &to) const noexcept
		{
			return starts_with(to.val, to.len);
		}

		bool starts_with(const std::string &str) noexcept
		{
			return starts_with(reinterpret_cast<const uint8_t *>(str.data()), str.size());
		}

		bool starts_with(char ch) const noexcept
		{
			return len > 0 && val[0] == ch;
		}

		bool ends_with(const uint8_t *v, size_t n) const noexcept
		{
			return len >= n && std::memcmp(val + (len-n), v, n) == 0;
		}

		bool ends_with(const char *str) const noexcept
		{
			return ends_with(reinterpret_cast<const uint8_t *>(str), std::strlen(str));
		}

		bool ends_with(const slice &to) const noexcept
		{
			return ends_with(to.val, to.len);
		}

		bool ends_with(const std::string &str) noexcept
		{
			return ends_with(reinterpret_cast<const uint8_t *>(str.data()), str.size());
		}

		bool ends_with(char ch) const noexcept
		{
			return len > 0 && val[len-1] == ch;
		}

		ssize_t find(char ch) const noexcept
		{
			if (len > 0) {
				uint8_t *m = static_cast<uint8_t *>(std::memchr(val, ch, len));
				if (m) { return m - val; }
			}
			return -1;
		}

		slice sub(size_t start, size_t n) const noexcept
		{
			if (start >= len) {
				return slice();
			}
			else if (start + n > len) {
				n = len - start;
			}
			return slice(val + start, n);
		}

		slice trim(size_t left, size_t right) const noexcept
		{
			if (left + right >= len) {
				return slice();
			}
			return slice(val + left, len - left - right);
		}

		slice trim_left(size_t left) const noexcept
		{
			if (left >= len) {
				return slice();
			}
			return slice(val + left, len - left);
		}

		slice trim_right(size_t right) const noexcept
		{
			if (right >= len) {
				return slice();
			}
			return slice(val, len - right);
		}

		void split(size_t off, slice &left, slice &right) const noexcept
		{
			uint8_t *v = val;
			size_t n = len;

			if (off <= 0) { off = 0; }
			else if (off >= n) { off = n; }

			left.val = v;
			left.len = off;
			right.val = v + off;
			right.len = n - off;
		}

		size_t copy(uint8_t *dst, size_t n) const noexcept
		{
			if (n > len) {
				n = len;
			}
			std::memcpy(dst, val, n);
			return n;
		}

		template <typename T>
		T &as() noexcept { return *reinterpret_cast<T *>(val); }

		template <typename T>
		const T &as() const noexcept { return *reinterpret_cast<T *>(val); }

		operator std::string() const noexcept
		{
			if (length() == 0) { return std::string(); }
			return std::string(reinterpret_cast<const char *>(value()), length());
		}

		void clear() noexcept
		{
			val = nullptr;
			len = 0;
		}

		bool operator==(const slice &to) const noexcept
		{
			return len == to.len && (
					len == 0 ||
					std::memcmp(val, to.val, len) == 0);
		}
		bool operator!=(const slice &to) const noexcept { return !(*this == to); }
		bool operator<(const slice &to) const noexcept { return compare(to) < 0; }
		bool operator<=(const slice &to) const noexcept { return compare(to) <= 0; }
		bool operator>(const slice &to) const noexcept { return compare(to) > 0; }
		bool operator>=(const slice &to) const noexcept { return compare(to) >= 0; }

		bool operator==(const std::string &to) const noexcept
		{
			return len == to.size() && (
					len == 0 ||
					std::memcmp(val, to.data(), len) == 0);
		}
		bool operator!=(const std::string &to) const noexcept { return !(*this == to); }
		bool operator<(const std::string &to) const noexcept { return compare(to) < 0; }
		bool operator<=(const std::string &to) const noexcept { return compare(to) <= 0; }
		bool operator>(const std::string &to) const noexcept { return compare(to) > 0; }
		bool operator>=(const std::string &to) const noexcept { return compare(to) >= 0; }

		bool operator==(const char *to) const noexcept
		{
			size_t n = std::strlen(to);
			return len == n && (
					len == 0 ||
					std::memcmp(val, to, len) == 0);
		}
		bool operator!=(const char *to) const noexcept { return !(*this == to); }
		bool operator<(const char *to) const noexcept { return compare(to) < 0; }
		bool operator<=(const char *to) const noexcept { return compare(to) <= 0; }
		bool operator>(const char *to) const noexcept { return compare(to) > 0; }
		bool operator>=(const char *to) const noexcept { return compare(to) >= 0; }
	};
}

#endif

