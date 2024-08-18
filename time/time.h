#ifndef TIME_H
#define TIME_H


// structures

typedef struct
{

    float current_frame_s;
    float current_frame_ms;
    float last_frame_ms;
    float frame_time_s;
    float frame_rate;

} TimeData;


// functions prototypes

void time_init(TimeData *time);
void time_setData(TimeData *time);


// functions implementations

/* sets time data values to zero */
void time_init(TimeData *time)
{
    time->current_frame_s = 0.0f;
    time->current_frame_ms = 0.0f;
    time->last_frame_ms = 0.0f;
    time->frame_time_s = 0.0f;
    time->frame_rate = 0.0f;
}


/* sets timing data */
void time_setData(TimeData *time)
{
    time->current_frame_ms = get_ticks_ms();

    time->current_frame_s = time->current_frame_ms * 0.001f;

    time->frame_time_s = (time->current_frame_ms - time->last_frame_ms) * 0.001f;

    time->frame_rate = 1 / time->frame_time_s;

    time->last_frame_ms = time->current_frame_ms;
}

#endif