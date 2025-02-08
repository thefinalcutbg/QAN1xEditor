#pragma once

class QStatusBar;
class Browser;

namespace GlobalWidgets {
	
	enum Answer { No, Yes, Cancel };

	inline QStatusBar* statusBar{ nullptr };
	inline Browser* browser{nullptr};

	bool askQuestion(const char* question);
	void showMessage(const char* message);
	int YesNoCancelQuestion(const char* question); //Yes = 1, No = 0, Cancel = -1
}