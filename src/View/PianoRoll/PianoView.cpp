#include "PianoView.h"

PianoView::PianoView(QWidget *parent)
	: QGraphicsView(parent)
{

	QGraphicsScene* scene = new QGraphicsScene;

	int xPos = 0;

	for (int i = 0; i < p_keys.size(); i++)
	{
		auto& key = p_keys[i];

		key = new PianoKey(i);

		bool isBlack = key->isBlack();

		scene->addItem(key);

		key->setZValue(isBlack);
	

		if (i > 0) {

			auto& currentKey = p_keys[i];
			auto& prevKey = p_keys[i-1];

			if (currentKey->isWhite() && prevKey->isWhite())
			{
				xPos += 15;
			}
			else if (currentKey->isBlack() && prevKey->isWhite())
			{
				xPos += 10;
			}
			else
			{
				xPos += 5;
			}
				
		}

		key->setPos(xPos, 0);
	}



	setScene(scene);
}

void PianoView::setNote(int note, bool pressed)
{
	pressed ?
		p_keys[note]->noteOn()
		:
		p_keys[note]->noteOff();
}

PianoView::~PianoView()
{}
