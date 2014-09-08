/*
-----------------------------------------------------------------------------
Filename:    SimonClient.h
-----------------------------------------------------------------------------

Adapted file from the 
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "BaseApplication.h"
#include "UDPReceive.h"
#include "UDPSend.h"
#include "DecodeVideoXVID.h"
#include "xvid.h"

// for logging
#include <time.h>

class SimonClient : public BaseApplication, public Ogre::RenderTargetListener 
{
public:
    SimonClient(void);
    virtual ~SimonClient(void);

protected:
    virtual void createScene(void);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);

	// set static variables for quick resetting
	static const OIS::KeyCode KEY_START = OIS::KC_S; // restart the game after an error
	static const OIS::KeyCode KEY_YELLOW = OIS::KC_U;
	static const OIS::KeyCode KEY_RED = OIS::KC_I;
	static const OIS::KeyCode KEY_BLUE = OIS::KC_J;
	static const OIS::KeyCode KEY_GREEN = OIS::KC_K;

	// logging
	void log( std::string msg );
	std::stringstream logMsg;

	// for testing... remove later
	int frameCounter;
	std::stringstream filename;

	// streaming stuff
	int counter;
	Ogre::RenderTexture *renderTexture;
	Ogre::TexturePtr rtt_texture;

	UDPSend clientSend; // sends key input to server
	UDPReceive receiver;

	DecodeVideoXVID *decoder;
	unsigned int inBuffer;
	char *decBuffer;
	double *ptime;

	Ogre::Rectangle2D* streamedScreen;
};

// #endif 
