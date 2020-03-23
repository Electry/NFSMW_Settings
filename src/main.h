#ifndef _MAIN_H_
#define _MAIN_H_
#include <stdbool.h>

#define TITLEID_LEN 9

typedef enum {
    YAW_SPRING_ORIGINAL,
    YAW_SPRING_FLEX,
    YAW_SPRING_STIFF,
    YAW_SPRING_STIFFER,
    YAW_SPRING_OFF
} yaw_spring_t;

typedef struct {
    unsigned int resolution_width;
    unsigned int resolution_height;

    unsigned int dq_max_pending_frames;

    float camera_aspect_ratio;

    float camera_external_fov_min;
    float camera_external_fov_max;

    bool camera_external_yaw_spring_enabled;
    yaw_spring_t camera_external_yaw_spring_first;
    yaw_spring_t camera_external_yaw_spring_second;

    float camera_external_distance;
    float camera_external_height;

    float lights_traffic_corona_intensity;
    float lights_racecar_corona_intensity;

    float groundcover_draw_distance;
    unsigned int groundcover_max_instances;

    bool lod_vehicles_force_static;
    unsigned int lod_vehicles_value;
    unsigned int lod_vehicles_shadows_value;
} config_t;

extern tai_module_info_t g_tai_info;
extern SceKernelModuleInfo g_sce_info;

extern config_t g_config;

extern uint32_t g_inject_num;
extern SceUID g_inject[];

void parse_config();
void patch_game(uint32_t offsets[]);

uint8_t *encode_t1_mov(uint8_t reg, uint8_t value);
uint8_t *encode_t2_mov(bool setflags, uint8_t reg, uint32_t value);
uint8_t *encode_t3_mov(uint8_t reg, uint16_t value);
uint8_t *encode_t1_movt(uint8_t reg, uint16_t value);
uint8_t *encode_mov32(uint8_t reg, uint32_t value);
uint8_t *encode_t2_vmov_f32(uint8_t reg, uint32_t value);

#endif
