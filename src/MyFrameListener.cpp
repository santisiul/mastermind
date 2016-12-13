#include "MyFrameListener.h"

MyFrameListener::MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, /*Ogre::OverlayManager* om,*/ Ogre::SceneManager* sm){
  OIS::ParamList param;
  size_t windowHandle;  std::ostringstream wHandleStr;

  _sceneManager = sm;
  _camera = cam;
  _win = win;
  /*_overlayManager = om;*/
  
  win->getCustomAttribute("WINDOW", &windowHandle);
  wHandleStr << windowHandle;
  param.insert(std::make_pair("WINDOW", wHandleStr.str()));
  param.insert(std::make_pair("x11_mouse_hide", std::string("true")));

  #if defined OIS_WIN32_PLATFORM
   param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
   param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
   param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
   param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
   param.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
   param.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
   param.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
  
  _inputManager = OIS::InputManager::createInputSystem(param);
  _keyboard = static_cast<OIS::Keyboard*>
    (_inputManager->createInputObject(OIS::OISKeyboard, true));
  _mouse = static_cast<OIS::Mouse*>
    (_inputManager->createInputObject( OIS::OISMouse, true));
 
  _keyboard->setEventCallback(this);
  _mouse->setEventCallback(this);
  _raySceneQuery = _sceneManager->createRayQuery( Ogre::Ray());
    
    _quit = false;
  
    init = true;
    inGame = false;
    //evaluate = false; //duda
    rounds = 0;
    moveCamera =false;
   
   
    file = new File();
}

MyFrameListener::~MyFrameListener() {
  _inputManager->destroyInputObject(_keyboard);
  _inputManager->destroyInputObject( _mouse);
  _sceneManager->destroyQuery( _raySceneQuery);
  OIS::InputManager::destroyInputSystem(_inputManager);

}

Ogre::Ray MyFrameListener::setRayQuery(int posx, int posy, Ogre::uint32 mask) {
  Ogre::Ray rayMouse = _camera->getCameraToViewportRay
    (posx/float(_win->getWidth()), posy/float(_win->getHeight()));
  _raySceneQuery->setRay(rayMouse);
  _raySceneQuery->setSortByDistance(true);
  _raySceneQuery->setQueryMask(mask);
  return (rayMouse);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
  
  Ogre::Real deltaT = evt.timeSinceLastFrame;
   

  bool mbleft;

  CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(deltaT);
  _keyboard->capture();  _mouse->capture();   // Captura eventos

  int posx = _mouse->getMouseState().X.abs;   // Posicion del puntero
  int posy = _mouse->getMouseState().Y.abs;   //  en pixeles.

  mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
  //if (_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
  if (_quit){
    file->save();
    return false;  
  }
    
  
  //Bucle
  if (init)
  {
     initialState();
  }
 

  if (inGame)
  {
    chronometer = chronometer + deltaT;
    clock();
    if (mbleft && !translate )
    {
      choose(posx,posy,deltaT);
    }
  
    if (translate)
    {
      Ogre::Vector3 v = finalPosition - ball->getPosition();
      v = v * deltaT * 10;
      ball->translate(v);
      if(ball->getPosition().squaredDistance(finalPosition) <= 0.001)
      {
        translate = false; 
      }
    }
    

  }
  
  if(evaluate){
    evaluation();
  }
  
  translateCamera(deltaT);
  return true;
}

void MyFrameListener::clock(){
  ostringstream strm; strm << static_cast<int>(chronometer / 60) << ":" << static_cast<int>(chronometer) % 60;   

  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* gameWindow = rootWindow->getChildRecursive("game");

  CEGUI::Window* clock = gameWindow->getChildRecursive("clock");
  clock->setText(strm.str());
}

void MyFrameListener::initialState(){
  //string mesh = "Sphere.mesh";
 _sceneManager->clearScene();

  Ogre::SceneNode* node3 = _sceneManager->createSceneNode("light");
  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); 
  Ogre::Light* light = _sceneManager->createLight("Light1");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(Ogre::Vector3(1,-1,-1));
  node3->attachObject(light);

  _sceneManager->getRootSceneNode()->addChild(node3);
 
  chronometer = 0;
  evaluate =false;
  translate= false;
  spheres = 0;
  optionsNum = 6;
  combinationSize = 4;
  rounds = 0;
  ball = NULL;
  indexPosition = 0;

  
    if (materials.size() == 0)
    {
      cout<<"entra a crear por primera vez"<<endl;
      materials.push_back("red");
      materials.push_back("green");
      materials.push_back("blue");
      materials.push_back("yellow");
      materials.push_back("orange");
      materials.push_back("violet");

      for(int i = 0; i < 13; i+=4){ posCombination.push_back(Ogre::Vector3(i-8,-14,0)); }
      for (int i = -9; i < 10; i+=3){ posOptions.push_back(Ogre::Vector3(i,-15,2)); }
      for (int i = 0; i < optionsNum; ++i){ options.push_back(NULL); }
    
      for (int i = 0; i < combinationSize; ++i){ combination.push_back(9); }

      hintsPos.push_back(Ogre::Vector3(7,-13.5,1));
      hintsPos.push_back(Ogre::Vector3(8,-13.5,1));
      hintsPos.push_back(Ogre::Vector3(7,-14.5,1));
      hintsPos.push_back(Ogre::Vector3(8,-14.5,1));

    }
    else{
      for (int i = 0; i < combinationSize; ++i)
      {
        posCombination[i]= Ogre::Vector3(i*4-8,-14,0);
      }
      for (int i = 0; i < combinationSize; ++i){ hints.pop_back(); } 

      for (int i = 0; i < combinationSize; ++i){ combination[i] = 9; } 
      hintsPos[0] = Ogre::Vector3(7,-13.5,1);
      hintsPos[1] = Ogre::Vector3(8,-13.5,1);
      hintsPos[2] = Ogre::Vector3(7,-14.5,1);
      hintsPos[3] = Ogre::Vector3(8,-14.5,1);

      cout<<"randomcombinsize"<<randomSpheres.size()<<endl;

      for (int i = 0; i < combinationSize; ++i){ randomSpheres.pop_back(); }
      for (int i = 0; i < combinationSize; ++i){ randomCombination.pop_back(); }
    }
    
    Ogre::SceneNode* node1 = createObject("Plane.mesh", "back", "back", Ogre::Vector3(0,0,0), STAGE);
    _sceneManager->getRootSceneNode()->addChild(node1);

    Ogre::SceneNode* node2 = createObject("Button.mesh", "button", "button", Ogre::Vector3(0,-18,1), BTN);
    node1->addChild(node2);
    //_sceneManager->getRootSceneNode()->addChild(node2);
  
  
    /*Ogre::SceneNode* node = createObject("front.mesh", "front", "front", Ogre::Vector3(0,16.7,2.6), STAGE);
    _sceneManager->getRootSceneNode()->getChild("back")->addChild(node);
    */
 
  
  
  reloadOptions();
 cout<<"initial state"<<endl;
  createCombination(combinationSize);
     
  evaluation();
  init = false;
}

void MyFrameListener::choose(int posx, int posy, Ogre::Real deltaT){
  Ogre::Ray r = setRayQuery(posx, posy, ~STAGE);
  Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
  Ogre::RaySceneQueryResult::iterator it;
    it = result.begin();

    if (it != result.end()) {

      Ogre::SceneNode* node = it->movable->getParentSceneNode();
      Ogre::Entity* ent  = static_cast<Ogre::Entity*>(it->movable);

      int pos = 5;
      string mat = ent->getSubEntity(0)->getMaterialName();
      

      if (mat == "button") //Click en el boton
      {
        if(indexPosition+1 > 3){
          evaluate = true;
          inGame = false;
        }
      }
      else if (mat == "alpha"){}
      else{

        if (mat == "red")        { pos = 0; }
        else if (mat == "green") { pos = 1; }
        else if (mat == "blue")  { pos = 2; }
        else if (mat == "yellow"){ pos = 3; }
        else if (mat == "orange"){ pos = 4; }
        else if (mat == "violet"){ pos = 5; }

    
        translate = true;
          ball = node;
        
          bool back = false;
          int n = 0;
          for (int i = 0; i < combinationSize; ++i)
          {
            if (node->getPosition().squaredDistance(posCombination[i]) <= 0.001)
            {
              back = true;
              
              n = i;
              break;
            }
        
          }
       

          for (int i = 0; i < combinationSize; ++i)
          {
            if (combination[i] == 9)
            {
              indexPosition = i;
              break;
            }
            indexPosition = 5;
          }

          if(back)
          {
            combination[n] = 9;
            options[pos] = node;
            finalPosition = posOptions[pos];
            _sceneManager->getRootSceneNode()->removeChild( node);
            _sceneManager->getRootSceneNode()->getChild("back")->addChild(node);

          }
          else if(indexPosition < combinationSize){
            combination[indexPosition] = pos;
            options[pos] = NULL;
            finalPosition = posCombination[indexPosition];
            _sceneManager->getRootSceneNode()->getChild("back")->removeChild(node);
            _sceneManager->getRootSceneNode()->addChild(node);
          }
          else{
            translate = false;
          }
            
        }
      }
}

void MyFrameListener::translateCamera(Ogre::Real deltaT){
  
  if (moveCamera){
    Ogre::Vector3 v = (moveCam-_camera->getPosition()) *deltaT*10;
    
    //Ogre::Vector3 v; 
    _camera->setPosition(_camera->getPosition() + v);
    _sceneManager->getRootSceneNode()->getChild("back")->translate(v);
    if (_camera->getPosition().squaredDistance(moveCam) <= 0.1)
    {
      moveCamera = false;
    }
    
  }
}
void MyFrameListener::evaluation(){
 
  cout<<"evaluation"<<endl;
  bool win = false;
  int blacks = 0;
  int whites = 0;

  if (rounds > 0)
  {    
    
    for (int i = 0; i < combinationSize; ++i)
    {
      for (int j = 0; j < combinationSize; ++j)
      {
        if (combination[i] == randomCombination[j])
        {
          if (i == j){ blacks++; }
          else{ whites++; }
        }
      }
    }
    if (blacks == combinationSize){ win = true; }      //Gana el Jugador
    for (int i = 0; i < combinationSize; ++i)
    {
      if (blacks > 0)
      {
        Ogre::Entity* ent = static_cast<Ogre::Entity*>(hints[i]->getAttachedObject(0));
        ent->setMaterialName("black");
        blacks--;
      }
      else if (whites > 0)
      {
        Ogre::Entity* ent = static_cast<Ogre::Entity*>(hints[i]->getAttachedObject(0));
        ent->setMaterialName("white");
        whites--;  
      }
    }
  }

  if (!win)
  {

    cout<<"round"<<rounds<<endl;
    evaluate = false;
    indexPosition = 0;
    rounds++;
    Ogre::Vector3 relocate = Ogre::Vector3(0, 4*rounds, 0);
    
    if (rounds > 6)
    {
      moveCamera = true;
      moveCam = _camera->getPosition() + Ogre::Vector3(0,4,0);
    }
    
    
    
    ostringstream sstrm; sstrm << "bar" << rounds;
    Ogre::SceneNode* node = createObject("bar.mesh", sstrm.str(), "wood", Ogre::Vector3(0,-14 ,0) + relocate, STAGE);
    _sceneManager->getRootSceneNode()->addChild(node);

    if(rounds > 1){
      inGame =  true;
      Ogre::Vector3 relocate2 = Ogre::Vector3(0, 4*(rounds-1), 0);
      ostringstream sstrm1; sstrm1 << "table" << rounds;
                      node = createObject("bar.mesh", sstrm1.str(), "alpha", Ogre::Vector3(0,-14 ,1) + relocate2, SPHERE);
      _sceneManager->getRootSceneNode()->addChild(node);
    }

    for (int i = 0; i < combinationSize; ++i)
    {
      ostringstream strm; strm << "node" << ++spheres;
      Ogre::SceneNode* node = createObject("Sphere.mesh", strm.str(), "grey", hintsPos[i] + relocate, STAGE);
      if(rounds == 1){ hints.push_back(node); }
      else hints[i] = node;
      _sceneManager->getRootSceneNode()->addChild(node);
    }

    //Se actualizan posiciones de combinacion
    for (int i = 0; i < combinationSize; ++i)
    {
      posCombination[i] = posCombination[i] + Ogre::Vector3(0,4,0);
      combination[i] = 9;               //Se limpia la combinacion
    }
    

    reloadOptions();
  }
  else{
    cout<<"ganaste"<<endl;
    evaluate = false;
    winner();
  }
}

Ogre::SceneNode* MyFrameListener::createObject(string mesh, string nameNode, string material, Ogre::Vector3 position, Ogre::uint32 mask){
  Ogre::Entity* entTmp = _sceneManager->createEntity(mesh);
  Ogre::SceneNode* nodeTmp = _sceneManager->createSceneNode(nameNode);
  entTmp->setMaterialName(material);
  entTmp->setQueryFlags(mask);
  nodeTmp->attachObject(entTmp);
  nodeTmp->setPosition(position);

  return nodeTmp;
}

void MyFrameListener::createCombination(int elementos){
  srand(time(NULL));
  
  while(randomCombination.size() != static_cast<size_t>(elementos))
  {
    cout<<"crea esferas ramdom"<<endl;
    int color = rand()%optionsNum; 
    
    bool repetead = false;
    for (size_t i = 0; i < randomCombination.size(); ++i)
    {
         if(color == randomCombination[i]) repetead = true;
    }
    if(!repetead) randomCombination.push_back(color);
  }
  
  if (randomSpheres.size() == 0)
  { 
 
    vector<Ogre::Vector3> combPos;
    for (int i = -3; i < 4; i+=2){ combPos.push_back(Ogre::Vector3(i,17,1.5)); }
    for (int i = 0; i < elementos; ++i)
    {
      ostringstream strm; strm << "node" << ++spheres;   
      Ogre::SceneNode* node = createObject("Sphere.mesh", strm.str(), materials[randomCombination[i]], combPos[i], STAGE);
      _sceneManager->getRootSceneNode()->getChild("back")->addChild(node);
      randomSpheres.push_back(node);
    }
  }
  else{
    for (int i = 0; i < elementos; ++i)
    {
      cout<<"randomspheres no vacio";
      static_cast<Ogre::Entity*>(randomSpheres[i]->getAttachedObject(0))->setMaterialName(materials[randomCombination[i]]);
    }
  }
}

void MyFrameListener::reloadOptions(){
 
  for (int i = 0; i < optionsNum; ++i)
  {
    if (options[i] == NULL || rounds == 0)
    {
      ostringstream strm;
      strm << "node" << ++spheres;
      Ogre::SceneNode* node = createObject("Sphere.mesh", strm.str(), materials[i], posOptions[i], SPHERE);
      options[i] = node;
      _sceneManager->getRootSceneNode()->getChild("back")->addChild(node);
      //_sceneManager->getRootSceneNode()->addChild( node);
    }
  }
}

void MyFrameListener::winner()
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* winWindow = rootWindow->getChildRecursive("win"); 
  winWindow->setVisible(true);
}

bool MyFrameListener::keyPressed(const OIS::KeyEvent& evt)
{
  if(evt.key==OIS::KC_ESCAPE) return _quit=true;
 
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(evt.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(evt.text);

  return true;
}

bool MyFrameListener::keyReleased(const OIS::KeyEvent& evt)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(evt.key));
  return true;
}

bool MyFrameListener::mouseMoved(const OIS::MouseEvent& evt)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);  
  return true;
}

bool MyFrameListener::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  return true;
}

bool MyFrameListener::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  return true;
}

CEGUI::MouseButton MyFrameListener::convertMouseButton(OIS::MouseButtonID id)
{
  CEGUI::MouseButton ceguiId;
  switch(id)
    {
    case OIS::MB_Left:
      ceguiId = CEGUI::LeftButton;
      break;
    case OIS::MB_Right:
      ceguiId = CEGUI::RightButton;
      break;
    case OIS::MB_Middle:
      ceguiId = CEGUI::MiddleButton;
      break;
    default:
      ceguiId = CEGUI::LeftButton;
    }
  return ceguiId;
}


bool MyFrameListener::quit(const CEGUI::EventArgs &e)
{
  _quit = true;
  return true;
}

bool MyFrameListener::credits(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  rootWindow->getChildRecursive("menu")->setVisible(false);
  rootWindow->getChildRecursive("credits")->setVisible(true);
  return true;
}
bool MyFrameListener::score(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  rootWindow->getChildRecursive("menu")->setVisible(false);
  CEGUI::Window* scoreWindow = rootWindow->getChildRecursive("score");
  scoreWindow->setVisible(true);

  stringstream sstrm; 
  for (int i = 0; i < file->getGamers(); ++i)
  {
     sstrm << file->getGamer(i) << endl;
  }
  sstrm << "[vert-alignment='top']";
  scoreWindow->getChild("text")->setText(sstrm.str());
  return true;
}

bool MyFrameListener::play(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  rootWindow->getChildRecursive("menu")->setVisible(false);
  rootWindow->getChildRecursive("game")->setVisible(true);


  //createCombination(combinationSize);
  inGame = true;
  return true;
}

bool MyFrameListener::back(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  rootWindow->getChildRecursive("menu")->setVisible(true);
  rootWindow->getChildRecursive("credits")->setVisible(false);
  rootWindow->getChildRecursive("score")->setVisible(false);

  return true;
}

bool MyFrameListener::pause(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* gameWindow = rootWindow->getChildRecursive("game");

  CEGUI::Window* finishButton = gameWindow->getChildRecursive("finishButton");
  bool show;
  if (finishButton->isVisible()){ show = false; }
  else{ show = true; }
  
  gameWindow->getChildRecursive("finishButton")->setVisible(show);
  gameWindow->getChildRecursive("restartButton")->setVisible(show);

  return true;
}

bool MyFrameListener::finish(const CEGUI::EventArgs &e)
{
  
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  rootWindow->getChildRecursive("menu")->setVisible(true);
  rootWindow->getChildRecursive("game")->setVisible(false);

  CEGUI::Window* gameWindow = rootWindow->getChildRecursive("game");
  gameWindow->getChildRecursive("finishButton")->setVisible(false);
  gameWindow->getChildRecursive("restartButton")->setVisible(false);

  CEGUI::Window* winWindow = rootWindow->getChildRecursive("win");
   string name = winWindow->getChildRecursive("nameEditbox")->getText().c_str();
  if (winWindow->isVisible())
  {

    stringstream sstrm; sstrm << name << " " << rounds << static_cast<int>(chronometer / 60) << ":" << (static_cast<int>(chronometer) % 60)<<endl;
    cout<<sstrm.str()<<endl;
    file->addGamer(sstrm.str());
  }
  rootWindow->getChildRecursive("win")->setVisible(false);

  initialState();
  //Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(_sceneManager->getRootSceneNode()->getChild("back")->getChild("front"));
  //node->setVisible(false);
  inGame = false;
  evaluate = false;

  

  return true;
}

bool MyFrameListener::restart(const CEGUI::EventArgs &e)
{
  CEGUI::Window* rootWindow=  CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* gameWindow = rootWindow->getChildRecursive("game");
  gameWindow->getChildRecursive("finishButton")->setVisible(false);
  gameWindow->getChildRecursive("restartButton")->setVisible(false);

  rootWindow->getChildRecursive("win")->setVisible(false);

  initialState();
  inGame = true;
  return true;
}