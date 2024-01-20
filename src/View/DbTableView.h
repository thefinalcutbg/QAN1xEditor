#include <QTableView>

class DbTableView : public QTableView
{
	Q_OBJECT

	void dropEvent(QDropEvent* e) override;

public:
	DbTableView(QWidget* parent);

	~DbTableView();

signals:
	void dataDroped();

};