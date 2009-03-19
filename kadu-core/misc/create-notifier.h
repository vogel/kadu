#ifndef KADU_CREATE_NOTIFIER_H
#define KADU_CREATE_NOTIFIER_H

#include "base.h"

/**
	klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier
	do ktorej mog� si� pod��cza� pozosta�e cze�ci kodu.
	przed wyj�ciem z konstruktora wywo�ujemy metod�:
	createNotifier.notify(this);
**/
class CreateNotifier : public QObject
{
	Q_OBJECT

public:
	void notify(QObject *new_object);

signals:
	void objectCreated(QObject *new_object);

};
#endif