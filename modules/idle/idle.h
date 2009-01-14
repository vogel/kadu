#ifndef IDLE_H
#define IDLE_H

#include <QtCore/QObject>

class Idle : public QObject
{
	Q_OBJECT

public:
	Idle();
	~Idle();

	bool isActive();
	int secondsIdle();

public slots:
	void test();
};

extern Idle *idle;

#endif
