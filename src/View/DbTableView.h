#include <QTableView>

class DbTableView : public QTableView
{
	Q_OBJECT

	void dropEvent(QDropEvent* e) override;
	void keyPressEvent(QKeyEvent* event) override;

public:
	DbTableView(QWidget* parent);

	~DbTableView();

signals:
	void dataDroped();
	void deletePressed();
	void copyRequested();
};