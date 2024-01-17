#pragma once

class QStatusBar;

#include <string>
namespace GlobalWidgets {
	
	inline QStatusBar* statusBar{ nullptr };

	bool askQuestion(const char* title, const char* question);
}