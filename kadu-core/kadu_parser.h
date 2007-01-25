#ifndef KADU_PARSER_H
#define KADU_PARSER_H

#include <qmap.h>
#include <qstring.h>

class UserListElement;

namespace KaduParser
{
	extern QMap<QString, QString> globalVariables;

	QString parse(const QString &s, const UserListElement &ule, bool escape = true);

	bool registerTag(const QString &name, QString (*func)(const UserListElement &));

	bool unregisterTag(const QString &name, QString (*func)(const UserListElement &));
}

#endif
