#ifndef KADU_PARSER_H
#define KADU_PARSER_H

#include <qmap.h>
#include <qstring.h>

class UserListElement;

class KaduParser
{

private:

	struct ParseElem
	{
		enum ParseElemType
		{
			PE_STRING,
			PE_CHECK_ALL_NOT_NULL,
			PE_CHECK_ANY_NULL,
			PE_CHECK_FILE_EXISTS,
			PE_CHECK_FILE_NOT_EXISTS,
			PE_EXECUTE,
			PE_VARIABLE,
			PE_ICONPATH,
			PE_EXTERNAL_VARIABLE,
			PE_EXECUTE2
		} type;

		QString str;
		ParseElem() : type(PE_STRING), str() {}
	};

	static QMap<QString, QString (*)(const UserListElement &)> registeredTags;

	static QString executeCmd(const QString &cmd);

public:

	static QMap<QString, QString> globalVariables;
	static QString parse(const QString &s, const UserListElement &ule, bool escape = true);
	static bool registerTag(const QString &name, QString (*func)(const UserListElement &));
	static bool unregisterTag(const QString &name, QString (*func)(const UserListElement &));

};

#endif
