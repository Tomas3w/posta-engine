#include "App.h"
#include <posta/Util/LoggingMacro.h>
#include "MainScene.h"

std::unique_ptr<App> app;

using namespace std;

posta::Scene* App::start()
{
	return new MainScene;
}

