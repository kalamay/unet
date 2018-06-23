#ifndef UNET_ILIST_H
#define UNET_ILIST_H

#include <memory>
#include <cstddef>

namespace unet
{
	template <class T, class Deleter = std::default_delete<T>>
	class ilist
	{
	public:
		using self_type = ilist<T>;
		using pointer = T*;
		using unique_ptr = std::unique_ptr<T, Deleter>;
		using reference = T&;
		using const_reference = const T&;

		class entry
		{
			entry *link[2];

			inline void
			init_head() noexcept
			{
				link[0] = link[1] = this;
			}

			inline void
			copy_head(const entry &src) noexcept
			{
				link[0] = src.link[0];
				link[0]->link[1] = this;
				link[1] = src.link[1];
				link[1]->link[0] = this;
			}

			reference remove() noexcept
			{
				if (is_added()) {
					entry *tmp[2] = { link[0], link[1] };
					tmp[0]->link[1] = tmp[1];
					tmp[1]->link[0] = tmp[0];
					link[0] = nullptr;
					link[1] = nullptr;
				}
				return *static_cast<pointer>(this);
			}

		public:
			entry() noexcept : link{nullptr, nullptr} {}
			~entry() noexcept { remove(); }

			bool is_added() const noexcept { return link[0] != nullptr; }
			unique_ptr take() noexcept { return unique_ptr(&remove()); }

			friend class ilist;
		};

	private:
		template <class iter_type, class pointer, class reference, int dir = 1>
		class ilist_base_iterator
		{
		protected:
			iter_type &as_iter() noexcept { return *static_cast<iter_type *>(this); }
			const iter_type &as_iter() const noexcept { return *static_cast<const iter_type *>(this); }
			const entry *get_at() const noexcept { return as_iter().get(); }

		public:
			using iterator_category = std::bidirectional_iterator_tag;

			ilist_base_iterator() noexcept {}

			reference operator*() noexcept { return *static_cast<pointer>(as_iter().get()); }
			pointer operator->() noexcept { return static_cast<pointer>(as_iter().get()); }
			bool operator==(const iter_type& rhs) const noexcept { return get_at() == rhs.get_at(); }
			bool operator!=(const iter_type& rhs) const noexcept { return get_at() != rhs.get_at(); }

			iter_type operator++() noexcept { iter_type i = as_iter(); as_iter().move_next(); return i; }
			iter_type operator++(int _) noexcept { (void)_; as_iter().move_next(); return as_iter(); }
			iter_type operator--() noexcept { iter_type i = as_iter(); as_iter().move_prev(); return i; }
			iter_type operator--(int _) noexcept { (void)_; as_iter().move_prev(); return as_iter(); }

			friend class ilist<T>;
		};

		template <int dir>
		class ilist_const_iterator :
			public ilist_base_iterator<ilist_const_iterator<dir>, const T *, const T &, dir>
		{
			const entry *at;

		public:
			using iter_type = ilist_const_iterator<dir>;

			ilist_const_iterator(const entry *at) noexcept
				: at(at) {}

			void move_next() noexcept { at = at->link[dir]; }
			void move_prev() noexcept { at = at->link[!dir]; }
			const entry *get() const noexcept { return at; }
		};

		template <int dir>
		class ilist_iterator :
			public ilist_base_iterator<ilist_iterator<dir>, T *, T &, dir>
		{
			entry *at, *next, *prev;

		public:
			using iter_type = ilist_iterator<dir>;

			ilist_iterator(entry *at) noexcept
				: at(at), next(at->link[dir]), prev(at->link[!dir]) {}

			void move_next() noexcept { prev = at; at = next; next = next->link[dir]; }
			void move_prev() noexcept { next = at; at = prev; prev = prev->link[!dir]; }
			entry *get() const noexcept { return at; }
		};

		template <int dir>
		bool has_dir(const_reference e) const noexcept
		{
			return e.link[dir] != &head;
		}

		template <int dir>
		reference get_dir(entry &e) noexcept
		{
			return *static_cast<pointer>(e.link[dir]);
		}

		template <int dir>
		void push_dir_force(reference e) noexcept
		{
			entry *link[2];
			link[!dir] = &head;
			link[dir] = head.link[dir];
			link[1]->link[0] = &e;
			e.link[1] = link[1];
			e.link[0] = link[0];
			link[0]->link[1] = &e;
		}

		void insert_force(entry &before, reference e) noexcept
		{
			before.link[0]->link[1] = &e;
			e.link[0] = before.link[0];
			e.link[1] = &before;
			before.link[0] = &e;
		}

		template <int dir>
		void push_dir(reference e) noexcept
		{
			push_dir_force<dir>(e.remove());
		}

		template <int dir>
		void splice_dir(entry &at, self_type &other) noexcept
		{
			if (!other.is_empty()) {
				entry *tmp[2];
				tmp[dir] = at.link[dir];
				tmp[!dir] = &at;
				tmp[dir]->link[!dir] = other.head.link[!dir];
				other.head.link[!dir]->link[dir] = tmp[dir];
				other.head.link[dir]->link[!dir] = tmp[!dir];
				tmp[!dir]->link[dir] = other.head.link[dir];
				other.head.init_head();
			}
		}

	public:
		using iterator = ilist_iterator<1>;
		using const_iterator = ilist_const_iterator<1>;
		using reverse_iterator = ilist_iterator<0>;
		using const_reverse_iterator = ilist_const_iterator<0>;

		ilist() noexcept { head.init_head(); }
		~ilist() noexcept { clear(); }

		ilist(const ilist &src) = delete;
		ilist &operator=(const ilist &src) = delete;

		ilist(ilist &&src) noexcept
		{
			clear();
			head.copy_head(src);
			src.init_head();
		}

		ilist &operator=(ilist &&src) noexcept
		{
			clear();
			head.copy_head(src);
			src.init_head();
			return *this;
		}

		iterator begin() noexcept { return iterator(head.link[1]); }
		iterator end() noexcept { return iterator(&head); }
		const_iterator begin() const noexcept { return const_iterator(head.link[1]); }
		const_iterator end() const noexcept { return const_iterator(&head); }
		const_iterator cbegin() const noexcept { return const_iterator(head.link[1]); }
		const_iterator cend() const noexcept { return const_iterator(&head); }
		reverse_iterator rbegin() noexcept { return reverse_iterator(head.link[0]); }
		reverse_iterator rend() noexcept { return reverse_iterator(&head); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(head.link[0]); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(&head); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(head.link[0]); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(&head); }

		bool is_empty() const noexcept { return head.link[1] == &head; }
		bool is_singular() const noexcept { return !is_empty() && head.link[0] == head.link[1]; }

		reference front() noexcept { return get_dir<1>(head); }
		reference back() noexcept { return get_dir<0>(head); }

		bool has_next(reference e) const noexcept { return has_dir<1>(e); }
		bool has_prev(reference e) const noexcept { return has_dir<0>(e); }

		reference next(reference e) noexcept { return get_dir<1>(e); }
		reference prev(reference e) noexcept { return get_dir<0>(e); }

		void push_front(reference e) noexcept { push_dir<1>(e); }
		void push_back(reference e) noexcept { push_dir<0>(e); }

		void push_front(const_reference e) noexcept { push_dir_force<1>(std::copy(e)); }
		void push_back(const_reference e) noexcept { push_dir_force<0>(std::copy(e)); }

		void insert(reference before, reference e) noexcept { insert_force(before, e.remove()); }
		void insert(reference before, const_reference e) { insert_force(before, std::copy(e)); }
		void insert(iterator before, reference e) noexcept { insert_force(before.at, e.remove()); }
		void insert(iterator before, const_reference e) { insert_force(before.at, std::copy(e)); }

		void splice(reference before, self_type &other) noexcept { splice_dir<0>(before, other); }
		void splice(iterator before, self_type &other) noexcept { splice_dir<0>(*before, other); }

		unique_ptr take(pointer e) noexcept { return e->take(); }
		unique_ptr take(reference e) noexcept { return take(&e); }
		unique_ptr take(iterator *it) noexcept { auto e = take(*it); it->at = nullptr; return e; }
		unique_ptr take(iterator &it) noexcept { auto e = take(&it); it.at = nullptr; return e; }
		unique_ptr take(reverse_iterator *it) noexcept { auto e = take(*it); it->at = nullptr; return e; }
		unique_ptr take(reverse_iterator &it) noexcept { auto e = take(&it); it.at = nullptr; return e; }

		unique_ptr take_front() noexcept { return take(front()); }
		unique_ptr take_back() noexcept { return take(back()); }

		void erase(pointer e) noexcept { e->take(); }
		void erase(reference e) noexcept { erase(&e); }
		void erase(iterator *it) noexcept { erase(*it); it->at = nullptr; }
		void erase(iterator &it) noexcept { erase(&it); it.at = nullptr; }
		void erase(reverse_iterator *it) noexcept { erase(*it); it->at = nullptr; }
		void erase(reverse_iterator &it) noexcept { erase(&it); it.at = nullptr; }

		void pop_front() noexcept { erase(front()); }
		void pop_back() noexcept { erase(back()); }

		void clear() noexcept { for (auto &e : *this) { erase(e); } }

		void swap(self_type &other) noexcept
		{
			entry tmp;
			tmp.copy_head(head);
			head.copy_head(other.head);
			other.head.copy_head(tmp);
		}

	private:

		entry head;
	};
}

namespace std
{
	template <class T>
	void swap(unet::ilist<T> &lhs, unet::ilist<T> &rhs)
	{
		lhs.swap(rhs);
	}
}

#endif

