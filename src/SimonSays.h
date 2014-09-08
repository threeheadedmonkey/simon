#include "BaseApplication.h"
#include "UDPReceive.h"
#include "UDPSend.h"
#include "EncodeVideoXVID.h"
#include "DecodeVideoXVID.h"

#include <iostream>
#include <fstream>

// for logging
#include <time.h>

class SimonSays : public BaseApplication
{
public:
    SimonSays(void);
    virtual ~SimonSays(void);
	

protected:
    virtual void createScene(void);
	//virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	virtual bool frameEnded(const Ogre::FrameEvent &evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	virtual bool keyPressed( const OIS::KeyEvent &arg );

	// logging
	void log( std::string msg );
	std::stringstream logMsg;
	time_t currTime;
	//std::ofstream logfile;

	bool getNewOrder( int sLength );
	OIS::KeyCode getRandomSpot(); // adds a random spot to the array
	void restartGame( bool restart );
	void turnOnSpot( const OIS::KeyCode color );
	void setShowMode( bool show );
	bool checkKey( const OIS::KeyCode keyPressed );


	bool gameStarted; // for the first start --> temporary, needs to be improved

	bool pause; // to include pauses between spots, just in case the same color shows up twice
	bool showMode;
	bool turnEnded; // to check if a turn has ended and the 'S' key is activated
	bool correctOrder; // whether the player entered the correct order, could be done using playerOrder too (check if it's empty)
	float simonTimer; // doesn't actually need to be declared in header file, sets the intervall for how long a light pops up
	unsigned int simonCounter; // counter for how many spots have been shown yet
	
	
	static const int S_LENGTH = 4; // how many spots you have to remember, e.g. round 1: 4 colours to remember -> simonCounter = 4; round2: simonCounter+=2,...
	static const int S_EXTENSION = 1; // how many spots are added per successful turn

	std::vector<OIS::KeyCode> simonOrder;	// the order the game sets
	std::vector<OIS::KeyCode> playerOrder;	// the order the player presses

	static const OIS::KeyCode KEY_START = OIS::KC_S; // restart the game after an error
	static const OIS::KeyCode KEY_YELLOW = OIS::KC_U;
	static const OIS::KeyCode KEY_RED = OIS::KC_I;
	static const OIS::KeyCode KEY_BLUE = OIS::KC_J;
	static const OIS::KeyCode KEY_GREEN = OIS::KC_K;

	// streaming stuff
	void setUpStream();

	Ogre::RenderTexture *renderTexture;
	Ogre::TexturePtr rtt_texture;
	EncodeVideoXVID *encoder;
	
	// testing decoding
	DecodeVideoXVID *decoder;
	char *decPointer;
	int frameCounter;
	std::string name;
	std::stringstream sstream;


	// encoding
	int inBuffer;
	char* encBuffer;
	char* encPointer;

	//std::string name;
	int counter;

	UDPSend sender;

	/*std::stringstream sstream;*/
	/*unsigned char *decBuffer;
	unsigned char *decPointer;
	int inBuffer; // count whats in

	EncodeVideoXVID *encoder;
	DecodeVideoXVID *decoder;*/
};

