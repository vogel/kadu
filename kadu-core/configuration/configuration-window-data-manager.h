/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGURATION_WINDOW_DATA_MANAGER_H
#define CONFIGURATION_WINDOW_DATA_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "exports.h"

class KADUAPI ConfigurationWindowDataManager : public QObject
{
	Q_OBJECT

public:
	explicit ConfigurationWindowDataManager(QObject *parent = 0) : QObject(parent) {}

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value) = 0;
	virtual QVariant readEntry(const QString &section, const QString &name) = 0;

};

#endif // CONFIGURATION_WINDOW_DATA_MANAGER_H
