#ifndef __TANKGAME_GAMECONTROLLER__
#define __TANKGAME_GAMECONTROLLER__

#include "IController.h"
#include "Command.h"

class GameController : public IController {
private:
  BaseGameView* _view;
  BaseGameModel* _model;
  
  vector<IPlayer*> _players;
  int _currentPlayerTurn;

  #pragma region IController Data
  AppConfig* _appConfig;
  TileManager* _tileManager;
  CImgDisplay* _mainDisplay;
  CImg<unsigned char>* _displayImage;

  bool _autoMode;
  bool _ended;
  bool _ending;

  #pragma endregion

protected:
  // prevent copy
  GameController(const GameController& g) {}
  GameController& operator=(const GameController& g) {}

public:
  #pragma region IControllerImplementation
  void setConfig(AppConfig* config) {
    _appConfig = config;
  }

  void setTileManager(TileManager* tileManager) {
    _tileManager = tileManager;
  }

  void setDisplayImage(CImg<unsigned char>* image) {
    _displayImage = image;
  }
  void setDisplay(CImgDisplay* display) {
    _mainDisplay = display;
  }

  bool registerPlayer(IPlayer* player) {
    IPlayer* newPlayer = _model->registerPlayer(player);

    if (newPlayer != NULL) {
      _players.push_back(newPlayer);
      return true;
    }

    return false;
  }

  ~GameController() {
    delete _appConfig;
    delete _tileManager;
  }

  #pragma endregion

  GameController(GameView* v, GameModel* m, CImgDisplay& dis, autoM = true) 
    : _view(v), _model(m), _mainDisplay(dis),
      _autoMode(autoM), _ended(false), _ending(false),
      _currentPlayerTurn(0) {}

  // the controller makes the next turn
  bool nextTurn() {
    
    // reduce down:
    int totalPlayer = _players.size();
    if (currentPlayerTurn >= totalPlayer)
        currentPlayerTurn %= totalPlayer;

    IPlayer* currentPlayer = _players[currentPlayerTurn++];
    

    Command nextMove = currentPlayer->nextMove();

    if (_model->isValidMove(currentPlayer, nextMove)) {
      vector<pair<int,int>> changes = _model->applyMove(currentPlayer, nextMove);

      _view->update(changes);

      _ending = _model->isEndGame();

      return true;
    } else {
      return false;
    }
  }

  bool start() {
    for (list<IPlayer*>::iterator it = _players.begin(); it != _players.end(); ++it) {
      (*it)->onStart();
    }
    
    return true;
  }

  bool finish() {
    for (list<IPlayer*>::iterator it = _players.begin(); it != _players.end(); ++it) {
      (*it)->onFinish();
    }
    _ended = true;
  }

  bool toggleMode() {
    _autoMode = !_autoMode;
  }

  bool isInAutoMode() const {
    return _autoMode;
  }

  bool isEnding() const {
    return _ending;
  }

  bool isEnded() const {
    return _ended;
  }

  void updateDisplay() {
    _view->updateDisplay(_mainDisplay);
  }
};
#endif