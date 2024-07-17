//
// Created by sajith.nandasena on 16.07.2024.
//

#ifndef IGNORE_UNUSED_H
#define IGNORE_UNUSED_H

template<typename... Ts>
constexpr void ignore_unused(Ts &&...)
{
}

template<typename... Ts>
constexpr void ignore_unused()
{
}
#endif //IGNORE_UNUSED_H
