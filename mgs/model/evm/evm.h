#pragma once
#include <inttypes.h>

struct vec3Float {
	float x;
	float y;
	float z;
};

struct vec4Float {
	float x;
	float y;
	float z;
	float w;
};

struct vec2Short {
	int16_t x;
	int16_t y;
};

struct vec3Short {
	int16_t x;
	int16_t y;
	int16_t z;
};

struct vec4Short {
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t w;
};

struct EvmHeader {
	uint32_t numUnknown;
	uint32_t numBones;
	vec3Float max;
	vec3Float min;
	uint32_t flag;
	uint32_t strcode;
	int32_t numVertexDefinition;
	uint32_t vertexDefinitionOffset;
};

struct EvmBone {
	uint32_t pad;
	int32_t parent;
	vec3Float parentPos;
	vec3Float worldPos;
	vec4Float max;
	vec4Float min;
};

struct EvmVertexDefinition {
	uint32_t flag;
	uint32_t textureStrcode;
	uint32_t texture2Strcode;
	uint32_t texture3Strcode;
	uint32_t numVertex;
	uint32_t numSkin;
	uint8_t skinningTable[8];
	uint32_t vertexOffset;
	uint32_t normalOffset;
	uint32_t uvOffset;
	uint32_t uv2Offset;
	uint32_t uv3Offset;
	uint32_t weightOffset;
	uint32_t pad;
	uint32_t pad2;
};