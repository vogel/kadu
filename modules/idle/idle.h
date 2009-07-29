#ifndef IDLE_H
#define IDLE_H

#include <QtCore/QObject>

#include "idle_exports.h"

class IDLEAPI Idle : public QObject
{
	Q_OBJECT

public:
	Idle();
	~Idle();

	bool isActive();
	int secondsIdle();
};

extern IDLEAPI Idle *idle;

#endif
