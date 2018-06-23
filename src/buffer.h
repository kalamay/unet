#ifndef UNET_BUFFER_H
#define UNET_BUFFER_H

#include <iostream>
#include <cstddef>

#include "base.h"
#include "ilist.h"
#include "slice.h"

namespace unet
{
	template <typename T>
	class buffer_impl
	{
		unsigned int len = 0;

		T &as_buffer() { return *static_cast<T *>(this); }
		const T &as_buffer() const { return *static_cast<const T *>(this); }

	public:
		unsigned int length() const { return len; }
		unsigned int available() const { return as_buffer().size() - len; }

		void bump(unsigned int n) { len = std::min(len+n, as_buffer().size()); }

		slice begin() { return slice(as_buffer().data(), len); }
		slice end() { return slice(as_buffer().data() + len, available()); }
	};

	class buffer :
		public ilist<buffer>::entry, public buffer_impl<buffer>,
		private nocopy, private nomove
	{
		unsigned int cap;
		uint8_t buf[0];

		void *operator new(size_t cap, unsigned int n)
		{
			return calloc(1, cap + n);
		}

		buffer(unsigned int n) : cap(n) {}

	public:
		static buffer *create(unsigned int n) { return new(n) buffer(n); }

		unsigned int size() const { return cap; }
		uint8_t *data() { return buf; }
		const uint8_t *data() const { return buf; }
	};

	template <int N>
	class static_buffer :
		public ilist<static_buffer<N>>::entry, public buffer_impl<static_buffer<N>>, 
		private nocopy, private nomove
	{
		unsigned int _pad;
		uint8_t buf[N];

	public:
		unsigned int size() const { return N; }
		uint8_t *data() { return buf; }
		const uint8_t *data() const { return buf; }
	};

	static_assert(sizeof(buffer) == 24, "buffer size invalid");
	static_assert(sizeof(static_buffer<0>) == 24, "static_buffer size invalid");
	static_assert(sizeof(static_buffer<64>) == 88, "static_buffer size invalid");
}

#endif

