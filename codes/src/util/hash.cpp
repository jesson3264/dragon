//
// Created by Jesson on 2025/5/17.
//

#include "hash.h"
namespace util
{
    uint32_t times33_hash(const char *str)
    {
        uint32_t hash = 5381; // 初始哈希种子
        int c;

        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }

        return hash;
    }
}