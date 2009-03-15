#ifndef POWERKADU_H
#define POWERKADU_H

#include <QtCore/QObject>
#include "action.h"

class AboutDialog;

class PowerKadu : public QObject
{
	Q_OBJECT

	public:
		PowerKadu();
		~PowerKadu();

	private:
		AboutDialog *aboutDialog;
		ActionDescription *powerKaduMenuActionDescription;

	private slots:
		void onAboutPowerKadu();
};

extern PowerKadu* powerKadu;

#endif
