//
// Created by sajit on 17/07/2024.
//

#ifndef BOOSTEXAMPLES_ERROR_H
#define BOOSTEXAMPLES_ERROR_H

#include "common.h"

 inline void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << " : " << ec.message() << std::endl;
}


#endif //BOOSTEXAMPLES_ERROR_H
