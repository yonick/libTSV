#include "SharedBuf.h"

SharedBuf<char> Null =
    { 1, 0, (SharedBuf<char>::Bytes*)Null.storage, 0 };