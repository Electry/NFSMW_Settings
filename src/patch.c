#include <vitasdk.h>
#include <taihen.h>

#include "main.h"

static void inject_data(int segidx, uint32_t offset, const void *data, size_t size) {
    g_inject[g_inject_num] = taiInjectData(g_tai_info.modid, segidx, offset, data, size);
    g_inject_num++;
}

static float yaw_spring_val(yaw_spring_t ys) {
    switch (ys) {
        default:
        case YAW_SPRING_FLEX:    return 0.0f;
        case YAW_SPRING_STIFF:   return 0.125f;
        case YAW_SPRING_STIFFER: return 0.25f;
        case YAW_SPRING_OFF:     return 1.0f;
    }

    return 0.0f;
}

void patch_game(uint32_t offsets[]) {
    //
    // Resolution
    //
    inject_data(0, offsets[0],     encode_t2_mov(true, 1, g_config.resolution_width), 4);
    inject_data(0, offsets[0] + 8, encode_t2_mov(true, 1, g_config.resolution_height), 4);

    //
    // VMEM mapping
    //
    if ((g_config.resolution_width * g_config.resolution_height) > (640 * 368)) {
        inject_data(1, offsets[1], (uint32_t[]){0x4F56000 - 5 * 1024 * 1024}, 4);
        inject_data(1, offsets[2], (uint32_t[]){0x34F1000 - 5 * 1024 * 1024}, 4);
        inject_data(1, offsets[3], (uint32_t[]){0x32F3000 - 5 * 1024 * 1024}, 4);
        inject_data(1, offsets[4], (uint32_t[]){0x10AA000 + 5 * 1024 * 1024}, 4);
    }

    //
    // DQ
    //
    inject_data(0, offsets[5], encode_t1_mov(0, g_config.dq_max_pending_frames), 2);

    //
    // Camera
    //
    inject_data(0, offsets[6], (uint8_t[]){0x00, 0xBF, 0x00, 0xBF}, 4);
    inject_data(1, offsets[7], (float []){g_config.camera_aspect_ratio}, 4);

    //
    // Camera External
    //
    inject_data(0, offsets[8],      encode_mov32(3, *(uint32_t *)&g_config.camera_external_fov_min), 8);
    inject_data(0, offsets[8] + 8,  (uint8_t[]){0x00, 0xEE, 0x10, 0x3A}, 4); // VMOV.F32 S0, R3
    inject_data(0, offsets[8] + 16, encode_mov32(3, *(uint32_t *)&g_config.camera_external_fov_max), 8);
    inject_data(0, offsets[8] + 24, (uint8_t[]){0x00, 0xEE, 0x10, 0x3A}, 4); // VMOV.F32 S0, R3

    if (!g_config.camera_external_yaw_spring_enabled) {
        inject_data(0, offsets[9], (uint8_t[]){0x00, 0xBF, 0x00, 0xBF}, 4);
    } else {
        if (g_config.camera_external_yaw_spring_first != YAW_SPRING_ORIGINAL)
            inject_data(0, offsets[10], encode_t2_vmov_f32(2, yaw_spring_val(g_config.camera_external_yaw_spring_first)), 4);
         if (g_config.camera_external_yaw_spring_second != YAW_SPRING_ORIGINAL)
            inject_data(0, offsets[11], encode_t2_vmov_f32(4, yaw_spring_val(g_config.camera_external_yaw_spring_second)), 4);
    }

    // Patches unused code in CameraDebugFly
    inject_data(0, offsets[12], encode_mov32(0, *(uint32_t *)&g_config.camera_external_distance), 8);
    inject_data(0, offsets[12] + 8, (uint8_t[]){
        0x02, 0xEE, 0x10, 0x0A,
        0x60, 0xEE, 0x02, 0x2A, // S0 mul
        0xC6, 0xED, 0x0C, 0x2A, // S0 store R6+0x30
        0x61, 0xEE, 0x02, 0x2A, // S2 mul
        0xC6, 0xED, 0x0E, 0x2A  // S2 store R6+0x38
    }, 20);
    inject_data(0, offsets[12] + 28, encode_mov32(0, *(uint32_t *)&g_config.camera_external_height), 8);
    inject_data(0, offsets[12] + 36, (uint8_t[]){
        0x02, 0xEE, 0x10, 0x0A,
        0x60, 0xEE, 0x82, 0x2A, // S1 mul
        0xC6, 0xED, 0x0D, 0x2A, // S1 store R6+0x34
        0x01, 0xF0, 0x0F, 0xBC  // B #0x1822
    }, 16);

    // Jump to code above
    inject_data(0, offsets[13], (uint8_t[]){0xFE, 0xF7, 0xD7, 0xBB, 0x00, 0xBF, 0x00, 0xBF}, 8);

    //
    // Lights
    //
    inject_data(0, offsets[14], encode_mov32(0, *(uint32_t *)&g_config.lights_traffic_corona_intensity), 8);
    inject_data(0, offsets[15], encode_t2_vmov_f32(1, *(uint32_t *)&g_config.lights_racecar_corona_intensity), 4);

    //
    // Groundcover
    //
    inject_data(1, offsets[16],     (uint32_t[]){g_config.groundcover_max_instances}, 4);
    inject_data(1, offsets[16] + 4, (float[]){g_config.groundcover_draw_distance}, 4);

    //
    // LODs
    //
    inject_data(1, offsets[17], (uint32_t[]){
        g_config.lod_vehicles_force_static,
        g_config.lod_vehicles_value,
        g_config.lod_vehicles_shadows_value}, 12);
}
