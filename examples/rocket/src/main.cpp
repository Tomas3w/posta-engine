#include <App.h>

int main(int argc, char* argv[])
{
	app.reset(new App());
	app->loop();
	return EXIT_SUCCESS;
}

