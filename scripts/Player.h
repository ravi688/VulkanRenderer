
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <engine/engine.h>

#define Player_TYPE_ID CREATE_COMPONENT_ID(1)

enum player_state_t
{
	PLAYER_STATE_RUNNING,
	PLAYER_STATE_WALKING, 
	PLAYER_STATE_JUMPNG,
	PLAYER_STATE_MOVING,
	PLAYER_STATE_CROUCHING
};

typedef struct Player
{
	DERIVE_FROM_COMPONENT;
	float move_speed; 
	float run_speed;
	float walk_speed;
	float jump_height;
	float crouch_angle;
	float crouch_speed;

} Player;


component_declare_callbacks(Player);

#endif/*__PLAYER_H__*/