#ifndef CONFIGURATION_AWARE_OBJECT
#define CONFIGURATION_AWARE_OBJECT

#include <qglobal.h>

#include <q3valuelist.h>

class ConfigurationAwareObject
{
	static Q3ValueList<ConfigurationAwareObject *> objects;

	static void registerObject(ConfigurationAwareObject *object);
	static void unregisterObject(ConfigurationAwareObject *object);

public:
	static void notifyAll();

	ConfigurationAwareObject();
	virtual ~ConfigurationAwareObject();

	virtual void configurationUpdated() = 0;
};

#endif // CONFIGURATION_AWARE_OBJECT
