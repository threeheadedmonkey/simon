#include "BaseApplication.h"

class SimonSays : public BaseApplication
{
public:
    SimonSays(void);
    virtual ~SimonSays(void);
	

protected:
    virtual void createScene(void);
	//virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	virtual bool frameEnded(const Ogre::FrameEvent &evt);
	virtual bool keyPressed( const OIS::KeyEvent &arg );

	bool getNewOrder( int sLength );
	OIS::KeyCode getRandomSpot(); // adds a random spot to the array
	void restartGame( bool restart );
	void turnOnSpot( const OIS::KeyCode color );
	void setShowMode( bool show );
	bool checkKey( const OIS::KeyCode keyPressed );

	bool pause; // to include pauses between spots, just in case the same color shows up twice
	bool showMode;
	float simonTimer; // doesn't actually need to be declared in header file, sets the intervall for how long a light pops up
	int simonCounter; // counter for how many spots have been shown yet
	int simonLength; // how many spots you have to remember, e.g. round 1: 4 colours to remember -> simonCounter = 4; round2: simonCounter+=2,...
	int simonExtension; // how many spots are added per successful turn

	std::vector<OIS::KeyCode> simonOrder;	// the order the game sets
	std::vector<OIS::KeyCode> playerOrder;	// the order the player presses

	static const OIS::KeyCode KEY_START = OIS::KC_S; // restart the game after an error
	static const OIS::KeyCode KEY_YELLOW = OIS::KC_U;
	static const OIS::KeyCode KEY_RED = OIS::KC_I;
	static const OIS::KeyCode KEY_BLUE = OIS::KC_J;
	static const OIS::KeyCode KEY_GREEN = OIS::KC_K;

	Ogre::RenderTexture *renderTexture;
	Ogre::TexturePtr rtt_texture;
	
	std::string name;
	int counter;
	/*std::stringstream sstream;*/



	/*unsigned char *decBuffer;
	unsigned char *decPointer;
	int inBuffer; // count whats in

	EncodeVideoXVID *encoder;
	DecodeVideoXVID *decoder;*/
};

