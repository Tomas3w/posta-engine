#include "App.h"
#include <posta/Util/LoggingMacro.h>
#include "Game.h"

std::unique_ptr<App> app;

using namespace std;

posta::Scene* App::start()
{
	return new Game;
}

