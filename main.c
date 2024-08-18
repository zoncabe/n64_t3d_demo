#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include "config/screen.h"
#include "control/controls.h"
#include "time/time.h"

#include "physics/physics.h"

#include "camera/camera.h"
#include "camera/camera_states.h"
#include "camera/camera_control.h"

#include "actor/actor.h"
#include "actor/actor_states.h"
#include "actor/actor_control.h"

#include "scene/scenery.h"


int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);

	Screen screen;
	screen_init(&screen);

	rdpq_init();
	joypad_init();

	TimeData timing;
	time_init(&timing);
  	rspq_syncpoint_t syncPoint = 0;

	ControllerData control;

	t3d_init((T3DInitParams){});

	//camera
	Camera camera = camera_create();

	//light
	LightData light = light_create();

	//actor
	Actor player = actor_create(0, "rom:/pipo.t3dm");

	T3DAnim animIdle = t3d_anim_create(player.model, "idle-breathing-left");
	t3d_anim_attach(&animIdle, &player.armature);

	T3DAnim animWalk = t3d_anim_create(player.model, "running-35-left");
	t3d_anim_attach(&animWalk, &player.blending_armature);

	float animBlend = 0.0f;
	
	actor_setState(&player, STAND_IDLE);

	//scenery
	Scenery room = scenery_create(0, "rom:/room.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		controllerData_getInputs(&control);
		time_setData(&timing);

				// use blend based on speed for smooth transitions
				animBlend = player.horizontal_speed * 0.003;
				if(animBlend > 1.0f)animBlend = 1.0f;

				// Update the animation and modify the skeleton, this will however NOT recalculate the matrices
				t3d_anim_update(&animIdle, timing.frame_time_s);
				t3d_anim_set_speed(&animWalk, animBlend + 0.15f);
				t3d_anim_update(&animWalk, timing.frame_time_s);

				// We now blend the walk animation with the idle/attack one
				t3d_skeleton_blend(&player.armature, &player.armature, &player.blending_armature, animBlend);

				if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

				// Now recalc. the matrices, this will cause any model referencing them to use the new pose
				t3d_skeleton_update(&player.armature);

		
		actorControl_setMotion(&player, &control, timing.frame_time_s, camera.angle_around_barycenter, camera.offset_angle);
		actor_integrate(&player, timing.frame_time_s);
		actor_setState(&player, player.state);
		actor_set(&player);

		cameraControl_setOrbitalMovement(&camera, &control);
		camera_getOrbitalPosition(&camera, player.body.position, timing.frame_time_s);
		camera_set(&camera, &screen);

		scenery_set(&room);

		// ======== Draw ======== //
		
		screen_clear(&screen);
	
		light_set(&light);
    
		t3d_matrix_push_pos(1);

		actor_draw(&player);

		scenery_draw(&room);
   
   		t3d_matrix_pop(1);

		syncPoint = rspq_syncpoint_new();

		rdpq_detach_show();
	}

	t3d_skeleton_destroy(&player.armature);
	t3d_skeleton_destroy(&player.blending_armature);

	t3d_anim_destroy(&animIdle);
	t3d_anim_destroy(&animWalk);

	//t3d_model_free(player.model);
	//t3d_model_free(room.model);

	t3d_destroy();
	return 0;
}
