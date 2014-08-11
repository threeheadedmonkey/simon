#include "SimonSays.h"

//-------------------------------------------------------------------------------------
SimonSays::SimonSays(void)
{
}
//-------------------------------------------------------------------------------------
SimonSays::~SimonSays(void)
{
	delete [] encBuffer;
}

//-------------------------------------------------------------------------------------
void SimonSays::createScene(void)
{
    // set up the basics
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3,0.3,0.3));
	/*Ogre::Light *light = mSceneMgr->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);*/

	// put in a floor ;)
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    plane, 1500, 1500, 40, 40, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* floor = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(floor);

	floor->setMaterialName("Examples/Rockwall");
	floor->setCastShadows(false);

	// for now, use a pinguin, character selection will be added later
	Ogre::Entity *player = mSceneMgr->createEntity("Player", "penguin.mesh");
	Ogre::SceneNode *playerNode = mSceneMgr->getRootSceneNode()->
		createChildSceneNode("PlayerNode");
	playerNode->attachObject(player);
	playerNode->translate(35,20,-65);
	player->setVisible( false );

	// use spots placed in 2x2 field to show the order
	// 
	//  yellow  | red		keys:	u	|	i
	//  ----------------			----------
	//  blue    | green				j	|	k


	Ogre::SceneNode *lightNode = mSceneMgr->getRootSceneNode()->
		createChildSceneNode("LightNode");
	lightNode->setPosition(0,80,0);

	Ogre::Light *yellow = mSceneMgr->createLight("yellowSpot");
	yellow->setType(Ogre::Light::LT_SPOTLIGHT);
	yellow->setDiffuseColour(.5,.5,0);
	yellow->setSpecularColour(.5,.5,0);
	yellow->setDirection(0,-1,0);
	//yellow->setSpotlightInnerAngle(Ogre::Degree(5.0f));
	//yellow->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	//yellow->setSpotlightFalloff(0.0f);
	//yellow->setPosition(Ogre::Vector3(0,0,0));
	//yellow->setSpotlightFalloff(0.0f);
	yellow->setSpotlightRange(Ogre::Degree(15), Ogre::Degree(15));
	lightNode->attachObject(yellow);
	yellow->setVisible( false );

	Ogre::Light *red = mSceneMgr->createLight("redSpot");
	red->setType(Ogre::Light::LT_SPOTLIGHT);
	red->setDiffuseColour(1,0,0);
	red->setSpecularColour(1,0,0);
	red->setDirection(1,-1,0);
	//red->setPosition(Ogre::Vector3(0,0,0));
	//red->setSpotlightFalloff(0.0f);
	red->setSpotlightRange(Ogre::Degree(15), Ogre::Degree(15));
	lightNode->attachObject(red);
	red->setVisible( false );

	Ogre::Light *blue = mSceneMgr->createLight("blueSpot");
	blue->setType(Ogre::Light::LT_SPOTLIGHT);
	blue->setDiffuseColour(0,0,1.0);
	blue->setSpecularColour(0,0,1.0);
	blue->setDirection(0,-1,1);
	//blue->setPosition(Ogre::Vector3(0,100,0));
	blue->setSpotlightRange(Ogre::Degree(15), Ogre::Degree(15));
	lightNode->attachObject(blue);
	blue->setVisible( false );

	Ogre::Light *green = mSceneMgr->createLight("greenSpot");
	green->setType(Ogre::Light::LT_SPOTLIGHT);
	green->setDiffuseColour(0,1,0);
	green->setSpecularColour(0,1,0);
	green->setDirection(1,-1,1);
	//green->setPosition(Ogre::Vector3(100,100,0));
	green->setSpotlightRange(Ogre::Degree(15), Ogre::Degree(15));
	lightNode->attachObject(green);
	green->setVisible( false );
	
	// everything's set up, let's get the game and the streaming started!
	
	mSceneMgr->getLight("yellowSpot")->setVisible( true );
	mSceneMgr->getLight("redSpot")->setVisible( true );
	mSceneMgr->getLight("blueSpot")->setVisible( true );
	mSceneMgr->getLight("greenSpot")->setVisible( true );
	
	turnEnded = false;
	gameStarted = false;
	correctOrder = true;
	pause = false; // set pause to false
	// --> game ist started by user input!!

	frameCounter = 1;

	// set up streaming
	setUpStream();
}

void SimonSays::setUpStream() {
	rtt_texture = 
		Ogre::TextureManager::getSingleton().createManual("RttTex",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,mWindow->getWidth(),mWindow->getHeight(),0,
		Ogre::PF_B8G8R8, // rgb24
		Ogre::TU_RENDERTARGET);

	//Ogre::RenderTexture *
	renderTexture = rtt_texture->getBuffer()->getRenderTarget();
	renderTexture->addViewport(mCamera); // cam see BaseApplication
	renderTexture->getViewport(0)->setClearEveryFrame(true); // clear every frame
	renderTexture->getViewport(0)->setOverlaysEnabled(true); // show overlay elems
	renderTexture->setAutoUpdated(true); // so you don't have to call update() every time

	// encoding stuff
	encBuffer = (char *) malloc(1024*1024*3); 
	encPointer = encBuffer;
	inBuffer = 0;
	
	decoder = new DecodeVideoXVID();
	decoder->global_init(true);
	decoder->dec_init(mWindow->getWidth(), mWindow->getHeight());

	encoder = new EncodeVideoXVID();
	encoder->global_init(true);
	encoder->enc_init(mWindow->getWidth(), mWindow->getHeight());

	sender.init("127.0.0.1",5000);
}

void SimonSays::restartGame( bool restart ) {
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3,0.3,0.3));
	// restart is true if the game is set up anew (e.g. first turn or the player made a mistake)
	if( restart ) {
		if( simonOrder.size() > 0 ) simonOrder.clear();	
		getNewOrder( S_LENGTH ); 
		// only needs to set once!!
		//simonLength = 4;
		//simonExtension = 1;
	} else { // otherwise add 2 spots to simonOrder!
		getNewOrder( S_EXTENSION ); // add new spots to the order
		//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3,0.3,1));
	}
	
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3,0.3,0.3));
	setShowMode(true);
	simonCounter = 0;
	simonTimer = 0;
	turnEnded = false;
	correctOrder = true; // set order to correct again
	playerOrder.clear();
}

bool SimonSays::frameEnded(const Ogre::FrameEvent &evt) {
	bool ret = BaseApplication::frameEnded(evt);

	//if ( showMode ) { // if its in show mode, activate the spots according to the order in simonOrder
		simonTimer += evt.timeSinceLastFrame; // returns time in seconds
		if( pause && simonTimer > 0.5 ) {
			mSceneMgr->getLight("yellowSpot")->setVisible( false );
			mSceneMgr->getLight("redSpot")->setVisible( false );
			mSceneMgr->getLight("blueSpot")->setVisible( false );
			mSceneMgr->getLight("greenSpot")->setVisible( false );
			simonTimer = 0;
			pause = false;
		} 
		
		if( !pause && simonTimer > 1 ) {
			simonTimer = 0;
			if( simonCounter < simonOrder.size() ) { // as long as there's stuff in the vector there's still spots to light up
				turnOnSpot( simonOrder.at(simonCounter) );
				playerOrder.push_back( simonOrder.at(simonCounter) );
				//simonOrder.erase( simonOrder.begin());
				simonCounter++;
				pause = true;
			} else {	// die Spieler-Runde starten
				setShowMode(false);
			}

		} // eof simonTimer > x
	//}
		if( turnEnded ) {
			setShowMode(true); // this turns off all the lights and stuff
		}

	return ret;
}

void SimonSays::log(std::string msg) {
	std::ofstream logfile;
	logfile.open("simon.log", std::ios::app );
	logfile << msg << "\n";
	logfile.close();
}


bool SimonSays::frameRenderingQueued(const Ogre::FrameEvent &evt) {
	/*if( inBuffer > 0 ) {
		memcpy(encBuffer, decPointer, inBuffer); // dst, src, size
	}

	decPointer = encBuffer;*/

	unsigned char *pDest = new unsigned char[mWindow->getWidth()*mWindow->getHeight()*3];
	// Ogre::PixelBox pb(mWindow->getWidth(), mWindow->getHeight(), 1, Ogre::PF_B8G8R8, pDest);

	const Ogre::PixelBox encodeDest = Ogre::PixelBox(mWindow->getWidth(),
									mWindow->getHeight(), 1,
									Ogre::PF_B8G8R8, (void*)(pDest));

	rtt_texture->getBuffer()->blitToMemory(encodeDest);

	// encode
	unsigned char *frame = new unsigned char[mWindow->getWidth()*mWindow->getHeight()*3];
	int encCounter = encoder->enc_main(pDest, frame); // Image, bitstream
	int sendCount = 0;

	memcpy(encBuffer+inBuffer, frame, encCounter); // dst, src, size
	inBuffer += encCounter;

	sendCount = sender.send(encBuffer,inBuffer); // buffer, len
	
	// optional logging to check whether the server sends something
	logMsg.str("");
	logMsg << "sent " << sendCount << " bytes.";
	log( logMsg.str() );

	inBuffer -= sendCount;
	memmove(encBuffer, encBuffer + sendCount, inBuffer); // dst, src, size
	
	delete[] pDest;
	delete[] frame;

	/*//  decode
	unsigned char *decFrame = new unsigned char[mWindow->getWidth()*mWindow->getHeight()*3];
	int decNum =0;
	
	do {
		decNum = decoder->dec_main(decPointer, inBuffer, decFrame);
		decPointer += decNum;
		inBuffer -= decNum;
	} while( decNum > 0 && inBuffer > 1);

	// check if there's something going on
	
	// uncomment from here ...........................................
	Ogre::Image *decImg = new Ogre::Image();
	decImg->loadDynamicImage(decFrame, mWindow->getWidth(), mWindow->getHeight(), Ogre::PF_B8G8R8);
	
	sstream.str("");
	sstream << "frame" << frameCounter << ".jpg";
	decImg->save(sstream.str());
	frameCounter++; */
	// .................... to here if you (don't) want to output files 

	return BaseApplication::frameRenderingQueued(evt);
}

/* 
* method to randomise an order, the param sLength defines how many
* random values will be added to simonOrder!
*/
bool SimonSays::getNewOrder( int sLength ) {
	srand(time(NULL));
	int randPos; 
	
	for ( int i = 0; i < sLength; i++ ) {
		randPos = rand() % 4;
		// set the first light according to the random num, just for testing purposes
		if( randPos == 0 ) {
			// yellow
			//mSceneMgr->getLight("yellowSpot")->setVisible(true);
			simonOrder.push_back(KEY_YELLOW);
		} else if( randPos == 1 ) {
			// red
			//mSceneMgr->getLight("redSpot")->setVisible(true);
			simonOrder.push_back(KEY_RED);
		} else if( randPos == 2 ) {
			// blue
			//mSceneMgr->getLight("blueSpot")->setVisible(true);
			simonOrder.push_back(KEY_BLUE);
		} else {
			// green
			//mSceneMgr->getLight("greenSpot")->setVisible(true);
			simonOrder.push_back(KEY_GREEN);
		}
	}

	// after setting up the simonOrder array the show mode starts
	showMode = true;

	return true;
}

void SimonSays::turnOnSpot( const OIS::KeyCode color ) {
	if( color == KEY_YELLOW ) {
		//Ogre::Light* light = mSceneMgr->getLight("yellowSpot");
		//light->setVisible(! light->isVisible());
		mSceneMgr->getLight("yellowSpot")->setVisible( true );
		mSceneMgr->getLight("redSpot")->setVisible( false );
		mSceneMgr->getLight("blueSpot")->setVisible( false );
		mSceneMgr->getLight("greenSpot")->setVisible( false );
	} else if ( color == KEY_RED ) {
		//Ogre::Light* light = mSceneMgr->getLight("yellowSpot");
		//light->setVisible(! light->isVisible());
		mSceneMgr->getLight("yellowSpot")->setVisible( false );
		mSceneMgr->getLight("redSpot")->setVisible( true );
		mSceneMgr->getLight("blueSpot")->setVisible( false );
		mSceneMgr->getLight("greenSpot")->setVisible( false );
	} else if ( color == KEY_BLUE ) {
		//Ogre::Light* light = mSceneMgr->getLight("yellowSpot");
		//light->setVisible(! light->isVisible());
		mSceneMgr->getLight("yellowSpot")->setVisible( false );
		mSceneMgr->getLight("redSpot")->setVisible( false );
		mSceneMgr->getLight("blueSpot")->setVisible( true );
		mSceneMgr->getLight("greenSpot")->setVisible( false );
	} else if ( color == KEY_GREEN ) {
		//Ogre::Light* light = mSceneMgr->getLight("yellowSpot");
		//light->setVisible(! light->isVisible());
		mSceneMgr->getLight("yellowSpot")->setVisible( false );
		mSceneMgr->getLight("redSpot")->setVisible( false );
		mSceneMgr->getLight("blueSpot")->setVisible( false );
		mSceneMgr->getLight("greenSpot")->setVisible( true );
	}
}

void SimonSays::setShowMode( bool show ) {
	showMode = show;

	if( showMode == false ) {
		mSceneMgr->getLight("yellowSpot")->setVisible( true );
		mSceneMgr->getLight("redSpot")->setVisible( true );
		mSceneMgr->getLight("blueSpot")->setVisible( true );
		mSceneMgr->getLight("greenSpot")->setVisible( true );
		mSceneMgr->getEntity("Player")->setVisible( true );
	} else if ( showMode == true ) {
		mSceneMgr->getLight("yellowSpot")->setVisible( false );
		mSceneMgr->getLight("redSpot")->setVisible( false );
		mSceneMgr->getLight("blueSpot")->setVisible( false );
		mSceneMgr->getLight("greenSpot")->setVisible( false );
		mSceneMgr->getEntity("Player")->setVisible( false );
		mSceneMgr->getSceneNode("PlayerNode")->setPosition( 35,20,-65 );
	}
}

// this method checks whether the player pressed the right key
bool SimonSays::checkKey( const OIS::KeyCode keyPressed ) {
	/*turnOnSpot( simonOrder.front() );
	playerOrder.push_back( simonOrder.front() );
	simonOrder.erase( simonOrder.begin());*/
	bool rightKey;

	if( playerOrder.front() != keyPressed ) {
		//if there's a mistake, set the ambient light to red and restart the game
		rightKey = false;
		correctOrder = false;
		turnEnded = true; // you're turn's over
	} else {
		rightKey = true;
		playerOrder.erase( playerOrder.begin() );
		if( playerOrder.size() < 1 ) {
			turnEnded = true;
		}
		// getNewOrder(simonExtension);
	}

	return rightKey;
}

bool SimonSays::keyPressed( const OIS::KeyEvent &arg ) {
	bool ret = BaseApplication::keyPressed( arg );

	if (arg.key == KEY_START ) {
		if( !gameStarted ) {
			restartGame( true );
			gameStarted = true;
		}
		// (re-)starts the game
		if( turnEnded ) {
			restartGame( !correctOrder ); // if the order is correct, NO restart
		}
	} 

	if( !showMode && !turnEnded ) {
		if (arg.key == KEY_YELLOW) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(0,20,0);
		} else if (arg.key == KEY_RED) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(65,20,0);
		} else if (arg.key == KEY_BLUE) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(0,20,65);
		} else if (arg.key == KEY_GREEN) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(65,20,65);
		}

		// check key after moving the player
		if( playerOrder.size() > 0 ) {
			if( !checkKey(arg.key) ) {
				mSceneMgr->getLight("yellowSpot")->setVisible( false );
				mSceneMgr->getLight("redSpot")->setVisible( false );
				mSceneMgr->getLight("blueSpot")->setVisible( false );
				mSceneMgr->getLight("greenSpot")->setVisible( false );
				mSceneMgr->setAmbientLight(Ogre::ColourValue(1,0.2,0.2));
			};
		} 
		
		if( playerOrder.size() < 1 ) {
			mSceneMgr->getLight("yellowSpot")->setVisible( false );
			mSceneMgr->getLight("redSpot")->setVisible( false );
			mSceneMgr->getLight("blueSpot")->setVisible( false );
			mSceneMgr->getLight("greenSpot")->setVisible( false );
			mSceneMgr->getEntity("Player")->setVisible( false );
			mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2,1,0.2));
		}

	} // eof !showMode

	return ret;
}

// ########################### listen for client input ############################

// TO DO: finish listener thread to get keyboard input from client!!
DWORD WINAPI Listener(LPVOID lpParam) 
{
	UDPReceive receiver = UDPReceive();
	receiver.init(5001); // set to port specified in client

	Ogre::SceneManager* mSceneMgr = (Ogre::SceneManager*)lpParam;
	Ogre::SceneNode* penguin = mSceneMgr->getSceneNode("PlayerNode");
	
	char* data = (char*) malloc(128);
	double* ptime = new double(1);
	std::string cInput;

	int counter;

	while(true) {
		counter = receiver.receive(data,strlen(data),ptime); 
		(*ptime)++;
		cInput.assign(data,0,counter);
		
		// make sure variables are set the same as in client!
		if ((cInput.find("KEY_YELLOW",0)!=std::string::npos)) {
			penguin->setVisible( true );
			penguin->setPosition(0,20,0);
		}

		if ((cInput.find("KEY_RED",0)!=std::string::npos)) {
			penguin->setVisible( true );
			penguin->setPosition(65,20,0);
		}

		if ((cInput.find("KEY_BLUE",0)!=std::string::npos)) {
			penguin->setVisible( true );
			penguin->setPosition(0,20,65);
		}

		if ((cInput.find("KEY_GREEN",0)!=std::string::npos)) {
			penguin->setVisible( true );
			penguin->setPosition(65,20,65);
		}
	}

	return 0; 
} 


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        SimonSays app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
