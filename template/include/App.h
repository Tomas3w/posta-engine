#ifndef APP_H
#define APP_H
#include <posta/App.h>

using namespace std;

class App : public posta::App
{
	public:
		posta::Scene* start() override;
};

extern App* app;

#endif // APP_H
