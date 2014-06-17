#include "BaseApplication.h"

class SimonSays : public BaseApplication
{
public:
    SimonSays(void);
    virtual ~SimonSays(void);
	

protected:
    virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	//virtual bool frameEnded(const Ogre::FrameEvent &evt);
	virtual bool keyPressed( const OIS::KeyEvent &arg );

	bool getNewOrder( int sLength );
	void restartGame();
	void SimonSays::turnOnSpot( const OIS::KeyCode color );
	void SimonSays::startTimer();

	bool showMode;
	int simonTimer; // doesn't actually need to be declared in header file, sets the intervall for how long a light pops up
	int simonCounter; // holds the value for current order length, e.g. round 1: 4 colours to remember -> simonCounter = 4; round2: simonCounter+=2,...

	std::vector<OIS::KeyCode> simonOrder;

	Ogre::Timer timer;

	static const OIS::KeyCode KEY_SWITCH = OIS::KC_S;
	static const OIS::KeyCode KEY_YELLOW = OIS::KC_U;
	static const OIS::KeyCode KEY_RED = OIS::KC_I;
	static const OIS::KeyCode KEY_BLUE = OIS::KC_J;
	static const OIS::KeyCode KEY_GREEN = OIS::KC_K;

	/*Ogre::RenderTexture *renderTexture;
	Ogre::TexturePtr rtt_texture;
	int counter;
	std::string name;
	std::stringstream sstream;*/



	/*unsigned char *decBuffer;
	unsigned char *decPointer;
	int inBuffer; // count whats in

	EncodeVideoXVID *encoder;
	DecodeVideoXVID *decoder;*/
};

