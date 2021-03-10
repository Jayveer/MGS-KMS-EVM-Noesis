#include "tool.h"

const char* g_pPluginName = "mgs_kms_evm";
const char* g_pPluginDesc = "Metal Gear Solid 2 EVM handler by Jayveer.";


bool checkEVM(BYTE* fileBuffer, int bufferLen, noeRAPI_t* rapi) {
    return true;
}

bool checkKMS(BYTE* fileBuffer, int bufferLen, noeRAPI_t* rapi) {
    uint32_t magic = *(uint32_t*)fileBuffer;
    return magic == 0x00C81371 || magic == 0x00B253A3;
}

noesisModel_t* loadEVM(BYTE* fileBuffer, int bufferLen, int& numMdl, noeRAPI_t* rapi) {
    EvmHeader*           header = (EvmHeader*)fileBuffer;
    EvmBone*             bones  = (EvmBone*)&fileBuffer[0x30];
    EvmVertexDefinition* vDef   = (EvmVertexDefinition*)&fileBuffer[header->vertexDefinitionOffset];

    void* ctx = rapi->rpgCreateContext();
    rapi->rpgSetOption(RPGOPT_GEOTWOSIDEDPRV, 1);

    modelBone_t* noeBones = (header->numBones) ? bindBones(bones, header->numBones, rapi) : NULL;

    CArrayList<noesisTex_t*>      texList;
    CArrayList<noesisMaterial_t*> matList;

    for (int i = 0; i < header->numVertexDefinition; i++) {
        bindMesh(&vDef[i], fileBuffer, rapi, matList, texList);
    }

    noesisMatData_t* md = rapi->Noesis_GetMatDataFromLists(matList, texList);
    rapi->rpgSetExData_Materials(md);   

    if (g_mgs2MarPrompt && header->numBones) {
        BYTE* motionFile = openMotion(rapi);
        if (motionFile) loadMotion(rapi, motionFile, noeBones, header->numBones);
    }

    noesisModel_t* mdl = rapi->rpgConstructModel();
    if (mdl) numMdl = 1;

    rapi->rpgDestroyContext(ctx);
    return mdl;
}

noesisModel_t* loadKMS(BYTE* fileBuffer, int bufferLen, int& numMdl, noeRAPI_t* rapi) {
    KmsHeader* header = (KmsHeader*)fileBuffer;
    KmsMesh*   mesh   = (KmsMesh* )&fileBuffer[0x40];

    void* ctx = rapi->rpgCreateContext();
    rapi->rpgSetOption(RPGOPT_GEOTWOSIDEDPRV, 1);

    modelBone_t* noeBones = bindKMSBones(mesh, header->numMesh, rapi);

    CArrayList<noesisTex_t*>      texList;
    CArrayList<noesisMaterial_t*> matList;

    for (int i = 0; i < header->numMesh; i++) {
        bindKMSMesh(&mesh[i], &noeBones[i], i, fileBuffer, rapi, matList, texList);
    }

    noesisMatData_t* md = rapi->Noesis_GetMatDataFromLists(matList, texList);
    rapi->rpgSetExData_Materials(md);

    if (g_mgs2MarPrompt && header->numBones) {
        BYTE* motionFile = openMotion(rapi);
        if (motionFile) loadMotion(rapi, motionFile, noeBones, header->numMesh);
    }

    noesisModel_t* mdl = rapi->rpgConstructModel();
    if (mdl) numMdl = 1;

    rapi->rpgDestroyContext(ctx);
    return mdl;
}

bool NPAPI_InitLocal(void) {
    int fh = g_nfn->NPAPI_Register("Metal Gear Solid 2", ".evm");
    if (fh < 0) return false;

    g_nfn->NPAPI_SetTypeHandler_TypeCheck(fh, checkEVM);
    g_nfn->NPAPI_SetTypeHandler_LoadModel(fh, loadEVM);

    fh = g_nfn->NPAPI_Register("Metal Gear Solid 2", ".kms");
    if (fh < 0) return false;

    g_nfn->NPAPI_SetTypeHandler_TypeCheck(fh, checkKMS);
    g_nfn->NPAPI_SetTypeHandler_LoadModel(fh, loadKMS);

    applyTools();

    return true;
}


void NPAPI_ShutdownLocal(void) {

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}