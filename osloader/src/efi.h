//
// Small EFI headers for osloader
// Implemented using UEFI 2.11 SPEC
//

#pragma once

#include <stdint.h>
#include <stddef.h>

#define EFIAPI __attribute__((ms_abi)) // Microsoft calling convention

// UEFI Data types
#define FALSE 0
#define TRUE
typedef uint8_t  BOOLEAN;
typedef int64_t  INTN;
typedef uint64_t UINTN;
typedef int8_t   INT8;
typedef uint8_t  UINT8;
typedef int16_t  INT16;
typedef uint16_t UINT16;
typedef int32_t  INT32;
typedef uint32_t UINT32;
typedef int64_t  INT64;
typedef uint64_t UINT64;
typedef char     CHAR8;
typedef uint16_t CHAR16;
typedef void     VOID;

// EFI GUID

typedef UINTN    EFI_STATUS;
typedef VOID*    EFI_HANDLE;
typedef VOID*    EFI_EVENT;
typedef UINT64   EFI_LBA;
typedef UINTN    EFI_TPL;

// UEFI spec Appendix D
#define EFI_SUCCESS 0



