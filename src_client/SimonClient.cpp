/*
-----------------------------------------------------------------------------
Filename:    SimonClient.cpp
-----------------------------------------------------------------------------

Adapted file from
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
#include "SimonClient.h"

//-------------------------------------------------------------------------------------
SimonClient::SimonClient(void)
{
}
//-------------------------------------------------------------------------------------
SimonClient::~SimonClient(void)
{
	delete [] decBuffer;
	decoder->dec_stop();
	receiver.closeSock();
	delete ptime;
}

//-------------------------------------------------------------------------------------
void SimonClient::createScene(void)
{
	// set up sender for keyboard input
	clientSend = UDPSend();
	clientSend.init("127.0.0.1",5001);
	
	// setup decoder
	decoder = new DecodeVideoXVID();
	decoder->global_init(true);
	decoder->dec_init(mWindow->getWidth(), mWindow->getHeight());

	decBuffer = (char *) malloc(1024*1024*2);
	inBuffer = 0;

	receiver.init(5000);
	ptime = new double(1);

	// render to texture...
	// see http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Intermediate+Tutorial+7

	rtt_texture = Ogre::TextureManager::getSingleton().createManual("RttTex", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, 
		Ogre::PF_BYTE_RGB, Ogre::TU_RENDERTARGET);

	renderTexture = rtt_texture->getBuffer()->getRenderTarget();
	

	// get video data from server and render
	streamedScreen = new Ogre::Rectangle2D(true);
	streamedScreen->setCorners(-1.0, 1.0, 1.0, -1.0); // Left is -1.0 and right +1.0, top is +1.0 and bottom -1.0
	streamedScreen->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
    
	Ogre::SceneNode* sScreenNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("sScreenNode");
	sScreenNode->attachObject(streamedScreen);

	Ogre::MaterialPtr renderMaterial = Ogre::MaterialManager::getSingleton().create("RttMat", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique* matTechnique = renderMaterial->createTechnique();
	matTechnique->createPass();
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex");

	streamedScreen->setMaterial("RttMat");
	renderTexture->addListener(this);

	frameCounter = 1; // to save files from frames
}

bool SimonClient::frameEnded(const Ogre::FrameEvent& evt) {
	// mInputContext.capture();
	// char* keyPressed; // only when if condition is met ;)

	renderTexture->update();

	if( mKeyboard->isKeyDown(KEY_YELLOW) ){
		char* keyPressed = "KEY_YELLOW"; // *!!
		clientSend.send(keyPressed,strlen(keyPressed));
	};
	if( mKeyboard->isKeyDown(KEY_RED) ){
		char* keyPressed = "KEY_RED"; 
		clientSend.send(keyPressed,strlen(keyPressed));
	};
	if( mKeyboard->isKeyDown(KEY_BLUE) ){
		char* keyPressed = "KEY_BLUE"; 
		clientSend.send(keyPressed,strlen(keyPressed));
	};
	if( mKeyboard->isKeyDown(KEY_GREEN) ){
		char* keyPressed = "KEY_GREEN";
		clientSend.send(keyPressed,strlen(keyPressed));
	};

	return BaseApplication::frameEnded(evt);
}

void SimonClient::log( std::string msg ) {
	std::ofstream logfile;
	logfile.open("simon_client.log", std::ios::app );
	logfile << msg << "\n";
	logfile.close();
}

bool SimonClient::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	unsigned char *decFrame = new unsigned char[mWindow->getWidth()*mWindow->getHeight()*3];
	int decCount = 0;

	Ogre::PixelBox pb(mWindow->getWidth(),mWindow->getHeight(), 1, Ogre::PF_B8G8R8, decFrame);

	int receivedCount = receiver.receive(decBuffer,65000,ptime);
	(*ptime)++;
	inBuffer += receivedCount;

	// optional logging to check how much bytes were received
	logMsg.str("");
	logMsg << "received " << receivedCount << " bytes.";
	log( logMsg.str() );

	do {
		decCount = decoder->dec_main(decBuffer, inBuffer, decFrame);
		//decBuffer += decCount;
		inBuffer -= decCount;
		memmove(decBuffer, decBuffer+decCount, inBuffer); // dst, src, size
	} while( decCount > 0 );

	//Ogre::PixelBox pb(mWindow->getWidth(),mWindow->getHeight(), 1, Ogre::PF_B8G8R8, decFrame);
	rtt_texture->getBuffer()->blitFromMemory(pb);
	
	// (un-)comment from here ...........................................
	Ogre::Image *decImg = new Ogre::Image();
	decImg->loadDynamicImage(decFrame, mWindow->getWidth(), mWindow->getHeight(), Ogre::PF_B8G8R8);
	
	/*
	filename.str("");
	filename << "frame" << frameCounter << ".png";
	decImg->save(filename.str());
	frameCounter++; 
	*/
	// .................... to here if you (don't) want to output files 
	

	delete[] decFrame;

	// in FrameEnded???
	// renderTexture->update();

	return BaseApplication::frameRenderingQueued(evt);
}


void SimonClient::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
	streamedScreen->setVisible(false);
}

void SimonClient::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
	streamedScreen->setVisible(true);
}

// class implements RenderTargetListener (used to receive notifications of RenderTarget events)
// TODO: check what happens if you leave that out ;) 

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
        SimonClient app;

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
