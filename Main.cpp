/*
*
* LeapPoint
* A LeapMotion application to detect the direction in which a
* user is pointing his finger. Uses the Leap Motion Controller
* API
* Author: Jayant Dhawan
*
*/

#include <iostream>
#include "Leap.h"

#define LOGTOFILE 0
#ifdef LOGTOFILE
//#define cout fout
#endif

using namespace std;
using namespace Leap;

bool bLastFrameProcessed = true;
Frame oFrame;
void processThisFrame();

class LPListener_c : public Listener
{
public:
	virtual void onInit(const Controller&);
	virtual void onServiceConnect(const Controller &);
	virtual void onConnect(const Controller &);

	virtual void onFrame(const Controller &);

	virtual void onDisconnect(const Controller &);	
	virtual void onServiceDisconnect(const Controller&);
};

void LPListener_c::onInit(const Controller&)
{
	cout << "Initialized\n";
}

void LPListener_c::onServiceConnect(const Controller &controller)
{
	cout << "Connected to service" << endl;

	Controller::PolicyFlag eCurrentPolicy = controller.policyFlags();

	cout << "Current policy flags: " << eCurrentPolicy << endl;
}

void LPListener_c::onConnect(const Controller &)
{
	cout << "Connected\n";
}

void LPListener_c::onFrame(const Controller & controller)
{
	static int iIgnore = 0;

	if (iIgnore++ < 20)
		return;
	else
		iIgnore = 0;

	cout << "Got a frame\n";

	//if (bLastFrameProcessed == true)
	{
		// Get the most recent frame
		oFrame = controller.frame(0);
		//bLastFrameProcessed = false;
		if (oFrame.isValid())
			processThisFrame();
	}
}

void LPListener_c::onDisconnect(const Controller &)
{
	cout << "Disconnected\n";
}

void LPListener_c::onServiceDisconnect(const Controller &)
{
	cout << "Disconnected from service\n";
}

void processThisFrame()
{
	static int iLastHandId = 0;

	cout << "Processing frame..." << endl;

	cout << "ID " << oFrame.id() << " FPS " << oFrame.currentFramesPerSecond() << " valid " << oFrame.isValid()
		<< endl;

	if (!oFrame.isValid())
		return;

	HandList oHandList = oFrame.hands();

	cout << "No. of hands detected " << oHandList.count() << endl;

	// TODO: Also for tool
	if (oHandList.count() == 1)
	{	
		Hand oHand = oHandList[0];

		if (!oHand.isValid())
			return;

		//int iHandId = oHand.id();

		//cout << "No. of fingers " << oHand.fingers().count() << endl;
	}

	//bLastFrameProcessed = true;
}

int main(int argc, char *argv[])
{
	bool bRet;

	Controller *pController;
	LPListener_c oListener;

	cout << "LeapPoint\n\n";

	// New controller object, add a listener
	pController = new Controller;

	bRet = pController->addListener(oListener);
	if (!bRet) {
		cout << "LeapPoint Error " << bRet;
		return 1;
	}

	//Config oConfig = pController->config();

	// Wait for user input before exiting, and also provide a delay
	cin.get();

	bRet = pController->removeListener(oListener);
	if (!bRet) {
		cout << "LeapPoint Error " << bRet;
	}

	// Wait for user input before exiting, and also provide a delay
	cin.get();

	delete pController;

	return 0;
}

