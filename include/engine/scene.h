#ifndef __SCENE_H__
#define __SCENE_H__

#include <engine/object.h>
#include <buffer.h>

typedef struct scene_t
{
	char name[24];
	LIST(object_t*) objects;
} scene_t; 


scene_t* scene_new(char* name);
void scene_clean(scene_t* scene);
void scene_destroy(scene_t* scene);

void scene_add_object(scene_t* scene, object_t* object);
void scene_remove_object(scene_t* scene, object_t* object);

/*Obsolete*/
object_t* scene_create_object(scene_t* scene, char* name);

/*To be implemented*/
scene_t* scene_merge(scene_t* scene1, scene_t* scene2);


#define ensure_scene_is_not_NULL(return_type)\
if(scene == NULL)\
{\
	log_err("scene_t is NULL");\
	return return_type;\
}

#endif/*__SCENE_H__*/