// Copyright (c) 2019-2020 OuterC - Nomango

#pragma once
#include <type_traits>

namespace oc
{

namespace __vector_details
{

// vector_memory_manager<> with memory operations
template<typename _Ty, typename _Alloc, bool _IsClassType = std::is_class<_Ty>::value>
struct vector_memory_manager;

//
// vector_memory_manager for common type
//
template<typename _Ty, typename _Alloc>
struct vector_memory_manager<_Ty, _Alloc, false>
{
	using value_type		= _Ty;
	using size_type			= size_t;
	using allocator_type	= typename _Alloc;

	static void copy_data(value_type* dest, const value_type* src, size_type count) { if (src == dest) return; ::memcpy(dest, src, size_type(count) * sizeof(value_type)); }
	static void copy_data(value_type* dest, size_type count, const value_type& val) { ::memset(dest, int(val), size_type(count) * sizeof(value_type)); }
	static void move_data(value_type* dest, const value_type* src, size_type count) { if (src == dest) return; ::memmove(dest, src, size_type(count) * sizeof(value_type)); }

	static value_type* allocate(size_type count)									{ return get_allocator().allocate(count); }
	static void deallocate(value_type*& ptr, size_type count) { if (ptr)			{ get_allocator().deallocate(ptr, count); ptr = nullptr; } }

	static void construct(value_type* ptr, size_type count)							{ }
	static void construct(value_type* ptr, size_type count, const value_type& val)	{ while (count) { --count; *(ptr + count) = val; } }
	static void destroy(value_type* ptr, size_type count)							{ }

private:
	static allocator_type& get_allocator()
	{
		static allocator_type allocator_;
		return allocator_;
	}
};

//
// vector_memory_manager for classes
//
template<typename _Ty, typename _Alloc>
struct vector_memory_manager<_Ty, _Alloc, true>
{
	using value_type		= _Ty;
	using size_type			= size_t;
	using allocator_type	= typename _Alloc;

	static void copy_data(value_type* dest, const value_type* src, size_type count) { if (src == dest) return; while (count--) (*dest++) = (*src++); }
	static void copy_data(value_type* dest, size_type count, const value_type& val) { while (count--) (*dest++) = val; }
	static void move_data(value_type* dest, const value_type* src, size_type count)
	{
		if (src == dest) return;
		if (dest > src && dest < src + count)
		{
			src = src + count - 1;
			dest = dest + count - 1;
			while (count--)
				(*dest--) = (*src--);
		}
		else
		{
			while (count--)
				(*dest++) = (*src++);
		}
	}

	static value_type* allocate(size_type count)									{ return get_allocator().allocate(count); }
	static void deallocate(value_type*& ptr, size_type count)						{ if (ptr) { get_allocator().deallocate(ptr, count); ptr = nullptr; } }

	static void construct(value_type* ptr, size_type count)							{ construct(ptr, count, value_type()); }
	static void construct(value_type* ptr, size_type count, const value_type& val)	{ while (count) get_allocator().construct(ptr + (--count), val); }
	static void destroy(value_type* ptr, size_type count)							{ while (count) get_allocator().destroy(ptr + (--count)); }

private:
	static allocator_type& get_allocator()
	{
		static allocator_type allocator_;
		return allocator_;
	}
};

}  // namespace __vector_details
}  // namespace oc
