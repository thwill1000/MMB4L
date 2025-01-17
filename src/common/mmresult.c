/*-*****************************************************************************

MMBasic for Linux (MMB4L)

mmresult.c

Copyright 2021-2024 Geoff Graham, Peter Mather and Thomas Hugo Williams.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holders nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

4. The name MMBasic be used when referring to the interpreter in any
   documentation and promotional material and the original copyright message
   be displayed  on the console at startup (additional copyright messages may
   be added).

5. All advertising materials mentioning features or use of this software must
   display the following acknowledgement: This product includes software
   developed by Geoff Graham, Peter Mather and Thomas Hugo Williams.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "cstring.h"
#include "mmresult.h"
#include "../Configuration.h"

#include <stdarg.h>
#include <string.h>

const char *audio_last_error();
const char *events_last_error();
const char *gamepad_last_error();
const char *graphics_last_error();

MmResult mmresult_last_code = kOk;
char mmresult_last_msg[STRINGSIZE] = { 0 };

static void formatAudioApiError() {
    snprintf(mmresult_last_msg, STRINGSIZE, "Audio error: %s", audio_last_error());
}

static void formatEventsApiError() {
    snprintf(mmresult_last_msg, STRINGSIZE, "Events error: %s", events_last_error());
}

static void formatGamepadApiError() {
    snprintf(mmresult_last_msg, STRINGSIZE, "Gamepad error: %s", gamepad_last_error());
}

static void formatGraphicsApiError() {
    snprintf(mmresult_last_msg, STRINGSIZE, "Graphics error: %s", graphics_last_error());
}

void mmresult_clear() {
   mmresult_last_code = kOk;
   mmresult_last_msg[0] = '\0';
}

MmResult mmresult_ex(MmResult result, const char *format, ...) {
    mmresult_last_code = result;
    va_list args;
    va_start(args, format);
    vsnprintf(mmresult_last_msg, STRINGSIZE, format, args);
    va_end(args);
    return result;
}

const char *mmresult_to_string(MmResult result) {
    if (result && result == mmresult_last_code) {
        // Use cached message.
        return mmresult_last_msg;
    }

    switch (result) {
        case kEventsApiError:
            formatEventsApiError();
            return mmresult_last_msg;
        case kGamepadApiError:
            formatGamepadApiError();
            return mmresult_last_msg;
        case kGraphicsApiError:
            formatGraphicsApiError();
            return mmresult_last_msg;
        case kAudioApiError:
            formatAudioApiError();
            return mmresult_last_msg;
        default:
            return mmresult_to_default_string(result);
    }
}

const char *mmresult_to_default_string(MmResult result) {
    if (result > kOk && result < kError) {
        if (result == kFilenameTooLong) return "Pathname too long";
        return strerror(result);
    }

    switch (result) {
        case kOk:            return "No error";
        case kError:         return "MMBasic error";
        case kInternalFault: return "Internal fault (sorry)";
        case kSyntax:        return "Syntax";
        case kArgumentCount: return "Argument count";
        case kStringTooLong: return "String too long";
        case kInvalidArgument: return "Invalid argument";
        case kInvalidArgumentType: return "Invalid argument type";
        case kInvalidFormat: return "Invalid format";
        case kUnknownOption: return "Unknown option";
        case kInvalidArray:  return "Invalid array value";
        case kInvalidBool:   return "Invalid boolean value";
        case kInvalidEditor: return "Invalid editor";
        case kInvalidEnvironmentVariableName: return "Invalid environment variable name";
        case kInvalidFloat:  return "Invalid float value";
        case kInvalidInt:    return "Invalid integer value";
        case kInvalidString: return "Invalid string value";
        case kInvalidValue:  return "Invalid value";
        case kUnknownSystemCommand: return "Unknown system command";
        case kOverflow:             return "Overflow";
        case kNotPersistent:        return "Invalid for non-persistent option";
        case kNameTooLong:          return "Name too long";
        case kUnimplemented:        return "Unimplemented function";
        case kFunctionNotFound:     return "Function not found";
        case kVariableNotFound:     return "Variable not found";
        case kTooManyFunctions:     return "Too many functions/labels/subroutines";
        case kTooManyVariables:     return "Too many variables";
        case kDuplicateFunction:    return "Function/subroutine already declared";
        case kHashmapFull:          return "Hashmap full";
        case kInvalidName:          return "Invalid name";
        case kInvalidArrayDimensions: return "Dimensions";
        case kFunctionTypeMismatch: return "Not a function";
        case kInvalidCommandLine: return "Invalid command line arguments";
        case kStringLength:         return "String length";
        case kTooManyDefines:             return "Too many #DEFINE directives";
        case kOutOfMemory:                return "Not enough memory";
        case kLineTooLong:                return "Line too long";
        case kProgramTooLong:             return "Program too long";
        case kUnterminatedComment:        return "Unterminated multiline comment";
        case kNoCommentToTerminate:       return "No comment to terminate";
        case kContainerEmpty:             return "Container empty";
        case kContainerFull:              return "Container full";
        case kEventsApiError:             return "SDL events error";
        case kFlashFileTooBig:            return "File too large to load into flash";
        case kFlashInvalidIndex:          return "Invalid flash slot";
        case kFlashModuleNotInitialised:  return "Flash simulation module not initialised";
        case kGamepadApiError:            return "SDL gamepad error";
        case kGraphicsApiError:           return "SDL graphics error";
        case kGraphicsInvalidColour:      return "Invalid colour";
        case kGraphicsInvalidColourDepth: return "Invalid colour depth";
        case kGraphicsInvalidId:          return "Invalid graphics surface ID";
        case kGraphicsInvalidReadSurface: return "Invalid graphics read surface";
        case kGraphicsInvalidSprite:      return "Invalid sprite";
        case kGraphicsInvalidSpriteIdZero: return "Invalid sprite 0";
        case kGraphicsInvalidSurface:     return "Invalid graphics surface";
        case kGraphicsInvalidVertices:    return "Invalid number of vertices";
        case kGraphicsInvalidWindow:      return "Invalid window";
        case kGraphicsInvalidWriteSurface: return "Invalid graphics write surface";
        case kGraphicsLoadBitmapFailed:   return "Bitmap could not be loaded";
        case kGraphicsReadAndWriteSurfaceSame: return "Graphics read and write surfaces are the same";
        case kGraphicsSurfaceAlreadyExists: return "Graphics surface already exists";
        case kGraphicsSurfaceSizeMismatch: return "Graphics surface size mismatch";
        case kGraphicsSurfaceTooLarge:    return "Graphics surface too large";
        case kGraphicsTooManySprites:     return "Maximum of 64 sprites";
        case kImageTooLarge:              return "Image too large";
        case kImageInvalidFormat:         return "Invalid image format";
        case kInvalidFont:                return "Invalid font";
        case kInvalidFontScaling:         return "Invalid font scaling; should be 1-15";
        case kUnexpectedText:             return "Unexpected text";
        case kUnknownDevice:              return "Unknown device/platform";
        case kUnsupportedOnCurrentDevice: return "Unsupported on current device/platform";
        case kUnsupportedParameterOnCurrentDevice: return "Unsupported parameter on current device/platform";
        case kInvalidMode:                return "Invalid graphics mode for current device";
        case kInvalidFlag:                return "Invalid flag";
        case kCannotBlitCloseWindow:      return "Use GRAPHICS DESTROY to close windows";
        case kMissingType:                return "Missing type";
        case kTypeSpecifiedTwice:         return "Type specified twice";
        case kInvalidFunctionDefinition:  return "Invalid function definition";
        case kInvalidSubDefinition:       return "Invalid subroutine definition";
        case kMissingCloseBracket:        return "Missing close bracket";
        case kMissingOpenBracket:         return "Missing open bracket";
        case kUnexpectedCloseBracket:     return "Unexpected close bracket";
        case kInvalidArrayParameter:      return "Invalid array parameter";
        case kTooManyParameters:          return "Too many parameters";
        case kInvalidInterruptSignature:  return "Invalid interrupt signature";
        case kGamepadNotFound:            return "Gamepad not found";
        case kGamepadInvalidId:           return "Invalid gamepad ID";
        case kGamepadNotOpen:             return "Gamepad not open";
        case kGamepadUnknownFunction:     return "Unknown gamepad function";
        case kAudioApiError:              return "SDL audio error";
        case kAudioFlacInitialisationFailed: return "FLAC file initialisation failed";
        case kAudioInUse:                 return "Sound output in use";
        case kAudioInvalidFrequency:      return "Valid is 0Hz to 20KHz";
        case kAudioInvalidSampleRate:     return "Invalid audio sample rate";
        case kAudioMp3InitialisationFailed: return "MP3 file initialisation failed";
        case kAudioNoModFile:             return "No MOD file playing";
        case kAudioNoMoreTracks:          return "No more audio tracks";
        case kAudioNothingToPause:        return "Nothing to pause";
        case kAudioNothingToPlay:         return "Nothing to play";
        case kAudioNothingToResume:       return "Nothing to resume";
        case kAudioSampleRateMismatch:    return "WAV file has different sample rate to MOD file";
        case kAudioWavInitialisationFailed: return "WAV file initialisation failed";
        case kGpioInvalidPin:             return "Invalid pin";
        case kGpioInvalidPulseWidth:      return "Invalid pulse width";
        case kGpioPinIsNotAnOutput:       return "Pin is not an output";
        case kKeyboardUnknownKey:         return "Unknown key pressed";
        case kNotParsed:                  return "Not parsed";
        case kFileInvalidExtension:       return "Invalid file extension; must begin with '.'";
        case kFileInvalidFileNumber:      return "Invalid file number";
        case kFileAlreadyOpen:            return "File or device already open";
        case kFileNotOpen:                return "File or device not open";
        case kFileInvalidSeekPosition:    return "Invalid seek position";
        case kNotEnoughData:              return "Not enough data";
        case kPreprocessorReplaceFailed:  return "Preprocessor string replacement failed";
        case kSpriteInactive:             return "Sprite not showing";
        case kSpritesAreHidden:           return "Sprites are hidden";
        case kSpritesNotHidden:           return "Sprites are not hidden";
        case kStackElementNotFound:       return "Stack element not found";
        case kStackIndexOutOfBounds:      return "Stack index out of bounds";
        default:                          return "Unknown result code";
    }
}
