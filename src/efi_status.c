
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include <efi.h>
#include <efilib.h>
#include "efi_status.h"

struct msg_tbl_{
    EFI_STATUS value;
    CHAR16* description;
};

typedef struct msg_tbl_ err_msg_tbl;
typedef struct msg_tbl_ warn_msg_tbl;

static err_msg_tbl err_tbl[] = {
    { EFI_SUCCESS, L"EFI_SUCCESS:The operation completed successfully." },
    { EFI_LOAD_ERROR, L"EFI_LOAD_ERROR:The image failed to load." },
    { EFI_INVALID_PARAMETER, L"EFI_INVALID_PARAMETER:A parameter was incorrect." },
    { EFI_UNSUPPORTED, L"EFI_UNSUPPORTED:The operation is not supported." },
    { EFI_BAD_BUFFER_SIZE, L"EFI_BAD_BUFFER_SIZE:The buffer was not the proper size for the request." },
    { EFI_BUFFER_TOO_SMALL, L"EFI_BUFFER_TOO_SMALL:The buffer is not large enough to hold the requested data." },
    { EFI_NOT_READY, L"EFI_NOT_READY:There is no data pending upon return." },
    { EFI_DEVICE_ERROR, L"EFI_DEVICE_ERROR:The physical device reported an error while attempting the operation." },
    { EFI_WRITE_PROTECTED, L"EFI_WRITE_PROTECTED:The device cannot be written to." },
    { EFI_OUT_OF_RESOURCES, L"EFI_OUT_OF_RESOURCES:A resource has run out." },
    { EFI_VOLUME_CORRUPTED, L"EFI_VOLUME_CORRUPTED:An inconstancy was detected on the file system causing the operating to fail." },
    { EFI_VOLUME_FULL, L"EFI_VOLUME_FULL:There is no more space on the file system." },
    { EFI_NO_MEDIA, L"EFI_NO_MEDIA:The device does not contain any medium to perform the operation." },
    { EFI_MEDIA_CHANGED, L"EFI_MEDIA_CHANGED:The medium in the device has changed since the last access." },
    { EFI_NOT_FOUND, L"EFI_NOT_FOUND:The item was not found." },
    { EFI_ACCESS_DENIED, L"EFI_ACCESS_DENIED:Access was denied." },
    { EFI_NO_RESPONSE, L"EFI_NO_RESPONSE:The server was not found or did not respond to the request." },
    { EFI_NO_MAPPING, L"EFI_NO_MAPPING:A mapping to a device does not exist." },
    { EFI_TIMEOUT, L"EFI_TIMEOUT:The timeout time expired." },
    { EFI_NOT_STARTED, L"EFI_NOT_STARTED:The protocol has not been started." },
    { EFI_ALREADY_STARTED, L"EFI_ALREADY_STARTED:The protocol has already been started." },
    { EFI_ABORTED, L"EFI_ABORTED:The operation was aborted." },
    { EFI_ICMP_ERROR, L"EFI_ICMP_ERROR:An ICMP error occurred during the network operation." },
    { EFI_TFTP_ERROR, L"EFI_TFTP_ERROR:A TFTP error occurred during the network operation." },
    { EFI_PROTOCOL_ERROR, L"EFI_PROTOCOL_ERROR:A protocol error occurred during the network operation." },
    { EFI_INCOMPATIBLE_VERSION, L"EFI_INCOMPATIBLE_VERSION:The function encountered an internal version that was incompatible with a version requested by the caller." },
    { EFI_SECURITY_VIOLATION, L"EFI_SECURITY_VIOLATION:The function was not performed due to a security violation." },
    { EFI_CRC_ERROR, L"EFI_CRC_ERROR:A CRC error was detected." },
    { EFI_END_OF_MEDIA, L"EFI_END_OF_MEDIA:Beginning or end of media was reached" },
    { EFI_END_OF_FILE, L"EFI_END_OF_FILE:The end of the file was reached." },
    { EFI_INVALID_LANGUAGE, L"EFI_INVALID_LANGUAGE:The language specified was invalid." },
    { EFI_COMPROMISED_DATA, L"EFI_COMPROMISED_DATA:The security status of the data is unknown or compromised and the data must be updated or replaced to restore a valid security status." },
    { EFI_IP_ADDRESS_CONFLICT, L"EFI_IP_ADDRESS_CONFLICT:There is an address conflict address allocation" },
    { EFI_HTTP_ERROR, L"EFI_HTTP_ERROR:A HTTP error occurred during the network operation." },
};

static warn_msg_tbl warn_tbl[] = {
    { EFI_SUCCESS, L"EFI_SUCCESS:The operation completed successfully." },
    { EFI_WARN_UNKNOWN_GLYPH, L"EFI_WARN_UNKNOWN_GLYPH:The string contained one or more characters that the device could not render and were skipped." },
    { EFI_WARN_DELETE_FAILURE, L"EFI_WARN_DELETE_FAILURE:The handle was closed, but the file was not deleted." },
    { EFI_WARN_WRITE_FAILURE, L"EFI_WARN_WRITE_FAILURE:The handle was closed, but the data to the file was not flushed properly." },
    { EFI_WARN_BUFFER_TOO_SMALL, L"EFI_WARN_BUFFER_TOO_SMALL:The resulting buffer was too small, and the data was truncated to the buffer size." },
    { EFI_WARN_STALE_DATA, L"EFI_WARN_STALE_DATA:The data has not been updated within the timeframe set by local policy for this type of data." },
    { EFI_WARN_FILE_SYSTEM, L"EFI_WARN_FILE_SYSTEM:The resulting buffer contains UEFI-compliant file system." },
};

CHAR16* print_status_msg(EFI_STATUS status)
{
    if (EFI_ERROR(status)){

        UINTN num = sizeof(err_tbl) / sizeof(err_tbl[0]);

        for (UINTN i = 0; num > i; ++i){

            if (status == err_tbl[i].value){

                return err_tbl[i].description;
            }
        }
    }else{

        UINTN num = sizeof(warn_tbl) / sizeof(warn_tbl[0]);

        for (UINTN i = 0; num > i; ++i){

            if (status == warn_tbl[i].value){

                return warn_tbl[i].description;
            }
        }
    }

    return L"Unknown status value.";
}

