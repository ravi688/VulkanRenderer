
#include <Camera.h>
#include <calltrace/calltrace.h>

void OnInstantiate(Camera)
{
	BindOnDestroy(Camera);
	log_msg("Camera has been instantiated, type id: %u\n", TYPE_ID(Camera));
}


void OnDestroy(Camera)
{
	log_msg("Camera has been destroyed\n");
}
