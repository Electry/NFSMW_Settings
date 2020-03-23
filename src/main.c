#include <vitasdk.h>
#include <taihen.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"

#define MAX_INJECT_NUM 30

tai_module_info_t g_tai_info;
SceKernelModuleInfo g_sce_info;

config_t g_config;

// patches
uint32_t g_inject_num = 0;
SceUID g_inject[MAX_INJECT_NUM] = {-1};

bool is_supported_game(const char *titleid, const uint32_t nid, uint32_t offsets[]) {
    if (strncasecmp(titleid, "PCSB00183", TITLEID_LEN) &&
        strncasecmp(titleid, "PCSE00089", TITLEID_LEN) &&
        strncasecmp(titleid, "PCSG00106", TITLEID_LEN))
        return false;

    switch (nid) {
        case 0xEE6F8557: // v1.00

            // Resolution
            offsets[0] = 0x2F8CDC;

            // VMEM
            offsets[1] = 0x1C6C;
            offsets[2] = 0x1CB4;
            offsets[3] = 0x280C;
            offsets[4] = 0x331C;

            // DQ
            offsets[5] = 0x54DA7E;

            // Camera
            offsets[6] = 0x36C30;
            offsets[7] = 0x431C;

            // Camera External
            offsets[8] = 0x10687A; // fov
            offsets[9] = 0x1038F4; // call update_yaw_spring
            offsets[10] = 0x10368C; // update_yaw_spring VLDR
            offsets[11] = 0x1038E0;
            offsets[12] = 0xFED82; // CameraDebugFly
            offsets[13] = 0x1005D0;

            // Lights
            offsets[14] = 0x149BBA; // traffic corona
            offsets[15] = 0xE9982; // racecar corona

            // Groundcover
            offsets[16] = 0x6CA8;

            // LODs
            offsets[17] = 0xF53E4;

            break;

        case 0x36DC8D31: // v1.01
            offsets[0] = 0x2F8D60;
            offsets[1] = 0x1C6C;
            offsets[2] = 0x1CB4;
            offsets[3] = 0x280C;
            offsets[4] = 0x331C;
            offsets[5] = 0x54DCAA;
            offsets[6] = 0x36C28;
            offsets[7] = 0x431C;
            offsets[8] = 0x106862;
            offsets[9] = 0x1038DC;
            offsets[10] = 0x103674;
            offsets[11] = 0x1038C8;
            offsets[12] = 0xFED6A;
            offsets[13] = 0x1005B8;
            offsets[14] = 0x149BA2;
            offsets[15] = 0xE996A;
            offsets[16] = 0x6CA8;
            offsets[17] = 0xF53E4;
            break;

        default:
            return false;
    }

    return true;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
    char titleid[16];
    g_tai_info.size = sizeof(tai_module_info_t);
    g_sce_info.size = sizeof(SceKernelModuleInfo);

    sceAppMgrAppParamGetString(0, 12, titleid, 16);
    taiGetModuleInfo(TAI_MAIN_MODULE, &g_tai_info);
    sceKernelGetModuleInfo(g_tai_info.modid, &g_sce_info);

    uint32_t offsets[18];
    if (is_supported_game(titleid, g_tai_info.module_nid, offsets)) {
        parse_config();
        patch_game(offsets);
    }

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
    // Release patches
    while (g_inject_num-- > 0) {
        if (g_inject[g_inject_num] >= 0)
            taiInjectRelease(g_inject[g_inject_num]);
    }

    return SCE_KERNEL_STOP_SUCCESS;
}
