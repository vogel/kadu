#ifndef PARSER_EXTENDER_H
#define PARSER_EXTENDER_H

#include <QtCore/QObject>
#include "configuration/configuration-aware-object.h"

class ParserExtender : public ConfigurationAwareObject
{
	public:
		ParserExtender();
		~ParserExtender();

	private:
		void init();
		void close();
		bool isStarted;

	protected:
		virtual void configurationUpdated();
};

extern ParserExtender *parserExtender;
#endif
