#ifndef BASE_GAME_H
#define BASE_GAME_H

#include "ConsoleRenderer.h"

class BaseGame
{
public:
	BaseGame() {}
	virtual ~BaseGame() {}

	void SetRenderer(ConsoleRenderer _renderer) { renderer = _renderer; }
	virtual void LoadContent() = 0;
	virtual bool Update(float deltaTime) = 0;
	virtual void UnloadContent() = 0;

protected:
	ConsoleRenderer renderer;
};

#endif
