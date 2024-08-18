#ifndef ACTORSTATES_H
#define ACTORSTATES_H


#include "actor_movement.h"

#define ACTOR_GRAVITY -6000


// function prototypes


void set_idle (Actor *actor);

void set_walking (Actor *actor);
            
void set_running (Actor *actor);
            
void set_sprinting (Actor *actor);

void set_jump (Actor *actor);

void set_falling (Actor *actor);


void actor_setState (Actor *actor, ActorState state);



void set_idle(Actor *actor)
{
    if (actor->state == STAND_IDLE) return;

    actor_setStopingAcceleration(actor);
    
    if  (fabs(actor->body.velocity.x) < 1 && fabs(actor->body.velocity.y) < 1){

        vector3_init(&actor->body.velocity);
        actor->horizontal_speed = 0;    
        actor->target_yaw = actor->body.rotation.z;
        actor->state = STAND_IDLE;
        actor->previous_state = STAND_IDLE;
    }
    
}


void set_walking(Actor *actor)
{
    actor_setAcceleration (actor, actor->settings.walk_target_speed, actor->settings.walk_acceleration_rate);
    if (actor->state == WALKING) return;
    actor->state = WALKING;
    actor->previous_state = WALKING;
}


void set_running(Actor *actor)
{
    actor_setAcceleration (actor, actor->settings.run_target_speed, actor->settings.run_acceleration_rate);
    if (actor->state == RUNNING) return;
    actor->state = RUNNING;
    actor->previous_state = RUNNING;
}


void set_sprinting(Actor *actor)
{
    actor_setAcceleration (actor, actor->settings.sprint_target_speed, actor->settings.run_acceleration_rate);
    if (actor->state == SPRINTING) return;
    actor->state = SPRINTING;
    actor->previous_state = SPRINTING;
}


void set_jump(Actor *actor) 
{       
    if (actor->input.jump_hold && !actor->input.jump_released && actor->input.jump_time_held < actor->settings.jump_timer_max){

        actor_setJumpAcceleration (actor, actor->settings.jump_target_speed, actor->settings.jump_acceleration_rate);
        actor_setAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
    } 
    
    else if (actor->body.velocity.z > 0){

        actor_setAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
        actor->body.acceleration.z = ACTOR_GRAVITY;
    }
    
    else {

        actor_setState(actor, FALLING);
        actor->input.jump_time_held = 0;
        return;
    }

    if (actor->state == JUMP) return;
    
    actor->state = JUMP;
    actor->grounded = 0;
    //actor->grounding_height = -FLT_MAX;
}


void set_falling (Actor *actor)
{   
    actor->grounded = 0;
    actor_setAcceleration (actor, actor->horizontal_speed, actor->settings.aerial_control_rate);
    actor->body.acceleration.z = ACTOR_GRAVITY;

    if (actor->body.position.z <= actor->grounding_height) {

        actor->grounded = 1;
        actor->body.acceleration.z = 0;
        actor->body.velocity.z = 0;
        actor->body.position.z = actor->grounding_height;

        actor_setState (actor, actor->previous_state);
        //actor->previous_state = FALLING;

        return;
    }


    if (actor->state == FALLING ) return;

    actor->state = FALLING;
    //actor->grounding_height = -FLT_MAX;
}



void actor_setState(Actor *actor, ActorState state) 
{
    switch(state) {

        case EMPTY: {
            break;
        } 
        case STAND_IDLE: {
            set_idle (actor);
            break;
        }
        case WALKING: {
            set_walking (actor);
            break;
        }
        case RUNNING: {
            set_running (actor);
            break;
        }
        case SPRINTING: {
            set_sprinting (actor);
            break;
        }
        case ROLL: {
            break;
        }
        case JUMP: {
            set_jump (actor);
            break;
        }
        case FALLING: {
            set_falling (actor);
            break;
        }   
    }
}

#endif