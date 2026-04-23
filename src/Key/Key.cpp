#include "Key.hpp"

void Key::setCurrentKey(int key)
{
	this->_current_key = key;
}

void Key::setKeyPressed(int key)
{
	this->_key_pressed = key;
}

void Key::setQueueKey(int key)
{
	this->_queue_key = key;
}

int Key::getCurrentKey() const
{
	return _current_key;
}

int Key::getKeyPressed() const
{
	return _key_pressed;
}

int Key::getQueueKey() const
{
	return _queue_key;
}

bool Key::isAutopilotEnabled() const
{
	return _autopilot_enabled;
}

void Key::update()
{
	int key = GetKeyPressed();

	setKeyPressed(key);

	if (key == KEY_A)
	{
		_autopilot_enabled = !_autopilot_enabled;
		return ;
	}

	if (key != KEY_UP && key != KEY_DOWN && key != KEY_LEFT && key != KEY_RIGHT)
		return ;

	if (!_current_key)
		setCurrentKey(key);
	else
		setQueueKey(key);
}