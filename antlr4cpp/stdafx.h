// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable: 4820) // 'bytes' bytes padding added after construct 'member_name'
#pragma warning(disable: 4710) // 'function' : function not inlined
#pragma warning(disable: 4512) // 'class' : assignment operator could not be generated
#pragma warning(disable: 4625) // 'derived class' : copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable: 4626) // 'derived class' : assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable: 4350) // behavior change: 'member1' called instead of 'member2' (An rvalue cannot be bound to a non-const reference.)

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
// new warnings in Visual Studio 2015
#pragma warning(disable: 5025) // 'derived class': move assignment operator was implicitly defined as deleted
#pragma warning(disable: 5026) // 'derived class': move constructor was implicitly defined as deleted because a base class move constructor is inaccessible or deleted
#pragma warning(disable: 5027) // 'derived class': move assignment operator was implicitly defined as deleted because a base class move assignment operator is inaccessible or deleted
#endif

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
