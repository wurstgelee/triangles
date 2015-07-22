/* Copyright (c) 2014-2015, StealthCoin/StealthSend Developers */
/* Copyright (c) 2014-2015, Triangles Developers */
/* See LICENSE for licensing information */

/**
 * \file anonymize.h
 * \brief Headers for anonymize.cpp
 **/

#ifndef TOR_RAZOR_H
#define TOR_RAZOR_H

#ifdef __cplusplus
extern "C" {
#endif

    char const* anonymize_tor_data_directory(
    );

    char const* anonymize_service_directory(
    );

    int check_interrupted(
    );

    void set_initialized(
    );

    void wait_initialized(
    );

#ifdef __cplusplus
}
#endif

#endif

