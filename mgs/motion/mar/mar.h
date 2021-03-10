#pragma once
#include <inttypes.h>

typedef struct MarData {
	uint32_t mtcmOffset;
	uint32_t mtcmSize;
} MarData;

typedef struct MarHeader {
	uint32_t magic;
	uint32_t maxJoint;
	uint32_t numMotion;
	uint32_t mtcmOffset;
} MarHeader;