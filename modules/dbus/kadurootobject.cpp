/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "gadu.h"
#include "kadu.h"

#include "kadurootobject.h"

KaduRootObject::KaduRootObject(QObject *parent) : QObject(parent)
{
	if (gadu)
		Status = gadu->currentStatus();
}

KaduRootObject::~KaduRootObject()
{
}

QString KaduRootObject::Version()
{
	return "Kadu " + kadu->version();
}

void KaduRootObject::Disconnect()
{
	Status = gadu->currentStatus();
	kadu->setOffline(Status.description());
}

void KaduRootObject::Reconnect()
{
	kadu->setStatus(Status);
	Status = UserStatus();
}

void KaduRootObject::Show()
{
	kadu->show();
}

void KaduRootObject::Hide()
{
	kadu->hide();
}

void KaduRootObject::Quit()
{
	kadu->quit();
}
