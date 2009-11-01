/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <QtCore/QMap>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI Parser
{
	typedef QString (*ObjectTagCallback)(const QObject * const);

	static QMap<QString, QString (*)(const Contact &)> registeredTags;
	static QMap<QString, ObjectTagCallback> registeredObjectTags;

	static QString executeCmd(const QString &cmd);

public:
	static QMap<QString, QString> globalVariables;
	static QString parse(const QString &s, const QObject * const object, bool escape = true);
	static QString parse(const QString &s, Account account, const Contact &contact, bool escape = true);
	static QString parse(const QString &s, Account account, const Contact &contact, const QObject * const object, bool escape = true);
	static bool registerTag(const QString &name, QString (*func)(const Contact &));
	static bool unregisterTag(const QString &name, QString (*func)(const Contact &));

	static bool registerObjectTag(const QString &name, ObjectTagCallback);
	static bool unregisterObjectTag(const QString &name, ObjectTagCallback);

};

#endif // PARSER_H
