#include "SimonSays.h"

//-------------------------------------------------------------------------------------
SimonSays::SimonSays(void)
{
}
//-------------------------------------------------------------------------------------
SimonSays::~SimonSays(void)
{
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


	// RenderTexture stuff
	Ogre::TexturePtr rtt_texture = 
		Ogre::TextureManager::getSingleton().createManual("RttTex",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,mWindow->getWidth(),mWindow->getHeight(),0,
		Ogre::PF_R8G8B8, // rgb24
		Ogre::TU_RENDERTARGET);

	//Ogre::RenderTexture *
	renderTexture = rtt_texture->getBuffer()->getRenderTarget();
	renderTexture->addViewport(mCamera); // cam see BaseApplication
	renderTexture->getViewport(0)->setClearEveryFrame(true); // clear every frame
	renderTexture->getViewport(0)->setOverlaysEnabled(true); // show overlay elems
	renderTexture->setAutoUpdated(true); // so you don't have to call update() every time



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

	// start the game!
	restartGame( false );


	/*simonLength = 4;
	setShowMode(true);
	simonExtension = 1;
	simonLength = 4;
	simonCounter = 0;
	
	/*for( int i = 0; i < simonLength; i++) { // fill simonOrder with randomly generated spotcolors
		simonOrder.push_back( getRandomSpot() );
	}
	getNewOrder( simonLength ); 
	simonTimer = 0;*/
	
	
	//counter = 0;
	// turnOnSpot( KEY_YELLOW );
	// called when starting game and when starting a new show order

	pause = false; // set pause to false
}

bool SimonSays::frameEnded(const Ogre::FrameEvent &evt) {
	bool ret = BaseApplication::frameEnded(evt);
	//turnOnSpot( KEY_BLUE );

	//if ( showMode ) { // if its in show mode, activate the spots according to the order in simonOrder
		simonTimer += evt.timeSinceLastFrame; // returns time in seconds
		if( pause && simonTimer > 2 ) {
			mSceneMgr->getLight("yellowSpot")->setVisible( false );
			mSceneMgr->getLight("redSpot")->setVisible( false );
			mSceneMgr->getLight("blueSpot")->setVisible( false );
			mSceneMgr->getLight("greenSpot")->setVisible( false );
			simonTimer = 0;
			pause = false;
		} 
		
		if( !pause && simonTimer > 2 ) {
			simonTimer = 0;
			//turnOnSpot( KEY_GREEN );
			if( simonCounter < simonLength ) { // as long as there's stuff in the vector there's still spots to light up
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

		/*renderTexture->writeContentsToFile(counter+".jpg");
		counter++;*/

	return ret;
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

OIS::KeyCode SimonSays::getRandomSpot() {
	srand(time(NULL));
	int randPos;
	randPos = rand() % 4;
	OIS::KeyCode retColor;

	if( randPos == 0 ) {
		// yellow
		//mSceneMgr->getLight("yellowSpot")->setVisible(true);
		retColor = KEY_YELLOW;
	} else if( randPos == 1 ) {
		// red
		//mSceneMgr->getLight("redSpot")->setVisible(true);
		retColor = KEY_RED;
	} else if( randPos == 2 ) {
		// blue
		//mSceneMgr->getLight("blueSpot")->setVisible(true);
		retColor = KEY_BLUE;
	} else {
		// green
		//mSceneMgr->getLight("greenSpot")->setVisible(true);
		retColor = KEY_GREEN;
	}

	return retColor;
}

void SimonSays::restartGame( bool restart ) {
	// restart is false if the game's started for the first time
	simonOrder.clear();
	simonLength = 4;
	setShowMode(true);
	simonExtension = 1;
	simonCounter = 0;
	getNewOrder( simonLength ); 
	simonTimer = 0;
	
	/*for( int i = 0; i < simonLength; i++) { // fill simonOrder with randomly generated spotcolors
		simonOrder.push_back( getRandomSpot() );
	}*/
	
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
		mSceneMgr->setAmbientLight(Ogre::ColourValue(1,0.2,0.2));
		rightKey = false;
	} else {
		rightKey = true;
		playerOrder.erase( playerOrder.begin() );
		// getNewOrder(simonExtension);
	}

	return rightKey;
}

bool SimonSays::keyPressed( const OIS::KeyEvent &arg ) {
	bool ret = BaseApplication::keyPressed( arg );

	//bool showMode = true; // lights are showing the order

	// if the order is wrong, set the ambient light to red or something like that

	if (arg.key == KEY_START ) {
		// re-starts the game
		restartGame( true );

		// toggle between show and playmode
		/*if( showMode == true ) {
			setShowMode( false );
			// turn off showMode, turn on spots, set player
		} else {
			setShowMode( true );
		}*/
	} 

	if( !showMode ) {
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
		checkKey( arg.key );

	} // eof !showMode

	return ret;
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
