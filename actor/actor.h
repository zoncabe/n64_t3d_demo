#ifndef ACTOR_H
#define ACTOR_H


// structures

typedef enum {

	EMPTY,
    STAND_IDLE,
    WALKING,
    RUNNING,
	SPRINTING,
	ROLL,
	JUMP,
	FALLING,

} ActorState;


typedef struct {

	float idle_acceleration_rate;
	float walk_acceleration_rate;
	float run_acceleration_rate;
	float roll_acceleration_rate;
	float roll_acceleration_grip_rate;
	float jump_acceleration_rate;
	float aerial_control_rate;

	float walk_target_speed;
	float run_target_speed;
	float sprint_target_speed;
	float idle_to_roll_target_speed;
	float idle_to_roll_grip_target_speed;
	float walk_to_roll_target_speed;
	float run_to_roll_target_speed;
	float sprint_to_roll_target_speed;
	float jump_target_speed;
	
	float jump_timer_max;

}ActorSettings;


typedef struct {

	float stick_magnitude;
	float stick_x;
	float stick_y;
	float jump_time_held;
	bool jump_hold;
	bool jump_released;

}Actorinput;


typedef struct {

	uint32_t id;
	rspq_block_t *dl;
	T3DMat4FP *modelMat;
	T3DModel *model;
	T3DSkeleton armature;
	T3DSkeleton blending_armature;
	
	Vector3 scale;
	RigidBody body;
	float target_yaw;
	Vector3 target_velocity;
	float horizontal_speed;
	bool grounded;
	float grounding_height;

	ActorState previous_state;
	ActorState state;

	ActorSettings settings;
	Actorinput input;

} Actor;


// function prototypes

Actor actor_create(uint32_t id, const char *model_path);
void actor_set(Actor *actor);
void actor_draw(Actor *actor);
void actor_delete(Actor *actor);


// function implemenations

Actor actor_create(uint32_t id, const char *model_path)
{
    Actor actor = {

        .id = id,
		.model = t3d_model_load(model_path),
        .modelMat = malloc_uncached(sizeof(T3DMat4FP)),

        .scale = {1.0f, 1.0f, 1.0f},
        
		.body = {
            .position = {0.0f, 0.0f, 0.0f},
            .velocity = {0.0f, 0.0f, 0.0f},
            .rotation = {0.0f, 0.0f, 0.0f},
        },
        
		.grounding_height = 0.0f,
        
		.settings = {
			.idle_acceleration_rate = 9,
			.walk_acceleration_rate = 4,
			.run_acceleration_rate = 6,
			.roll_acceleration_rate = 20,
			.roll_acceleration_grip_rate = 2,
			.jump_acceleration_rate = 50,
			.aerial_control_rate = 2.5,
			.walk_target_speed = 200,
			.run_target_speed = 650,
			.sprint_target_speed = 900,
			.idle_to_roll_target_speed = 300,
			.idle_to_roll_grip_target_speed = 50,
			.walk_to_roll_target_speed = 400,
			.run_to_roll_target_speed = 780,
			.sprint_to_roll_target_speed = 980,
			.jump_target_speed = 800, 
			.jump_timer_max = 0.13
        },
    };

	actor.armature = t3d_skeleton_create(actor.model);
	actor.blending_armature = t3d_skeleton_clone(&actor.armature, false);

    rspq_block_begin();
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw_skinned(actor.model, &actor.armature);
    actor.dl = rspq_block_end();

    t3d_mat4fp_identity(actor.modelMat);

    return actor;
}

void actor_set(Actor *actor)
{	
	t3d_mat4fp_from_srt_euler(actor->modelMat,
		(float[3]){actor->scale.x, actor->scale.y, actor->scale.z},
		(float[3]){rad(actor->body.rotation.x), rad(actor->body.rotation.y), rad(actor->body.rotation.z)},
		(float[3]){actor->body.position.x, actor->body.position.y, actor->body.position.z}
	);
}

void actor_draw(Actor *actor) 
{	
	t3d_matrix_set(actor->modelMat, true);
	rspq_block_run(actor->dl);
}

void actor_delete(Actor *actor) 
{
	free_uncached(actor->modelMat);
}


#endif