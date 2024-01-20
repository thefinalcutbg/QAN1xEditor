#pragma once

class QStatusBar;
class Browser;

namespace GlobalWidgets {
	
	inline QStatusBar* statusBar{ nullptr };
	inline Browser* browser{nullptr};

	bool askQuestion(const char* question);
}