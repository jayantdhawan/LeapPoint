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
void processThisFrame_Tool();
void processThisFrame_Hand_Tool();
float axMin, azMin, axMax, azMax, ayMin, ayMax, denMin, denMax;

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

	if (iIgnore++ < 100)
		return;
	else
		iIgnore = 0;

	cout << endl;
	//cout << "Got a frame\n";

	//if (bLastFrameProcessed == true)
	{
		// Get the most recent frame
		oFrame = controller.frame(0);

		//bLastFrameProcessed = false;
		if (oFrame.isValid())
			processThisFrame_Hand_Tool();
		else {
			cout << "Number of hands 0 Number of tools 0";
		}
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
	//static int iLastHandId = 0;
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

		//int iHandId = oHand.id();

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
				Vector currTipPos = oFinger.tipPosition();

				// Specify the tracking region
				tracking_region_t sObj = {
						{ -50, +46 },	// x.min, x.max
						{ +15, +95 },			// y.min, y.max
						{ -50, -50 }	// z.min, z.max
				};

				//sObj.x.min -= currTipPos.x;
				//sObj.x.max += currTipPos.x;
				sObj.y.min -= currTipPos.y;
				sObj.y.max -= currTipPos.y;
				//sObj.z.min -= currTipPos.z;

				//cout << "Tip Position: " << currTipPos.x << ", " << currTipPos.y << ", " << currTipPos.z << endl;
				//cout << "New values: y min " << sObj.y.min << " y max " << sObj.y.max << endl;

				denMin = sqrt((sObj.x.min * sObj.x.min) + (sObj.z.min * sObj.z.min));
				denMax = sqrt((sObj.x.max * sObj.x.max) + (sObj.z.max * sObj.z.max));

				axMin = sObj.x.min / denMin;
				//ayMin = sObj.y.min / denMin;
				azMin = sObj.z.min / denMin;

				axMax = sObj.x.max / denMax;
				//ayMax = sObj.y.max / denMax;
				azMax = sObj.z.max / denMax;

				cout << "OA Min (" << axMin << ", " << ", " << ayMin << ", " << azMin << ")\n Max (" 
					<< axMax << ", " << ", " << ayMax << ", "<< azMax << ")" << endl;

				if (((oDir.x > axMin) && (oDir.x < axMax)) &&
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

void processThisFrame_Tool()
{
	ToolList oToolList = oFrame.tools();

	if (oToolList.count() == 1)
	{
		Tool oTool = oToolList[0];

		if (!oTool.isValid())
			return;

		//int iHandId = oHand.id();

		Vector oDir = oTool.direction();
		Vector currTipPos = oTool.tipPosition();

		// Specify the tracking region
		tracking_region_t sObj = {
				{ -50, +46 },	// x.min, x.max
				{ +15, +95 },			// y.min, y.max
				{ -50, -50 }	// z.min, z.max
		};

		//sObj.x.min -= currTipPos.x;
		//sObj.x.max += currTipPos.x;
		sObj.y.min -= currTipPos.y;
		sObj.y.max -= currTipPos.y;
		//sObj.z.min -= currTipPos.z;

		//cout << "Tip Position: " << currTipPos.x << ", " << currTipPos.y << ", " << currTipPos.z << endl;
		//cout << "New values: y min " << sObj.y.min << " y max " << sObj.y.max << endl;

		denMin = sqrt((sObj.x.min * sObj.x.min) + (sObj.z.min * sObj.z.min));
		denMax = sqrt((sObj.x.max * sObj.x.max) + (sObj.z.max * sObj.z.max));

		axMin = sObj.x.min / denMin;
		//ayMin = sObj.y.min / denMin;
		azMin = sObj.z.min / denMin;

		axMax = sObj.x.max / denMax;
		//ayMax = sObj.y.max / denMax;
		azMax = sObj.z.max / denMax;

		cout << "OA Min (" << axMin << ", " << ", " << ayMin << ", " << azMin << ")\n Max (" 
			<< axMax << ", " << ", " << ayMax << ", "<< azMax << ")" << endl;

		if (((oDir.x > axMin) && (oDir.x < axMax)) &&
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

	//bLastFrameProcessed = true;
}

void processThisFrame_Hand_Tool()
{
	//static int iLastHandId = 0;
	int i;
	Vector tipPosFinger(0, 0, 0), tipPosTool(0, 0, 0);
	Vector tipDirFinger(0, 0, 0), tipDirTool(0, 0, 0);
	float t1x, t1y, t1z, d1x, d1y, d1z;
	float t2x, t2y, t2z, d2x, d2y, d2z;
	
	cout << "Processing frame..." << endl;

	cout << "ID " << oFrame.id() << " FPS " << oFrame.currentFramesPerSecond() << " valid " << oFrame.isValid()
		<< endl;

	// 
	// Get the finger tip's position first
	//
	
	HandList oHandList = oFrame.hands();
	ToolList oToolList = oFrame.tools();
	
	cout << "Number of hands " << oHandList.count();
	cout << " Number of tools " << oToolList.count() << endl;
	
	if (oHandList.count() == 1) {	
		Hand oHand = oHandList[0];

		if (!oHand.isValid())
			return;

		FingerList oFingerList = oHand.fingers();

		for (i = 0; i < oFingerList.count(); i++) {
			Finger oFinger = oFingerList[i];
			 
			if (!oFinger.isValid())
				continue;

			if (oFinger.type() == Finger::TYPE_INDEX) {
				tipPosFinger = oFinger.tipPosition();
				t1x = tipPosFinger.x;
				t1y = tipPosFinger.y;
				t1z = tipPosFinger.z;

				tipDirFinger = oFinger.direction();
				d1x = tipDirFinger.x;
				d1y = tipDirFinger.y;
				d1z = tipDirFinger.z;
			}
		}
	}
	else
		return;

	// 
	// Now the tool's tip's position
	//	

	if (oToolList.count() == 1)
	{
		Tool oTool = oToolList[0];

		if (!oTool.isValid())
			return;

		//int iHandId = oHand.id();

		tipPosTool = oTool.tipPosition();
		t2x = tipPosTool.x;
		t2y = tipPosTool.y;
		t2z = tipPosTool.z;

		tipDirTool = oTool.direction();
		d2x = tipDirTool.x;
		d2y = tipDirTool.y;
		d2z = tipDirTool.z;
	}
	else
		return;
	
	cout << "Tool " << tipPosTool.x << " Finger " << tipPosFinger.x << endl;
	
	
	// From www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
	
	// Get the angle between the two lines (using their unit vectors)
	float mag_dot_prod = d1x*d2x + d1y*d2y + d1z*d2z;
	float cosine_w = (mag_dot_prod < 0) ? -(mag_dot_prod) : (mag_dot_prod);
	cout << "Cosine of angle: " << cosine_w << endl;
	
	// Get the vector from the tip of the finger to the tip of the tool, to determine 'vector d'
	float Dx = t2x - t1x;
	float Dy = t2y - t1y;
	float Dz = t2z - t1z;
	
	// Get the D.(cross-product of the unit direction vectors)
	float k = (Dx * (d1y*d2z - d1z*d2y) +
				Dy  * (d1z*d2x - d1x*d2z) +
				Dz * (d1x*d2y - d1y*d2x)) / 
				sqrt(1 - (cosine_w*cosine_w));
		
	cout << "Minimum distance: " << k << endl;

	if (k < 20 && k > -20)
		cout << "Pointing at the finger" << endl;
	else
		cout << "Pointing elsewhere" << endl;
		
/*
	if (tipPosTool.x - tipPosFinger.x < 50)
		cout << "Within proximity" << endl;
	else
		cout << "Outside proximity" << endl;
*/

}

int main(int argc, char *argv[])
{
	bool bRet;

	Controller *pController;
	LPListener_c oListener;

	cout << "LeapPoint\n\n";


	//cout << "Min (" << axMin << ", " << azMin << ")\t Max (" << axMax << ", " << azMax <<")" << endl;

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
