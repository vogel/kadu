/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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
