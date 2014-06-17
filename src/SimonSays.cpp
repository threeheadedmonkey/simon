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

	showMode = true;
	
	simonTimer = 0;
	getNewOrder( 4 ); // start with a length of 4 fields to remember

	turnOnSpot( KEY_YELLOW );
	// called when starting game and when starting a new show order
	
	timer.reset();
	startTimer();
}

bool SimonSays::frameRenderingQueued(const Ogre::FrameEvent &evt) {
	bool ret = BaseApplication::frameRenderingQueued(evt);
	//turnOnSpot( KEY_BLUE );

	//if ( showMode ) { // if its in show mode, activate the spots according to the order in simonOrder
		simonTimer += evt.timeSinceLastFrame; // returns time in seconds
		if( simonTimer > 5) {
			simonTimer = 0;
			turnOnSpot( KEY_GREEN );
			/*if( simonOrder.size() > 0 ) { // as long as there's stuff in the vector there's still spots to light up
				turnOnSpot( simonOrder.front() );
				simonOrder.erase( simonOrder.begin());
			} else {	// die Spieler-Runde starten
				showMode = false;
			}*/
		}
	//}

	return ret;
}

void SimonSays::startTimer() {
	while( showMode ) {
		if( timer.getMilliseconds() > 2000 ) {
			timer.reset();
			//turnOnSpot( KEY_RED );

			
		}
	}
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

void SimonSays::restartGame() {
	// restarts the game and increases simonCount
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

bool SimonSays::keyPressed( const OIS::KeyEvent &arg ) {
	bool ret = BaseApplication::keyPressed( arg );

	//bool showMode = true; // lights are showing the order

	// if the order is wrong, set the ambient light to red or something like that

	if (arg.key == KEY_SWITCH) {
		// toggle between show and playmode
		if( showMode == true ) {
			// turn off showMode, turn on spots, set player
			mSceneMgr->getLight("yellowSpot")->setVisible( true );
			mSceneMgr->getLight("redSpot")->setVisible( true );
			mSceneMgr->getLight("blueSpot")->setVisible( true );
			mSceneMgr->getLight("greenSpot")->setVisible( true );
			mSceneMgr->getEntity("Player")->setVisible( true );
			showMode = false;
		} else {
			mSceneMgr->getLight("yellowSpot")->setVisible( false );
			mSceneMgr->getLight("redSpot")->setVisible( false );
			mSceneMgr->getLight("blueSpot")->setVisible( false );
			mSceneMgr->getLight("greenSpot")->setVisible( false );
			mSceneMgr->getEntity("Player")->setVisible( false );
			mSceneMgr->getSceneNode("PlayerNode")->setPosition( 35,20,-65 );
			showMode = true;
		}
	} else if (arg.key == KEY_YELLOW) {
		if( !showMode ) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(0,20,0);
		}
	} else if (arg.key == KEY_RED) {
		if( !showMode ) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(65,20,0);
		}
	} else if (arg.key == KEY_BLUE) {
		if( !showMode ) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(0,20,65);
		}
	} else if (arg.key == KEY_GREEN) {
		if( !showMode ) {
			Ogre::SceneNode* playerNode = mSceneMgr->getSceneNode("PlayerNode");
			playerNode->setPosition(65,20,65);
		}
	}

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
