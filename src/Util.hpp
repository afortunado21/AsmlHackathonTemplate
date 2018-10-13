#ifndef __Util__
#define __Util__

#include <utility>
#include <WString.h>

std::pair<int, int> map_single(int x, int y)
{
    return std::pair<int, int>((3 - x / 8) * 8 + x % 8, y);
}

uint32_t read_uint32_t(const String& str, unsigned int *index)
{
    uint32_t result = 0;
    // unsigned int index = 0;
    for (; *index < str.length(); (*index)++)
    {
        if (!isdigit(str[*index]))
        {
            break;
        }
        result = 10 * result + (str[*index] - '0');
    }
    *index += 1;
    return result;
}

#endif // __Util__
