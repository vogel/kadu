/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_IMAGE_SERVICE_H
#define CHAT_IMAGE_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class KADUAPI ChatImageService : public QObject
{
	Q_OBJECT

signals:
	virtual void imageReceived(const QString &id, const QString &fileName);

};

#endif // CHAT_IMAGE_SERVICE_H
