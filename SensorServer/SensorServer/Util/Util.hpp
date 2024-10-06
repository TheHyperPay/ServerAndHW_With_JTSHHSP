#pragma once
#include <algorithm>

#define UNDEFINE_NAME		L"Undefine_Name"

#define snprintf(dst, format, ...)	_snprintf_s(dst.data(), dst.size(), _TRUNCATE, format, __VA_ARGS__)
#define snwprintf(dst, format, ...)	_snwprintf_s(dst.data(), dst.size(), _TRUNCATE, format, __VA_ARGS__)

#define fixInRange(minimum, x, maximum)		min(maximum, max(x, minimum))
#define isInRange(minimum, x, maximum)		(x == fixInRange(minimum, x, maximum)) ? true : false


inline bool isOverFlow_uint(unsigned int original, unsigned int add) 
{
	return (original > UINT_MAX - add);
}

inline bool isOverFlow_uchar(unsigned char original, unsigned char add) 
{
	return (original > UCHAR_MAX - add);
}


//기본 매크로 회피용
#define __W(x)			L##x
#define _W(x)			__W(x)


#undef SAFE_DELETE
#define SAFE_DELETE(obj)			\
{									\
	if ((obj)) delete(obj);			\
	(obj) = 0L;						\
}

#undef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(arr)		\
{									\
	if ((arr)) delete[](arr);		\
	(arr) = 0L;						\
}

#undef SAFE_FREE
#define SAFE_FREE(obj)				\
{									\
	if ((obj)) obj->free();			\
	(obj) = 0L;						\
}

#undef SAFE_RELEASE
#define SAFE_RELEASE(obj)			\
{									\
	if (obj) { obj.release(); }		\
}