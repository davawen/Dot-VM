#pragma once

constexpr unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    char c = *str;

    while((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}