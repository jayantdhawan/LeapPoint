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
float axMin, azMin, axMax, azMax, denMin, denMax;

typedef struct {
	float min;
	float max;
} extremes_t;

typedef struct {
	extremes_t x;
	extremes_t y;
	extremes_t z;
} tracking_region_t;


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
	int i;

	cout << "Processing frame..." << endl;

	cout << "ID " << oFrame.id() << " FPS " << oFrame.currentFramesPerSecond() << " valid " << oFrame.isValid()
		<< endl;

	HandList oHandList = oFrame.hands();

	cout << "No. of hands detected " << oHandList.count() << endl;

	// TODO: Also for tool
	if (oHandList.count() == 1)
	{	
		Hand oHand = oHandList[0];

		if (!oHand.isValid())
			return;

		int iHandId = oHand.id();

		FingerList oFingerList = oHand.fingers();
		Finger oFinger;

		for (i = 0; i < oFingerList.count(); i++)
		{
			oFinger = oFingerList[i];
			 
			if (!oFinger.isValid())
				continue;

			if (oFinger.type() == Finger::TYPE_INDEX)
			{
				Vector oDir = oFinger.direction();

				if (((oDir.x > axMin) && (oDir.x < axMax)) &
					((oDir.z < azMin) && (oDir.z < azMax)))	// z is going to be more negative if within area
				{
					cout << "Pointing within area\t";
				}
				else
				{
					cout << "Pointing outside area\t";
				}
				cout << "Vector (x, y, z): (" << oDir.x << ", " << oDir.y
					<< ", " << oDir.z << ")\n";
			}
		}

	}

	//bLastFrameProcessed = true;
}

int main(int argc, char *argv[])
{
	bool bRet;

	Controller *pController;
	LPListener_c oListener;

	cout << "LeapPoint\n\n";

	// Specify the tracking region
	tracking_region_t sObj = {
		{-5.0, +4.6},	// x.min, x.max
		{0, 0},			// y.min, y.max
		{-5.0, -5.0}	// z.min, z.max
	};

	denMin = sqrt((sObj.x.min * sObj.x.min) + (sObj.z.min * sObj.z.min));
	denMax = sqrt((sObj.z.max * sObj.z.max) + (sObj.z.max * sObj.z.max));

	axMin = sObj.x.min / denMin;
	azMin = sObj.z.min / denMin;

	axMax = sObj.x.max / denMax;
	azMax = sObj.z.max / denMax;

	cout << "Min (" << axMin << ", " << azMin << ")\t Max (" << axMax << ", " << azMax <<")" << endl;

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
