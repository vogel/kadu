#ifndef POWERKADU_H
#define POWERKADU_H

#include <QtCore/QObject>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "action.h"

class AboutDialog;

class PowerKadu : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:
		PowerKadu();
		~PowerKadu();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	private:
		AboutDialog *aboutDialog;
		ActionDescription *powerKaduMenuActionDescription;
		void unloadModules();

	private slots:
		void onAboutPowerKadu();

	protected:
		virtual void configurationUpdated();
};

extern PowerKadu* powerKadu;

#endif
