#ifndef KADU_PARSER_H
#define KADU_PARSER_H

#include <qglobal.h>

#include <qmap.h>
#include <qstring.h>

class QObject;
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

	typedef QString (*ObjectTagCallback)(const QObject * const);

	static QMap<QString, QString (*)(const UserListElement &)> registeredTags;
	static QMap<QString, ObjectTagCallback> registeredObjectTags;

	static QString executeCmd(const QString &cmd);

public:

	static QMap<QString, QString> globalVariables;
	static QString parse(const QString &s, const UserListElement &ule, bool escape = true);
	static QString parse(const QString &s, const UserListElement &ule, const QObject * const object, bool escape = true);
	static bool registerTag(const QString &name, QString (*func)(const UserListElement &));
	static bool unregisterTag(const QString &name, QString (*func)(const UserListElement &));

	static bool registerObjectTag(const QString &name, ObjectTagCallback);
	static bool unregisterObjectTag(const QString &name, ObjectTagCallback);

};

#endif
