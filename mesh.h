#pragma once
#include <vector>
#include "mat.h"
#include "mgs/model/kms/kms.h"
#include "mgs/model/evm/evm.h"
#include "noesis/plugin/pluginshare.h"

inline
void bindFace(int16_t* vertices, uint16_t fc, std::vector<uint16_t>& faceBuffer) {
    if (vertices[3] & 0x8000) return;

    uint16_t fa = fc < 3 ? 0 : fc - 2;
    uint16_t fb = fc < 2 ? 0 : fc - 1;

    faceBuffer.push_back(fa);
    faceBuffer.push_back(fb);
    faceBuffer.push_back(fc);
    faceBuffer.push_back(fa);
    faceBuffer.push_back(fc);
    faceBuffer.push_back(fb);
}

inline
void bindVertex(int16_t* vertices, std::vector<float>& vertexBuffer, float scale) {
    vertexBuffer.push_back(vertices[0] * scale);
    vertexBuffer.push_back(vertices[1] * scale);
    vertexBuffer.push_back(vertices[2] * scale);
}

inline
void bindNormal(int16_t* normals, std::vector<float>& normalBuffer) {
    float scale = 1.0f / 4096.0f;
    normalBuffer.push_back(normals[0] * scale);
    normalBuffer.push_back(normals[1] * scale);
    normalBuffer.push_back(normals[2] * scale);
}

inline
void bindUV(int16_t* uvs, std::vector<float>& uvBuffer) {
    float scale = 1.0f / 4096.0f;
    uvBuffer.push_back(uvs[0] * scale);
    uvBuffer.push_back(uvs[1] * scale);
}

inline
void bindSkin(uint8_t* skin, uint8_t* skinningTable, int32_t numSkin, std::vector<float>& weightBuffer, std::vector<uint8_t>& boneBuffer) {
    for (int i = 0; i < numSkin; i++) {
        weightBuffer.push_back(skin[i] / 128.0f);
    }

    for (int i = 4; i < 4 + numSkin; i++) {
        int idx = skin[i] >> 2;
        boneBuffer.push_back(skinningTable[idx]);
    }
}

inline
void bindKMSSkin(int16_t* vertices, KmsMesh* mesh, int meshNum, std::vector<float>& weightBuffer, std::vector<uint8_t>& boneBuffer) {
    float weight = vertices[3] / 4096.0f;
    weightBuffer.push_back(weight);
    boneBuffer.push_back(meshNum);

    if (mesh->parent > -1) {
        weightBuffer.push_back(1.0f - weight);
        boneBuffer.push_back(mesh->parent);
    }
}

inline
void setOrigin(KmsMesh* mesh, modelBone_t* noeBone, noeRAPI_t* rapi) {
    modelMatrix_t t = g_identityMatrix;
    g_mfn->Math_VecCopy(noeBone->mat.o, t.o);
    rapi->rpgSetTransform(&t);
}

inline
void bindMesh(EvmVertexDefinition* vDef, BYTE* fileBuffer, noeRAPI_t* rapi, CArrayList<noesisMaterial_t*>& matList, CArrayList<noesisTex_t*>& texList) {
    std::vector<float>  vertexBuffer, normalBuffer, uvBuffer, uvBuffer2, uvBuffer3, weightBuffer;
    std::vector<uint16_t> faceBuffer;
    std::vector<uint8_t> boneBuffer;

    int16_t* uvIndex     = (int16_t*)&fileBuffer[vDef->uvOffset];
    int16_t* uv2Index    = (int16_t*)&fileBuffer[vDef->uv2Offset];
    int16_t* uv3Index    = (int16_t*)&fileBuffer[vDef->uv3Offset];
    uint8_t* weightIndex = (uint8_t*)&fileBuffer[vDef->weightOffset];
    int16_t* vertexIndex = (int16_t*)&fileBuffer[vDef->vertexOffset];
    int16_t* normalIndex = (int16_t*)&fileBuffer[vDef->normalOffset];

    float scale = 1.0f / 16.0f;

    for (int i = 0; i < vDef->numVertex; i++) {

        if (vDef->vertexOffset) bindVertex(vertexIndex, vertexBuffer, scale);
        if (vDef->vertexOffset) bindFace(vertexIndex, i, faceBuffer);
        if (vDef->normalOffset) bindNormal(normalIndex, normalBuffer);

        if (vDef->uvOffset)  bindUV(uvIndex, uvBuffer);
        if (vDef->uv2Offset) bindUV(uv2Index, uvBuffer2);
        if (vDef->uv3Offset) bindUV(uv3Index, uvBuffer3);

        if (vDef->weightOffset) bindSkin(weightIndex, vDef->skinningTable, vDef->numSkin, weightBuffer, boneBuffer);

        uvIndex += 4;
        uv2Index += 4;
        uv3Index += 4;
        weightIndex += 8;
        vertexIndex += 4;
        normalIndex += 4;
    }

    uint32_t textures[3] = { vDef->textureStrcode, vDef->texture2Strcode, vDef->texture3Strcode };
    bindMat(textures, fileBuffer, rapi, matList, texList);

    if (!uvBuffer.empty())     rapi->rpgBindUV1BufferSafe(&uvBuffer[0], RPGEODATA_FLOAT, 8, uvBuffer.size() * 4);
    if (!uvBuffer2.empty())    rapi->rpgBindUV2BufferSafe(&uvBuffer2[0], RPGEODATA_FLOAT, 8, uvBuffer2.size() * 4);
    if (!uvBuffer3.empty())    rapi->rpgBindUVXBufferSafe(&uvBuffer3[0], RPGEODATA_FLOAT, 8, 2, 2, uvBuffer3.size() * 4);
    if (!normalBuffer.empty()) rapi->rpgBindNormalBufferSafe(&normalBuffer[0], RPGEODATA_FLOAT, 12, normalBuffer.size() * 4);
    if (!vertexBuffer.empty()) rapi->rpgBindPositionBufferSafe(&vertexBuffer[0], RPGEODATA_FLOAT, 12, vertexBuffer.size() * 4);

    if (!weightBuffer.empty()) rapi->rpgBindBoneIndexBuffer(&boneBuffer[0], RPGEODATA_UBYTE, vDef->numSkin, vDef->numSkin);
    if (!weightBuffer.empty()) rapi->rpgBindBoneWeightBuffer(&weightBuffer[0], RPGEODATA_FLOAT, vDef->numSkin * 4, vDef->numSkin);

    if (!faceBuffer.empty())   rapi->rpgCommitTriangles(&faceBuffer[0], RPGEODATA_USHORT, faceBuffer.size(), RPGEO_TRIANGLE, 0);
    rapi->rpgClearBufferBinds();
}


inline
void bindKMSMesh(KmsMesh* mesh, modelBone_t* noeBone, int meshNum, BYTE* fileBuffer, noeRAPI_t* rapi, CArrayList<noesisMaterial_t*>& matList, CArrayList<noesisTex_t*>& texList) {

    setOrigin(mesh, noeBone, rapi);

    KmsVertexDefinition* vDef = (KmsVertexDefinition*)&fileBuffer[mesh->vertexDefinitionOffset];  

    float scale = 1.0f;

    for (int i = 0; i < mesh->numVertexDefinition; i++) {
        std::vector<float>  vertexBuffer, normalBuffer, uvBuffer, uvBuffer2, uvBuffer3, weightBuffer;
        std::vector<uint16_t> faceBuffer;
        std::vector<uint8_t> boneBuffer;

        int16_t* uvIndex     = (int16_t*)&fileBuffer[vDef[i].uvOffset];
        int16_t* uv2Index    = (int16_t*)&fileBuffer[vDef[i].uv2Offset];
        int16_t* uv3Index    = (int16_t*)&fileBuffer[vDef[i].uv3Offset];
        int16_t* vertexIndex = (int16_t*)&fileBuffer[vDef[i].vertexOffset];
        int16_t* normalIndex = (int16_t*)&fileBuffer[vDef[i].normalOffset];

        for (int j = 0; j < vDef[i].numVertex; j++) {

            bindVertex(vertexIndex, vertexBuffer, scale);
            bindNormal(normalIndex, normalBuffer);
            bindFace(normalIndex, j, faceBuffer);

            if (vDef->uvOffset)  bindUV(uvIndex,  uvBuffer);
            if (vDef->uv2Offset) bindUV(uv2Index, uvBuffer2);
            if (vDef->uv3Offset) bindUV(uv3Index, uvBuffer3);

            bindKMSSkin(vertexIndex, mesh, meshNum, weightBuffer, boneBuffer);

            uvIndex  += 2;
            uv2Index += 2;
            uv3Index += 2;
            vertexIndex += 4;
            normalIndex += 4;
        }

        uint32_t textures[3] = { vDef[i].textureStrcode, vDef[i].texture2Strcode, vDef[i].texture3Strcode };
        bindMat(textures, fileBuffer, rapi, matList, texList);

        int numWeight = 1 + (mesh->parent > -1);

        if (!uvBuffer.empty())  rapi->rpgBindUV1BufferSafe(&uvBuffer[0], RPGEODATA_FLOAT, 8, uvBuffer.size() * 4);
        if (!uvBuffer2.empty()) rapi->rpgBindUV2BufferSafe(&uvBuffer2[0], RPGEODATA_FLOAT, 8, uvBuffer2.size() * 4);
        if (!uvBuffer3.empty()) rapi->rpgBindUVXBufferSafe(&uvBuffer3[0], RPGEODATA_FLOAT, 8, 2, 2, uvBuffer3.size() * 4);

        rapi->rpgBindBoneIndexBuffer(&boneBuffer[0], RPGEODATA_UBYTE, numWeight, numWeight);
        rapi->rpgBindBoneWeightBuffer(&weightBuffer[0], RPGEODATA_FLOAT, numWeight * 4, numWeight);
        rapi->rpgBindNormalBufferSafe(&normalBuffer[0], RPGEODATA_FLOAT, 12, normalBuffer.size() * 4);
        rapi->rpgBindPositionBufferSafe(&vertexBuffer[0], RPGEODATA_FLOAT, 12, vertexBuffer.size() * 4);
        rapi->rpgCommitTriangles(&faceBuffer[0], RPGEODATA_USHORT, faceBuffer.size(), RPGEO_TRIANGLE, 0);

        rapi->rpgClearBufferBinds();
    }
}