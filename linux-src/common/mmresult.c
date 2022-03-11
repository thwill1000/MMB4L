#include <string.h>

#include "mmresult.h"

const char *mmresult_to_string(MmResult result) {
    if (result > kOk && result < kError) {
        return strerror(result);
    }

    switch (result) {
        case kOk:            return "No error";
        case kError:         return "MMBasic error";
        case kInternalFault: return "Internal fault (sorry)";
        case kSyntax:        return "Syntax";
        case kStringTooLong: return "String too long";
        case kInvalidFormat: return "Invalid format";
        case kUnknownOption: return "Unknown option";
        case kInvalidBool:   return "Invalid boolean value";
        case kInvalidFloat:  return "Invalid float value";
        case kInvalidInt:    return "Invalid int value";
        case kInvalidString: return "Invalid string value";
        case kInvalidValue:  return "Invalid value";
        case kUnknownSystemCommand: return "Unknown system command";
        default:             return "Unknown result code";
    }
}
