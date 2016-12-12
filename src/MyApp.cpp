

#include "MyApp.h" 

MyApp::MyApp() {
  _sceneManager = NULL;
  _framelistener = NULL;
  
}

MyApp::~MyApp() {
  delete _root;
  delete _framelistener;
}

int MyApp::start() {
  _root = new Ogre::Root();
  
  if(!_root->restoreConfig()) {
    _root->showConfigDialog();
    _root->saveConfig();
  }
  
  Ogre::RenderWindow* window = _root->initialise(true,"MasterMind");
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
  _sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  // _sceneManager->addRenderQueueListener(new Ogre::OverlaySystem());
  
  Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
  cam->setPosition(Ogre::Vector3(0,0,50));
  cam->lookAt(Ogre::Vector3(0,0,0));
  cam->setNearClipDistance(5);
  cam->setFarClipDistance(10000);

  Ogre::Viewport* viewport = window->addViewport(cam);
  viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));
  double width = viewport->getActualWidth();
  double height = viewport->getActualHeight();
  cam->setAspectRatio(width / height);
  
  loadResources();
  createScene();
  //createOverlay();
  //Ogre::SceneNode *node = _sceneManager->getSceneNode("SinbadNode");

  _framelistener = new MyFrameListener(window, cam, /*_overlayManager,*/ _sceneManager);
  _root->addFrameListener(_framelistener);
  
  createGUI();
  
  _root->startRendering();
  return 0;
}

void MyApp::loadResources() {
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void MyApp::createScene() {
 
  Ogre::SceneNode* node2 = _sceneManager->createSceneNode("light");
  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); 
  Ogre::Light* light = _sceneManager->createLight("Light1");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(Ogre::Vector3(1,-1,-1));
  node2->attachObject(light);

  _sceneManager->getRootSceneNode()->addChild(node2);
}

void MyApp::createGUI(){
  //CEGUI
  renderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

  // Let's make the OS and the CEGUI cursor be in the same place
  CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();  
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);
 
 CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
  //Sheet
  CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Ex2");

  //Config Window
  CEGUI::Window* menu = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu.layout");
  
  
  CEGUI::Window* exitButton = menu->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::quit, 
                    _framelistener));

  CEGUI::Window* creditsButton = menu->getChild("CreditsButton");
  creditsButton->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::credits,
                    _framelistener));

  CEGUI::Window* playButton = menu->getChild("PlayButton");
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::play,
                    _framelistener));

  CEGUI::Window* credits = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("credits.layout");
  credits->setVisible(false);
  
  CEGUI::Window* returnButton = credits->getChild("ReturnButton");
  returnButton->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::back,
                    _framelistener));

  CEGUI::Window* game = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("game.layout");
  game->setVisible(false);
  
  CEGUI::Window* pause = game->getChild("PauseButton");
  pause->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::pause,
                    _framelistener));

  CEGUI::Window* finish = game->getChild("finishButton");
  finish->setVisible(false);
  finish->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::finish,
                    _framelistener));

  CEGUI::Window* restart = game->getChild("restartButton");
  restart->setVisible(false);
  restart->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::restart,
                    _framelistener));
  game->getChild("restartButton")->setVisible(false);


  /*
  CEGUI::Window* returnButton = credits->getChild("ReturnButton");
  returnButton->subscribeEvent(CEGUI::PushButton::EventClicked,
             CEGUI::Event::Subscriber(&MyFrameListener::back,
                    _framelistener));
  */

  //Attaching buttons
  sheet->addChild(menu);
  sheet->addChild(credits);
  sheet->addChild(game);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}



/*
void MyApp::createOverlay() {
  _overlayManager = Ogre::OverlayManager::getSingletonPtr();
  Ogre::Overlay* overlay = _overlayManager->getByName("Info");
  overlay->show();
}
*/
