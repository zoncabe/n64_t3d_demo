#ifndef ACTOR_CONTROLS_H
#define ACTOR_CONTROLS_H


// function prototypes

void actorControl_setJump(Actor* actor, ControllerData *control, float frame_time);
void actorControl_setRoll(Actor* actor, ControllerData *control);
void actorControl_moveWithStick(Actor* actor, ControllerData *control, float camera_angle_around, float camera_offset);
void actorControl_setData(Actor* actor, ControllerData *control, float frame_time, float camera_angle_around, float camera_offset);


// function implementations

void actorControl_setJump(Actor* actor, ControllerData *control, float frame_time)
{    
    if (control->pressed.a && actor->state != ROLL && actor->state != JUMP && actor->state != FALLING) {
        
        actor->input.jump_hold = true;
        actor->input.jump_released = false;
        actor_setState(actor, JUMP);
    }

    else if (control->held.a && actor->state == JUMP) {
        
        actor->input.jump_time_held += frame_time;
    }
    else {

        actor->input.jump_released = true;
        actor->input.jump_hold = false;    
    }
}

void actorControl_setRoll(Actor* actor, ControllerData *control)
{

}

void actorControl_moveWithStick(Actor *actor, ControllerData *control, float camera_angle_around, float camera_offset)
{
    int deadzone = 6;
    float stick_magnitude = 0; 

    if (fabs(control->input.stick_x) >= deadzone || fabs(control->input.stick_y) >= deadzone) {

        Vector2 stick = {control->input.stick_x, control->input.stick_y};
        
        stick_magnitude = vector2_magnitude(&stick);
        actor->horizontal_target_speed = stick_magnitude * 6;
        actor->target_yaw = deg(atan2(control->input.stick_x, -control->input.stick_y) - rad(camera_angle_around - (0.5 * camera_offset)));
    }

    
    if (stick_magnitude == 0 && actor->state != ROLL && actor->state != JUMP && actor->state != FALLING){
        actor_setState(actor, STAND_IDLE);
    }

    else if (stick_magnitude > 0 && stick_magnitude <= 80 && actor->state != ROLL && actor->state != JUMP && actor->state != FALLING){
        actor_setState(actor, WALKING);
    }

    else if (control->held.r && stick_magnitude > 80 && actor->state != ROLL && actor->state != JUMP && actor->state != FALLING){
        actor_setState(actor, SPRINTING);
    }

    else if (stick_magnitude > 80 && actor->state != ROLL && actor->state != JUMP && actor->state != FALLING){
        actor_setState(actor, RUNNING);
    }
}


void actorControl_setData(Actor* actor, ControllerData *control, float frame_time, float camera_angle_around, float camera_offset)
{    
   
    actorControl_setJump(actor, control, frame_time);

    actorControl_moveWithStick(actor, control, camera_angle_around, camera_offset);

}



#endif