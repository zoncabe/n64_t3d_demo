#ifndef ACTOR_MOVEMENT_H
#define ACTOR_MOVEMENT_H


#define ACTOR_GRAVITY -2000


// function prototypes

void actorMotion_setHorizontalAcceleration (Actor *actor, float target_speed, float acceleration_rate);

void actorMotion_setHorizontalInertiaAcceleration (Actor *actor, float target_speed, float acceleration_rate);

void actorMotion_setStopingAcceleration (Actor *actor);

void actorMotiion_setJumpAcceleration (Actor *actor, float target_speed, float acceleration_rate);

void actorMotion_integrate (Actor *actor, float frame_time);


void actorMotion_setHorizontalAcceleration(Actor *actor, float target_speed, float acceleration_rate)
{
    actor->target_velocity.x = target_speed * sinf(rad(actor->target_yaw));
    actor->target_velocity.y = target_speed * -cosf(rad(actor->target_yaw));

    actor->body.acceleration.x = acceleration_rate * (actor->target_velocity.x - actor->body.velocity.x);
    actor->body.acceleration.y = acceleration_rate * (actor->target_velocity.y - actor->body.velocity.y);
}

void actorMotion_setHorizontalInertiaAcceleration(Actor *actor, float target_speed, float acceleration_rate)
{
    actor->target_velocity.x = target_speed * sinf(rad(actor->body.rotation.z));
    actor->target_velocity.y = target_speed * -cosf(rad(actor->body.rotation.z));

    actor->body.acceleration.x = acceleration_rate * (actor->target_velocity.x - actor->body.velocity.x);
    actor->body.acceleration.y = acceleration_rate * (actor->target_velocity.y - actor->body.velocity.y);
}

void actorMotion_setStopingAcceleration (Actor *actor)
{
    actor->body.acceleration.x = actor->settings.idle_acceleration_rate * (0 - actor->body.velocity.x);
    actor->body.acceleration.y = actor->settings.idle_acceleration_rate * (0 - actor->body.velocity.y);
}

void actorMotiion_setJumpAcceleration(Actor *actor, float target_speed, float acceleration_rate)
{
    actor->body.acceleration.z = acceleration_rate * (target_speed - actor->body.velocity.z);
}


void actorMotion_integrate (Actor *actor, float frame_time)
{

    if (actor->body.acceleration.x != 0 || actor->body.acceleration.y != 0 || actor->body.acceleration.z != 0){
        vector3_addScaledVector(&actor->body.velocity, &actor->body.acceleration, frame_time);
    }

	if (fabs(actor->body.velocity.x) < 10 && fabs(actor->body.velocity.y) < 10){
		actor->body.velocity.x = 0;
		actor->body.velocity.y = 0;
	}

    if (actor->body.velocity.x != 0 || actor->body.velocity.y != 0 || actor->body.velocity.z != 0) 
        vector3_addScaledVector(&actor->body.position, &actor->body.velocity, frame_time);

    if (actor->body.velocity.x != 0 || actor->body.velocity.y != 0) {

		actor->body.rotation.z = deg(atan2(-actor->body.velocity.x, -actor->body.velocity.y));

        Vector2 horizontal_velocity = {actor->body.velocity.x, actor->body.velocity.y};
        actor->horizontal_speed = vector2_magnitude(&horizontal_velocity);       
	}
}

void actorMotion_setIdle(Actor *actor)
{
    actorMotion_setStopingAcceleration(actor);
    
    if  (fabs(actor->body.velocity.x) < 1 && fabs(actor->body.velocity.y) < 1){

        vector3_init(&actor->body.velocity);
        actor->horizontal_speed = 0;    
        actor->target_yaw = actor->body.rotation.z;
    }
}

void actorMotion_setWalking(Actor *actor)
{
    //actorMotion_setHorizontalAcceleration (actor, actor->horizontal_target_speed, actor->settings.walk_acceleration_rate);
    actorMotion_setHorizontalAcceleration (actor, actor->settings.walk_target_speed, actor->settings.walk_acceleration_rate);
}

void actorMotion_setRunning(Actor *actor)
{    
    //actorMotion_setHorizontalAcceleration (actor, actor->horizontal_target_speed, actor->settings.run_acceleration_rate);
    actorMotion_setHorizontalAcceleration (actor, actor->settings.run_target_speed, actor->settings.run_acceleration_rate);
}

void actorMotion_setSprinting(Actor *actor)
{
    //actorMotion_setHorizontalAcceleration (actor, actor->horizontal_target_speed * 1.5f, actor->settings.run_acceleration_rate);
    actorMotion_setHorizontalAcceleration (actor, actor->settings.sprint_target_speed, actor->settings.run_acceleration_rate);
}

void actorMotion_setJump(Actor *actor)
{
          
    if (actor->input.jump_hold && !actor->input.jump_released && actor->input.jump_time_held < actor->settings.jump_timer_max){

        actorMotiion_setJumpAcceleration (actor, actor->settings.jump_target_speed, actor->settings.jump_acceleration_rate);
        actorMotion_setHorizontalAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
    } 
    
    else if (actor->body.velocity.z > 0){

        actorMotion_setHorizontalAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
        actor->body.acceleration.z = ACTOR_GRAVITY;
    }
    
    else {

        actor->state = FALLING;
        actor->input.jump_time_held = 0;
        return;
    }
}

void actorMotion_setFalling(Actor *actor)
{
    actor->grounded = 0;
    actorMotion_setHorizontalAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
    actor->body.acceleration.z = ACTOR_GRAVITY;

    if (actor->body.position.z <= actor->grounding_height) {

        actor->grounded = 1;
        actor->body.acceleration.z = 0;
        actor->body.velocity.z = 0;
        actor->body.position.z = actor->grounding_height;

        actor->state = actor->locomotion_state;

        return;
    }
}

void actor_setMotion(Actor *actor, float frame_time)
{
    if (actor->state == STAND_IDLE) actorMotion_setIdle(actor);
    else if (actor->state == WALKING) actorMotion_setWalking(actor);
    else if (actor->state == RUNNING) actorMotion_setRunning(actor);
    else if (actor->state == SPRINTING) actorMotion_setSprinting(actor);
    else if (actor->state == JUMP) actorMotion_setJump(actor);
    else if (actor->state == FALLING) actorMotion_setFalling(actor);

	actorMotion_integrate(actor, frame_time);
}

#endif