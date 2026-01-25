#include "APP.hpp"



int main(void) 
{
	APP_Init();

	t1.start();

	while(1)
	{
		System::run();
	}
}