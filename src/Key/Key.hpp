#pragma once

#include "../../lib/lib.hpp"
#include <iostream>
#include <vector>

class Key
{
private:
	int _current_key = 0;
	int _key_pressed = 0;
	int _queue_key = 0;
	bool _autopilot_enabled = false;

public:
	void setCurrentKey(int key);
	void setKeyPressed(int key);
	void setQueueKey(int key);
	int getCurrentKey() const;
	int getKeyPressed() const;
	int getQueueKey() const;
	bool isAutopilotEnabled() const;
	void update();
};