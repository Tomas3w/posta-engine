#include "Game.h"
#include <posta/Util/LoggingMacro.h>

int main(int argc, char* argv[])
{
	std::unique_ptr<App> application(new App);
	app = application.get();
	app->loop();
	return EXIT_SUCCESS;
}

