#include "fconf.c"
#include "fconf_client.h"
#include "fbus_service.h"

int main(int argc, char* argv[])
{
	fbus_service_init(argc, argv);

	FConf* fconf = fconf_client_create();
	fconf_test(fconf);
	fbus_service_run();

	fconf_destroy(fconf);

	return 0;
}
