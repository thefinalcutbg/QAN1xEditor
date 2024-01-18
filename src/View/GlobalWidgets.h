#pragma once

class QStatusBar;

namespace GlobalWidgets {
	
	inline QStatusBar* statusBar{ nullptr };

	bool askQuestion(const char* question);
}