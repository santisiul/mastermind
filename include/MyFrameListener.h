#include <Ogre.h>
//#include <OgreFrameListener.h>
//#include <OgreRenderWindow.h>
#include <OIS.h> 
#include <CEGUI.h>
/*#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
*/
#define STAGE 1 << 0
#define SPHERE 1 << 1
#define BTN 1 << 2





using namespace std;

class MyFrameListener : public Ogre::FrameListener, OIS::KeyListener, OIS::MouseListener {
private:
	OIS::InputManager* _inputManager;
	OIS::Keyboard* _keyboard;
  OIS::Mouse* _mouse;
  Ogre::Camera* _camera;
  Ogre::RenderWindow* _win;
	Ogre::SceneManager* _sceneManager;
  Ogre::RaySceneQuery* _raySceneQuery;
  //Ogre::OverlayManager* _overlayManager;

  float chronometer;

  int rounds;
	int spheres;
	
  vector<string> materials;
  	
  vector<Ogre::SceneNode*> options;         //Esferas de opciones
  vector<Ogre::Vector3> posOptions;
  int optionsNum;
  
  vector<Ogre::Vector3> posCombination;     //Esfera de combinacion elegida
  vector<int> combination;
  int combinationSize;

  vector<Ogre::Vector3> hintsPos;
  vector<Ogre::SceneNode*> hints;           //Esferas de indicio

  vector<Ogre::SceneNode*> randomSpheres;   //Esferas de combinacion aleatoria
  vector<int> randomCombination;

  Ogre::SceneNode* ball;
  Ogre::Vector3 finalPosition;
  int indexPosition;

  bool translate;

  Ogre::Vector3 moveCam;
  //  vector<Ogre::Vector3> combPos;

  //vector<Ogre::SceneNode*> spheresComb;  
  	
  //  bool menu;
  //  bool initial;
  	//bool credits;
  //	bool score;
    bool init;
  	bool inGame;
  	bool evaluate;
    bool moveCamera;
  //	bool win;

  Ogre::Ray setRayQuery(int posx, int posy, Ogre::uint32 mask);

  bool keyPressed(const OIS::KeyEvent& evt);
  bool keyReleased(const OIS::KeyEvent& evt);
  bool mouseMoved(const OIS::MouseEvent& evt);
  bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
  bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

  bool _quit;
  CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);
public:
  MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam,
   /* Ogre::OverlayManager* om,*/ Ogre::SceneManager* sm);
  ~MyFrameListener();
  bool frameStarted(const Ogre::FrameEvent& evt);

  void initialState();
  void clean();
  void choose(int posx, int posy, Ogre::Real deltaT);
  void evaluation();
  void createCombination(int combination);
  void reloadOptions();
  void translateCamera(Ogre::Real deltaT);
  void clock();
  Ogre::SceneNode* createObject(string mesh, string nameNode, string material, Ogre::Vector3 position, Ogre::uint32 mask);

  bool quit(const CEGUI::EventArgs &e);
  bool credits(const CEGUI::EventArgs &e);
  bool play(const CEGUI::EventArgs &e);
  bool back(const CEGUI::EventArgs &e);
  bool pause(const CEGUI::EventArgs &e);
  bool finish(const CEGUI::EventArgs &e);
  bool restart(const CEGUI::EventArgs &e);

};

