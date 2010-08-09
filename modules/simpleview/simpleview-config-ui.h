#ifndef SIMPLEVIEW_CONFIG_UI_H
#define SIMPLEVIEW_CONFIG_UI_H

#include <QtCore/QObject>


#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

class SimpleViewConfigUi :
	public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SimpleViewConfigUi)

	static SimpleViewConfigUi *Instance;
	bool KeepSize;

	SimpleViewConfigUi();
	virtual ~SimpleViewConfigUi();

	void createDefaultConfiguration();

private slots:
	void mainConfigurationWindowDestroyed();

protected:
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();
	static SimpleViewConfigUi *instance(){return Instance; }

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	bool keepSize() {return KeepSize; }
};

#endif
