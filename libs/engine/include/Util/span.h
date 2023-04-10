#ifndef POSTAENGINE_SPAN_UTILITY_H
#define POSTAENGINE_SPAN_UTILITY_H
#include <array>
#include <iostream>

namespace Engine {
	template<class T>
	class span
	{
		struct iterator
		{
			iterator()
			{
				_ptr = nullptr;
			}

			iterator(T* ptr)
			{
				_ptr = ptr;
			}

			iterator& operator++()
			{
				_ptr++;
				return *this;
			}

			iterator& operator--()
			{
				_ptr--;
				return *this;
			}

			iterator operator++(int)
			{
				iterator it = *this;
				++*this;
				return it;
			}

			iterator operator--(int)
			{
				iterator it = *this;
				--*this;
				return it;
			}

			bool operator!=(const iterator& other) const
			{
				return _ptr != other._ptr;
			}

			T& operator*() const
			{
				return *_ptr;
			}


		private:
			T* _ptr;
		};

		public:
			span()
			{
				_ptr = nullptr;
				_size = 0;
			}
			span(T* ptr, size_t size)
			{
				_ptr = ptr;
				_size = size;
			}

			T* data()
			{
				return _ptr;
			}

			iterator begin()
			{
				return iterator(_ptr);
			}

			iterator end()
			{
				return iterator(_ptr + _size);
			}

			size_t size()
			{
				return this->_size;
			}

			bool empty()
			{
				return this->_size == 0;
			}

			T& operator[](size_t index)
			{
				return *(_ptr + index);
			}
		private:
			T* _ptr;
			size_t _size;
	};

	template<template<class> class C, class T>
	span<T> make_span(C<T>& c)
	{
		return span<T>(c.data(), c.size());
	}

	template<class T, const size_t SIZE>
	span<T> make_span(std::array<T, SIZE>& array)
	{
		return span<T>(array.data(), SIZE);
	}
}

#endif // POSTAENGINE_SPAN_UTILITY_H
