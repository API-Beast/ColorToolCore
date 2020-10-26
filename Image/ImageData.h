#pragma once

#include "../Math/Vector2.h"
#include <new>
#include <cstring>
#include <type_traits>
#include <cassert>
#include <memory>

constexpr int ImageData_SIMD_Alignment = 32;

template<typename T>
struct ImageData
{
	T* data = nullptr;
	int data_length = 0;
	Vec2i size = {0, 0};

	ImageData()=default;
	~ImageData(){ clear(); };

	ImageData(ImageData<T>&& other) noexcept;
	ImageData<T>& operator=(ImageData<T>&& other) noexcept;

	ImageData(const ImageData<T>& other);
	ImageData<T>& operator=(const ImageData<T>& other);

	template<typename ClrSpace>
	void copy_from(const ImageData<ClrSpace>& other);

	template<typename ClrSpace>
	ImageData<ClrSpace> make_copy(){ ImageData<ClrSpace> retVal; retVal.copy_from(*this); return retVal; };

	template<typename P, typename ClrSpace, int PxSize, int XOffset = 0, int YOffset = 1, int ZOffset = 2, int WOffset = -1>
	void import_c_array(int w, int h, P* ptr, int length);

	void allocate(int w, int h);
	void clear();
	constexpr int pixel_offset(){ return sizeof(T); };
	int row_offset() const { return sizeof(T) * size.x; };

	bool is_valid() const { return data != nullptr; };

	const T& operator[](int i) const{ return *(data+i); };
	      T& operator[](int i)      { return *(data+i); };

	T* begin(){ return data; };
	T* end()  { return data + data_length; };
	const T* begin() const { return data; };
	const T* end()   const { return data + data_length; };
};

template<typename T>
void ImageData<T>::allocate(int w, int h) 
{
	clear();
	data_length = w*h;
	data = (T*)::operator new(data_length * sizeof(T), std::align_val_t(ImageData_SIMD_Alignment));
	size.x = w;
	size.y = h;
}

template<typename T>
void ImageData<T>::clear() 
{
	::operator delete(data, std::align_val_t(ImageData_SIMD_Alignment));
}

template<typename T>
template<typename P, typename ClrSpace, int PxSize, int XOffset, int YOffset, int ZOffset, int WOffset>
void ImageData<T>::import_c_array(int w, int h, P* ptr, int length) 
{
	allocate(w, h);
	int pixel_length = length / PxSize;
	int overhead = data_length - pixel_length;
	assert(overhead >= 0);
	assert(length > 0);
	assert((length % PxSize) == 0);
	static_assert(PxSize == ((WOffset != -1) + (ZOffset != -1) + (YOffset != -1) + (XOffset != -1)));
	for(int i = 0; i<pixel_length; i++)
	{
		if constexpr(WOffset != -1)
			data[i] = colorspace_cast<T>(ClrSpace(ptr[i*PxSize+XOffset], ptr[i*PxSize+YOffset], ptr[i*PxSize+ZOffset], ptr[i*PxSize+WOffset]));
		else if constexpr(ZOffset != -1)
			data[i] = colorspace_cast<T>(ClrSpace(ptr[i*PxSize+XOffset], ptr[i*PxSize+YOffset], ptr[i*PxSize+ZOffset]));
		else if constexpr(YOffset != -1)
			data[i] = colorspace_cast<T>(ClrSpace(ptr[i*PxSize+XOffset], ptr[i*PxSize+YOffset]));
		else
			data[i] = colorspace_cast<T>(ClrSpace(ptr[i*PxSize+XOffset]));
	}
	std::uninitialized_default_construct_n(data+(length-1), overhead);
}

template<typename T>
ImageData<T>::ImageData(ImageData<T>&& other) noexcept
{
	this->operator=(std::move(other));
}

template<typename T>
ImageData<T>::ImageData(const ImageData<T>& other) 
{
	copy_from(other);
}

template<typename T>
ImageData<T>& ImageData<T>::operator=(ImageData<T>&& other)  noexcept
{
	if(other.data)
	{
		size = other.size;
		data = other.data;
		data_length = other.data_length;
		other.data = nullptr;
	}
	return *this;	
}

template<typename T>
ImageData<T>& ImageData<T>::operator=(const ImageData<T>& other) 
{
	copy_from(other);
	return *this;	
}

template<typename T>
template<typename ClrSpace>
void ImageData<T>::copy_from(const ImageData<ClrSpace>& other)
{
	if(other.data)
	{
		allocate(other.size.x, other.size.y);
		T* ptr = data;
		for(auto it = other.begin(); it != other.end(); it++)
		{
			(*ptr) = colorspace_cast<T>(*it); 
			ptr++;
		}
	}
}
