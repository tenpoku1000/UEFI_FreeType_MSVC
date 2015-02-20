#ifndef _EFISTDARG_H_
#define _EFISTDARG_H_

/*++

Copyright (c) 1998  Intel Corporation

Module Name:

    devpath.h

Abstract:

    Defines for parsing the EFI Device Path structures



Revision History

--*/

/* @@@ */
#ifndef _MSC_VER
typedef __builtin_va_list va_list;

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#define va_copy(d,s)	__builtin_va_copy(d,s)
#else
/**
FILE: UDK2014.SP1.Complete.MyWorkSpace\UDK2014.SP1.MyWorkSpace\MyWorkSpace\MdePkg\Include\Base.h

Copyright(c) 2006 - 2013, Intel Corporation.All rights reserved.<BR>
Portions copyright(c) 2008 - 2009, Apple Inc.All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.The full text of the license may be found at
http ://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/**
Return the size of argument that has been aligned to sizeof (UINTN).

@param  n    The parameter size to be aligned.

@return The aligned size.
**/
#define _INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))

/**
Variable used to traverse the list of arguments. This type can vary by
implementation and could be an array or structure.
**/
typedef CHAR8* va_list;

/**
Retrieves a pointer to the beginning of a variable argument list, based on
the name of the parameter that immediately precedes the variable argument list.

This function initializes Marker to point to the beginning of the variable
argument list that immediately follows Parameter.  The method for computing the
pointer to the next argument in the argument list is CPU-specific following the
EFIAPI ABI.

@param   Marker       The VA_LIST used to traverse the list of arguments.
@param   Parameter    The name of the parameter that immediately precedes
the variable argument list.

@return  A pointer to the beginning of a variable argument list.

**/
#define va_start(Marker, Parameter) (Marker = (va_list) ((UINTN) & (Parameter) + _INT_SIZE_OF (Parameter)))

/**
Returns an argument of a specified type from a variable argument list and updates
the pointer to the variable argument list to point to the next argument.

This function returns an argument of the type specified by TYPE from the beginning
of the variable argument list specified by Marker.  Marker is then updated to point
to the next argument in the variable argument list.  The method for computing the
pointer to the next argument in the argument list is CPU-specific following the EFIAPI ABI.

@param   Marker   VA_LIST used to traverse the list of arguments.
@param   TYPE     The type of argument to retrieve from the beginning
of the variable argument list.

@return  An argument of the type specified by TYPE.

**/
#define va_arg(Marker, TYPE)   (*(TYPE *) ((Marker += _INT_SIZE_OF (TYPE)) - _INT_SIZE_OF (TYPE)))

/**
Terminates the use of a variable argument list.

This function initializes Marker so it can no longer be used with VA_ARG().
After this macro is used, the only way to access the variable argument list is
by using VA_START() again.

@param   Marker   VA_LIST used to traverse the list of arguments.

**/
#define va_end(Marker)      (Marker = (va_list) 0)

/**
Initializes a VA_LIST as a copy of an existing VA_LIST.

This macro initializes Dest as a copy of Start, as if the VA_START macro had been applied to Dest
followed by the same sequence of uses of the VA_ARG macro as had previously been used to reach
the present state of Start.

@param   Dest   VA_LIST used to traverse the list of arguments.
@param   Start  VA_LIST used to traverse the list of arguments.

**/
#define va_copy(Dest, Start)  ((void)((Dest) = (Start)))

#endif
#endif
