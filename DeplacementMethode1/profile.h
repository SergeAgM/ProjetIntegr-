#ifndef PROFILE_H
#define	PROFILE_H

float Travel_Time(float distance, float a, float v, int type);

float profile_position(float a, float v, float Tt, float time_elapsed, float distance);

float profile_angulaire(float a, float Tt, float time_elapsed, float angle);

#endif