#ifndef MPD_CONFIG
#define MPD_CONFIG

#include "gui/windows/main-configuration-window.h"

class MPDConfig : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

		void createDefaultConfiguration();

	public:
		MPDConfig();
		~MPDConfig();

		QString host;
		QString port;
		QString timeout;

		void mainConfigurationWindowCreated(MainConfigurationWindow *                          mainConfigurationWindow);

	protected:
		 void configurationUpdated();

};

#endif
