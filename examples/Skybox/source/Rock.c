
#include <Rock.h>
#include <calltrace/calltrace.h>

void OnInstantiate(Rock)
{
	BindOnDestroy(Rock);
	log_msg("Rock has been instantiated, type id: %u\n", TYPE_ID(Rock));
}


void OnDestroy(Rock)
{
	log_msg("Rock has been destroyed\n");
}
