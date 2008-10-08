#ifndef PARSER_EXTENDER_H
#define PARSER_EXTENDER_H

#include <QtCore/QObject>
#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class ParserExtender : public ConfigurationUiHandler, ConfigurationAwareObject
{
    Q_OBJECT
	public:
		ParserExtender();
		~ParserExtender();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	private:
		void init();
		void close();
		bool isStarted;

	protected:
		virtual void configurationUpdated();
};

extern ParserExtender *parserExtender;
#endif
