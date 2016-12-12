#include <Ogre.h>
#include <CEGUI.h>
#include <vector>
#include "MyFrameListener.h"
#include <RendererModules/Ogre/Renderer.h>

//using namespace std;
using namespace Ogre;

class MyApp {
  
private:
  Ogre::SceneManager* _sceneManager;
  Ogre::Root* _root;
  MyFrameListener* _framelistener;
  //Ogre::OverlayManager* _overlayManager;
  CEGUI::OgreRenderer* renderer;
  
  
public:
  MyApp();
  ~MyApp();  
  int start();
  void loadResources();
  void createScene();

  void createGUI();
  //void createOverlay();
  //void createSphere(string mesh, string nameNode, string material, Ogre::Vector3 position);
};
