#ifndef ACTORSTATES_H
#define ACTORSTATES_H


#define STAND_IDLE 1
#define WALKING 2
#define RUNNING 3
#define SPRINTING 4
#define ROLL 5
#define JUMP 6
#define FALLING 7


// function prototypes

void actorState_setIdle (Actor *actor);

void actorState_setWalking (Actor *actor);
            
void actorState_setRunning (Actor *actor);
            
void actorState_setSprinting (Actor *actor);

void actorState_setJump (Actor *actor);

void actorState_setFalling (Actor *actor);

void actor_setState (Actor *actor, uint8_t state);


void actorState_setIdle(Actor *actor)
{
    if (actor->state == STAND_IDLE) return;
    actor->previous_state = actor->state;
    actor->state = STAND_IDLE;
    actor->locomotion_state = STAND_IDLE;
    
}

void actorState_setWalking(Actor *actor)
{
    if (actor->state == WALKING) return;
    actor->previous_state = actor->state;
    actor->state = WALKING;
    actor->locomotion_state = WALKING;
}

void actorState_setRunning(Actor *actor)
{
    if (actor->state == RUNNING) return;
    actor->previous_state = actor->state;
    actor->state = RUNNING;
    actor->locomotion_state = RUNNING;
}

void actorState_setSprinting(Actor *actor)
{
    if (actor->state == SPRINTING) return;
    actor->previous_state = actor->state;
    actor->state = SPRINTING;
    actor->locomotion_state = SPRINTING;
}

void actorState_setJump(Actor *actor) 
{
    if (actor->state == JUMP) return;
    
    actor->previous_state = actor->state;
    actor->state = JUMP;
    actor->grounded = 0;
    //actor->grounding_height = -FLT_MAX;
}

void actorState_setFalling (Actor *actor)
{
    if (actor->state == FALLING ) return;

    actor->previous_state = actor->state;
    actor->state = FALLING;
    //actor->grounding_height = -FLT_MAX;
}

void actor_setState(Actor *actor, uint8_t state) 
{
    switch(state) {
        
        case STAND_IDLE: {
            actorState_setIdle (actor);
            break;
        }
        case WALKING: {
            actorState_setWalking (actor);
            break;
        }
        case RUNNING: {
            actorState_setRunning (actor);
            break;
        }
        case SPRINTING: {
            actorState_setSprinting (actor);
            break;
        }
        case ROLL: {
            break;
        }
        case JUMP: {
            actorState_setJump (actor);
            break;
        }
        case FALLING: {
            actorState_setFalling (actor);
            break;
        }   
    }
}

#endif