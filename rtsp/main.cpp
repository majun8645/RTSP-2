#include <stdio.h>
#include "MyEvent.h"
#include "RTSP.h"
int main()
{
	MyEvent::getInstance();
	RTSP rtsp("127.0.0.1", 554, "", "", "test.264");
	rtsp.connect();
	std::string data;
	rtsp.OPTIONS(data);
	MyEvent::getInstance()->loop();

	return 0;
}