#ifndef IGSYAJL_VERSION_H_
#define IGSYAJL_VERSION_H_

#include "yajl_common.h"

#define IGSYAJL_MAJOR 2
#define IGSYAJL_MINOR 1
#define IGSYAJL_MICRO 1

#define IGSYAJL_VERSION ((IGSYAJL_MAJOR * 10000) + (IGSYAJL_MINOR * 100) + IGSYAJL_MICRO)

#ifdef __cplusplus
extern "C" {
#endif

extern int IGSYAJL_API igsyajl_version(void);

#ifdef __cplusplus
}
#endif

#endif /* IGSYAJL_VERSION_H_ */

