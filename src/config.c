#include <vitasdk.h>
#include <taihen.h>

#include "main.h"

static void set_default_config() {
    g_config.resolution_width  = 736; // default = 640
    g_config.resolution_height = 448; // default = 368

    g_config.dq_max_pending_frames = 1; // default = 2

    g_config.camera_aspect_ratio = 1.764706f;

    g_config.camera_external_fov_min = 75.0;
    g_config.camera_external_fov_max = 90.0;

    g_config.camera_external_yaw_spring_enabled = true;
    g_config.camera_external_yaw_spring_first = YAW_SPRING_ORIGINAL;
    g_config.camera_external_yaw_spring_second = YAW_SPRING_STIFF; // default = ORIGINAL

    g_config.camera_external_distance = 1.25f; // default = 1.0
    g_config.camera_external_height = 1.5f;

    g_config.lights_traffic_corona_intensity = 0.1f; // default = 0.7
    g_config.lights_racecar_corona_intensity = 2.0f; // default = 1.0

    g_config.groundcover_draw_distance = 200.0f; // default = 125.0
    g_config.groundcover_max_instances = 20000;

    g_config.lod_vehicles_force_static = true; // default = false
    g_config.lod_vehicles_value = 0;
    g_config.lod_vehicles_shadows_value = 0;
}

void parse_config() {
    set_default_config();

    // TODO: ... read user cfg
}
