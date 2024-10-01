#ifndef ACTOR_ANIMATION_H
#define ACTOR_ANIMATION_H


// structures

typedef struct {

	T3DAnim idle_left;
	T3DAnim transition_left;
	T3DAnim walking_left;
	T3DAnim running_left;
	T3DAnim sprinting_left;
	T3DAnim roll_left;
	T3DAnim jump_left;
	T3DAnim falling_left;
	T3DAnim landing_left;

} AnimationSet;


typedef struct {

	uint8_t current;
	uint8_t previous;

	AnimationSet main;
	AnimationSet blend;

	uint8_t change_delay;
	float blending_ratio;
	float speed_rate;
	bool synced;

} ActorAnimation;


// function implemenations

void animationSet_init(const Actor* actor, AnimationSet* set)
{
	set->idle_left = t3d_anim_create(actor->model, "idle-breathing-left");
	set->transition_left = t3d_anim_create(actor->model, "transition-left");
	set->walking_left = t3d_anim_create(actor->model, "walking-left");          // 31 frames
	set->running_left = t3d_anim_create(actor->model, "running-10-left");       // 23 frames
	set->sprinting_left = t3d_anim_create(actor->model, "running-60-left");		// 18 frames
}

// hardcoded from frame count
// to do; an automated way
float walk_to_run_ratio =  0.34782608696f;			// 31 / 23 -1
float run_to_walk_ratio =  0.741935483871f;			// 23 / 31
float run_to_sprint_ratio =  1.27777777778f;		// 23 / 18	
float sprint_to_run_ratio =  0.782608695652f;		// 18 / 23
////////////////////////////

ActorAnimation actorAnimation_create(const Actor* actor)
{
	ActorAnimation animation;
	animationSet_init(actor, &animation.main);
	animationSet_init(actor, &animation.blend);
	animation.current = 1;
	animation.previous = 1;
	animation.change_delay = 0;
	animation.blending_ratio = 0.0f;
	animation.speed_rate = 0.0f;
	animation.synced = false;
	return animation;
}

void actorAnimation_attach(const Actor* actor, ActorAnimation* animation)
{
	// attach main
	t3d_anim_attach(&animation->main.idle_left, &actor->armature.main);
	t3d_anim_attach(&animation->main.walking_left, &actor->armature.main);
	t3d_anim_attach(&animation->main.running_left, &actor->armature.main);
	t3d_anim_attach(&animation->main.sprinting_left, &actor->armature.main);

	// attach blend
	t3d_anim_attach(&animation->blend.transition_left, &actor->armature.blend);
	t3d_anim_attach(&animation->blend.walking_left, &actor->armature.blend);
	t3d_anim_attach(&animation->blend.running_left, &actor->armature.blend);
	t3d_anim_attach(&animation->blend.sprinting_left, &actor->armature.blend);
}

	//t3d_anim_set_speed(&actor_animation.blend.walking_left, actor->armature.blending_ratio + 0.15f);		

void actorAnimation_setStandIdle(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	if(animation->previous == WALKING || animation->current == WALKING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.walk_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

		t3d_anim_update(&animation->main.idle_left, frame_time);
		t3d_anim_update(&animation->blend.walking_left, frame_time);
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}
	else if (animation->previous == RUNNING || animation->current == RUNNING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.run_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

		t3d_anim_update(&animation->main.idle_left, frame_time);
		t3d_anim_update(&animation->blend.running_left, frame_time);
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}
	else if (animation->previous == SPRINTING || animation->current == SPRINTING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.sprint_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;
		
		t3d_anim_update(&animation->main.idle_left, frame_time);
		t3d_anim_update(&animation->blend.sprinting_left, frame_time);
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}


	else t3d_anim_update(&animation->main.idle_left, frame_time);
}

void actorAnimation_setWalking(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{	
	if (animation->previous == STAND_IDLE || animation->current == STAND_IDLE) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.walk_target_speed;
		if (animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;
		if (animation->current == STAND_IDLE) t3d_anim_set_time(&animation->blend.walking_left, 0.0f);

		t3d_anim_update(&animation->main.idle_left, frame_time);

		t3d_anim_set_speed(&animation->blend.walking_left, animation->blending_ratio);
		t3d_anim_update(&animation->blend.walking_left, frame_time);


		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}


	else if (animation->previous == RUNNING || animation->current == RUNNING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.run_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

		t3d_anim_update(&animation->main.running_left, frame_time);

		t3d_anim_set_speed(&animation->blend.walking_left, animation->blending_ratio);
		t3d_anim_update(&animation->blend.walking_left, frame_time);
	}


	else if (animation->previous == SPRINTING || animation->current == SPRINTING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.sprint_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

		t3d_anim_update(&animation->main.sprinting_left, frame_time);

		t3d_anim_set_speed(&animation->blend.walking_left, animation->blending_ratio);
		t3d_anim_update(&animation->blend.walking_left, frame_time);
	}

	else
		t3d_anim_update(&animation->main.walking_left, frame_time);
}

void actorAnimation_setRunning(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	if (animation->previous == STAND_IDLE || animation->current == STAND_IDLE) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.run_target_speed;
		if (animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;
		if (animation->current == STAND_IDLE) t3d_anim_set_time(&animation->blend.running_left, 0.0f);

		t3d_anim_set_speed(&animation->blend.running_left, animation->blending_ratio);

		t3d_anim_update(&animation->blend.running_left, frame_time);
		t3d_anim_update(&animation->main.idle_left, frame_time);
		
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}

	else if (animation->previous == WALKING || animation->current == WALKING) {
		
		if (actor->horizontal_speed <= actor->settings.walk_target_speed){

			animation->blending_ratio = actor->horizontal_speed / actor->settings.walk_target_speed;
			if (animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;
				
			t3d_anim_set_speed(&animation->blend.walking_left, animation->blending_ratio);

			t3d_anim_update(&animation->main.idle_left, frame_time);
			t3d_anim_update(&animation->blend.walking_left, frame_time);
			t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
		
		}

		else {
			
			animation->blending_ratio = (actor->horizontal_speed - actor->settings.walk_target_speed) / (actor->settings.run_target_speed - actor->settings.walk_target_speed);
			if (animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

			t3d_anim_set_speed(&animation->main.running_left, (1 - walk_to_run_ratio) + (animation->blending_ratio * walk_to_run_ratio));
			t3d_anim_set_speed(&animation->blend.walking_left, 1 + (animation->blending_ratio * walk_to_run_ratio));

			t3d_anim_set_time(&animation->main.running_left, animation->blend.walking_left.time * run_to_walk_ratio);
			
			t3d_anim_update(&animation->blend.walking_left, frame_time);
			t3d_anim_update(&animation->main.running_left, frame_time);

			t3d_skeleton_blend(&actor->armature.main, &actor->armature.blend, &actor->armature.main, animation->blending_ratio);
		}
		
	}

	else
		t3d_anim_update(&animation->main.running_left, frame_time);
}

void actorAnimation_setSprinting(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{

	animation->blending_ratio = actor->horizontal_speed / actor->settings.sprint_target_speed;
	if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

	t3d_anim_update(&animation->main.running_left, frame_time);
	
	t3d_anim_set_speed(&animation->blend.sprinting_left, animation->blending_ratio);
	t3d_anim_update(&animation->blend.sprinting_left, frame_time);
	
	
	t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
}

void actorAnimation_setCurrent(Actor* actor, ActorAnimation* animation, const uint8_t state, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	switch(animation->current) {

		case STAND_IDLE: {
			actorAnimation_setStandIdle(actor, animation, frame_time, syncpoint);
			break;
		}
		case WALKING: {
			actorAnimation_setWalking(actor, animation, frame_time, syncpoint);
			break;
		}
		case RUNNING: {
			actorAnimation_setRunning(actor, animation, frame_time, syncpoint);
			break;
		}
		case SPRINTING: {
			actorAnimation_setSprinting(actor, animation, frame_time, syncpoint);
			break;
		}
		case ROLL: {
			break;
		}
		case JUMP: {
			break;
		}
		case FALLING: {
			break;
		}
	}
}

void actor_setAnimation(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
    switch(actor->state) {

        case STAND_IDLE: {

			if (animation->current != STAND_IDLE && animation->change_delay < 1) {
				animation->change_delay++;
				actorAnimation_setCurrent(actor, animation, actor->state, frame_time, syncpoint);
			} 
			else {
				actorAnimation_setStandIdle(actor, animation, frame_time, syncpoint);
				if (animation->current != STAND_IDLE) {
					animation->previous = animation->current;
					animation->current = STAND_IDLE;
					animation->change_delay = 0;
				}
			}
            break;
        }

        case WALKING: {
			
			if (animation->current != WALKING && animation->change_delay < 1) {
				animation->change_delay++;
				actorAnimation_setCurrent(actor, animation, actor->state, frame_time, syncpoint);
			}
			else {
				actorAnimation_setWalking(actor, animation, frame_time, syncpoint);
				if (animation->current != WALKING) {
					animation->previous = animation->current;
					animation->current = WALKING;
					animation->change_delay = 0;
				}
			}
            break;
        }

        case RUNNING: {
			
			if (animation->current != RUNNING && animation->change_delay < 1) {
				animation->change_delay++;
				actorAnimation_setCurrent(actor, animation, actor->state, frame_time, syncpoint);
			}
			else {
				actorAnimation_setRunning(actor, animation, frame_time, syncpoint);
				if (animation->current != RUNNING) {
					animation->previous = animation->current;
					animation->current = RUNNING;
					animation->change_delay = 0;
				}
			}
            break;
        }

        case SPRINTING: {

			if (animation->current != SPRINTING && animation->change_delay < 1) {
				animation->change_delay++;
				actorAnimation_setCurrent(actor, animation, actor->state, frame_time, syncpoint);
			}
			else {
				actorAnimation_setSprinting(actor, animation, frame_time, syncpoint);
				if (animation->current != SPRINTING) {
					animation->previous = animation->current;
					animation->current = SPRINTING;
					animation->change_delay = 0;
				}
			}
            break;
        }

        case ROLL: {
            break;
        }
        case JUMP: {
            break;
        }
        case FALLING: {
            break;
        }   
    }
	
	if(syncpoint)rspq_syncpoint_wait(*syncpoint);
	t3d_skeleton_update(&actor->armature.main);
}


#endif