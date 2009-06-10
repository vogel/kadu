/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_BUTTON_H
#define STATUS_BUTTON_H

#include <QtGui/QPushButton>

#include "configuration/configuration-aware-object.h"
#include "protocols/status.h"

class StatusButton : public QPushButton, private ConfigurationAwareObject
{
	Q_OBJECT

private slots:
	void statusChanged(Status status);

protected:
	virtual void configurationUpdated();

public:
	explicit StatusButton(QIcon icon, const QString &text, QWidget *parent = 0);

};

#endif // STATUS_BUTTON_H
