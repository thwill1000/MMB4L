#include "../common/mmtime.h"
#include "../common/version.h"

void fun_time(void) {
    targ = T_STR;
    sret = GetTempStrMemory();
    mmtime_time_string(mmtime_now_ns(), sret);
    CtoM(sret);
}