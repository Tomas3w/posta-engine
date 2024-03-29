#include "App.h"
#include <posta/Util/LoggingMacro.h>
#include "Game.h"

App* app = nullptr;

using namespace std;

posta::Scene* App::start()
{
	return new Game;
}

