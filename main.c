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
#include "actor/actor_motion.h"
#include "actor/actor_control.h"
#include "actor/actor_animation.h"

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
	ActorAnimation player_animation = actorAnimation_create(&player);
	actorAnimation_attach(&player, &player_animation);

	//scenery
	Scenery room = scenery_create(0, "rom:/room.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		controllerData_getInputs(&control);
		time_setData(&timing);

		actorControl_setData(&player, &control, timing.frame_time_s, camera.angle_around_barycenter, camera.offset_angle);
		actor_setState(&player, player.state);
		actor_setMotion(&player, timing.frame_time_s);
		actor_setAnimation(&player, &player_animation, timing.frame_time_s, &syncPoint);

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

	t3d_skeleton_destroy(&player.armature.main);
	t3d_skeleton_destroy(&player.armature.blend);

	//t3d_anim_destroy(&animIdle);
	//t3d_anim_destroy(&animWalk);

	t3d_model_free(player.model);
	t3d_model_free(room.model);

	t3d_destroy();
	return 0;
}
