#ifndef CAMERA_H
#define CAMERA_H

/* camera.H
here are all the structures and functions prototypes that involve the setting up of the camera */


// structures

typedef enum {
	INTRO,
	MENU,
	PAUSE,
    ORBITAL,
	AIMING,

} CameraState;

typedef struct {

	float orbitational_acceleration_rate;
	Vector2 orbitational_max_velocity;

	float zoom_acceleration_rate;
	float zoom_deceleration_rate;
	float zoom_max_speed;

	float target_zoom;
	float target_zoom_aim;

	float offset_acceleration_rate;
	float offset_deceleration_rate;
	float offset_max_speed;

	float target_offset;
	float target_offset_aim;
	
	float max_pitch;

} CameraSettings;


typedef struct {
	
	Mtx modeling;

	u16 normal;
	Mtx projection;
	Mtx position_mtx;

	Vector3 position;
	float offset_height;
	
	float distance_from_barycenter; // the barycenter is choosen and it's the center of the orbitational movement
	float angle_around_barycenter;
	float pitch;

	float horizontal_barycenter_distance;
	float vertical_barycenter_distance;

	Vector3 target; // target as in the place at which the camera must aim
	float target_distance; 
	float horizontal_target_distance;
	float vertical_target_distance;

	Vector2 orbitational_acceleration;
	Vector2 orbitational_velocity;
	Vector2 orbitational_target_velocity; // target as in intended velocity

	float offset_angle;

	float offset_acceleration;
	float offset_speed;
	int offset_direction;

	float zoom_acceleration;
	float zoom_speed;
	int zoom_direction;

	CameraSettings settings;
	
} Camera;

typedef struct{

	uint8_t ambient_color[4];
	uint8_t directional_color[4];
	T3DVec3 direction;
	
} LightData;


// functions prototypes

void camera_getOrbitalPosition(Camera *camera, Vector3 barycenter, float frame_time);
void camera_set_dl(Camera *camera);
void light_set_dl(LightData *light);


/*camera_getOrbitalPosition
calculates the camera position given the input controlled variables*/

void camera_getOrbitalPosition(Camera *camera, Vector3 barycenter, float frame_time)
{
	camera->orbitational_velocity.x += camera->orbitational_acceleration.x * frame_time;
    camera->orbitational_velocity.y += camera->orbitational_acceleration.y * frame_time;
	camera->zoom_speed += camera->zoom_acceleration * frame_time;
	camera->offset_speed += camera->offset_acceleration * frame_time;

	if (fabs(camera->orbitational_velocity.x) < 1 && fabs(camera->orbitational_velocity.y) < 1 && fabs(camera->zoom_speed) < 1 && fabs(camera->offset_speed) < 1)
	{
		camera->orbitational_velocity.x = 0;
		camera->orbitational_velocity.y = 0;
		camera->zoom_speed = 0;
		camera->offset_speed = 0;

	}

    camera->pitch += camera->orbitational_velocity.x * frame_time;
	camera->angle_around_barycenter += camera->orbitational_velocity.y * frame_time;
	
	camera->distance_from_barycenter += camera->zoom_direction * camera->zoom_speed * frame_time;
	camera->offset_angle += camera->offset_direction * camera->offset_speed * frame_time;

	if (camera->angle_around_barycenter > 360) camera->angle_around_barycenter -= 360;
    if (camera->angle_around_barycenter < 0) camera->angle_around_barycenter  += 360;

    if (camera->pitch > camera->settings.max_pitch) camera->pitch = camera->settings.max_pitch;
    if (camera->pitch < -camera->settings.max_pitch) camera->pitch = -camera->settings.max_pitch;

    camera->horizontal_barycenter_distance = camera->distance_from_barycenter * cosf(rad(camera->pitch));
	camera->vertical_barycenter_distance = camera->distance_from_barycenter * sinf(rad(camera->pitch));

	camera-> horizontal_target_distance = camera->target_distance * cosf(rad(camera->pitch));
	camera->vertical_target_distance = camera->target_distance * sinf(rad(camera->pitch + 180));

    camera->position.x = barycenter.x - (camera->horizontal_barycenter_distance * sinf(rad(camera->angle_around_barycenter - camera->offset_angle)));
    camera->position.y = barycenter.y - (camera->horizontal_barycenter_distance * cosf(rad(camera->angle_around_barycenter - camera->offset_angle)));
    camera->position.z = barycenter.z + camera->offset_height + camera->vertical_barycenter_distance;
	
	camera->target.x = barycenter.x - camera-> horizontal_target_distance * sinf(rad(camera->angle_around_barycenter + 180));
	camera->target.y = barycenter.y - camera-> horizontal_target_distance * cosf(rad(camera->angle_around_barycenter + 180));
	camera->target.z = barycenter.z + camera->offset_height + camera->vertical_target_distance;
}


/* set camera
handles the system functions that enters the camera position and rotation values */

void camera_set_dl(Camera *camera, T3DViewport viewport)
{

	t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.0f), 10.0f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

}


/* set light
temporary function until i learn how the lights work  */
void light_set_dl(LightData *light)
{
    int i;

    //color
    for (i=0; i<3; i++) {

        // Ambient color
        light->amb.l.col[i]  = 130;
        light->amb.l.colc[i] = 130;

        //directional light color
        light->dir.l.col[i]  = 255;
        light->dir.l.colc[i] = 255;
    }

    // Direction
    light->dir.l.dir[0] = -127 * sinf(light->rotation.x * 0.0174532925);
    light->dir.l.dir[1] =  127 * sinf(light->rotation.z * 0.0174532925) * cosf(light->rotation.x * 0.0174532925);
    light->dir.l.dir[2] =  127 * cosf(light->rotation.z * 0.0174532925) * cosf(light->rotation.x * 0.0174532925);

    // Send the light struct to the RSP
    gSPNumLights(glistp++, NUMLIGHTS_1);
    gSPLight(glistp++, &light->dir, 1);
    gSPLight(glistp++, &light->amb, 2);
    gDPPipeSync(glistp++); 
}

#endif
