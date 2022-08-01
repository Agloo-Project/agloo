/*
 * Copyright (c) 2006-2020, DreamGrow Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-07     luhuadong    the first version
 */

#ifndef __AG_DEF_H__
#define __AG_DEF_H__

/* include agconfig header to import configuration */
#include "agconfig.h"

/* Version information */
#define AG_VERSION                      1L              /* major version number */
#define AG_SUBVERSION                   0L              /* minor version number */
#define AG_REVISION                     0L              /* revise version number */

/* Lidar software version */
#define AG_LIDAR_VERSION                ((AG_VERSION * 10000) + \
                                         (AG_SUBVERSION * 100) + AG_REVISION)

/* Error code definitions */
#define AG_EOK                          0               /* There is no error */
#define AG_ERROR                        1               /* A generic error happens */
#define AG_ETIMEOUT                     2               /* Timed out */
#define AG_EFULL                        3               /* The resource is full */
#define AG_EEMPTY                       4               /* The resource is empty */
#define AG_ENOMEM                       5               /* No memory */
#define AG_ENOSYS                       6               /* No system */
#define AG_EBUSY                        7               /* Busy */
#define AG_EIO                          8               /* IO error */
#define AG_EINTR                        9               /* Interrupted system call */
#define AG_EINVAL                       10              /* Invalid argument */
#define AG_ENOFILE                      11              /* No file */
#define AG_ENOPERM                      12              /* NO permission */

/* LiDAR state definitions */
#define AG_SYSTEM_POWEROFF              0x00                /* Power-off status */
#define AG_SYSTEM_INIT                  0x01                /* Initialized status */
#define AG_SYSTEM_NORMAL                0x02                /* Normal status */
#define AG_SYSTEM_ERROR                 0x03                /* Error status */
#define AG_SYSTEM_DEGRADE               0x04                /* Degrade status */
#define AG_SYSTEM_SLEEP                 0x05                /* Sleep status */

/* Basic data type definitions */
#ifdef AG_USING_LIBC
#include <stdint.h>
#include <stddef.h>
typedef int8_t                          ag_int8_t;      /*  8bit integer type */
typedef int16_t                         ag_int16_t;     /* 16bit integer type */
typedef int32_t                         ag_int32_t;     /* 32bit integer type */
typedef uint8_t                         ag_uint8_t;     /*  8bit unsigned integer type */
typedef uint16_t                        ag_uint16_t;    /* 16bit unsigned integer type */
typedef uint32_t                        ag_uint32_t;    /* 32bit unsigned integer type */
typedef int64_t                         ag_int64_t;     /* 64bit integer type */
typedef uint64_t                        ag_uint64_t;    /* 64bit unsigned integer type */
typedef size_t                          ag_size_t;      /* Type for size number */

#else
typedef signed   char                   ag_int8_t;      /*  8bit integer type */
typedef signed   short                  ag_int16_t;     /* 16bit integer type */
typedef signed   int                    ag_int32_t;     /* 32bit integer type */
typedef unsigned char                   ag_uint8_t;     /*  8bit unsigned integer type */
typedef unsigned short                  ag_uint16_t;    /* 16bit unsigned integer type */
typedef unsigned int                    ag_uint32_t;    /* 32bit unsigned integer type */

#ifdef ARCH_CPU_64BIT
typedef signed long                     ag_int64_t;     /* 64bit integer type */
typedef unsigned long                   ag_uint64_t;    /* 64bit unsigned integer type */
typedef unsigned long                   ag_size_t;      /* Type for size number */
#else
typedef signed long long                ag_int64_t;     /* 64bit integer type */
typedef unsigned long long              ag_uint64_t;    /* 64bit unsigned integer type */
typedef unsigned int                    ag_size_t;      /* Type for size number */
#endif /* ARCH_CPU_64BIT */
#endif /* AG_USING_LIBC */

typedef int                             ag_bool_t;      /* boolean type */
typedef long                            ag_base_t;      /* Nbit CPU related date type */
typedef unsigned long                   ag_ubase_t;     /* Nbit unsigned CPU related data type */

typedef ag_base_t                       ag_err_t;       /* Type for error number */
typedef ag_uint32_t                     ag_time_t;      /* Type for time stamp */
typedef ag_base_t                       ag_flag_t;      /* Type for flags */
typedef ag_ubase_t                      ag_dev_t;       /* Type for device */
typedef ag_base_t                       ag_off_t;       /* Type for offset */

/* boolean type definitions */
#define AG_TRUE                         1               /* boolean true  */
#define AG_FALSE                        0               /* boolean fails */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AG_DEF_H__ */
