/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "change-notifier.h"
#include "change-notifier.moc"

ChangeNotifier::ChangeNotifier(QObject *parent) : QObject(parent), Blocked(0), Changed(false)
{
}

ChangeNotifier::~ChangeNotifier()
{
}

void ChangeNotifier::block()
{
    Blocked++;
}

void ChangeNotifier::unblock()
{
    Q_ASSERT(Blocked > 0);

    Blocked--;

    if (0 == Blocked && Changed)
    {
        emit changed();
        Changed = false;
    }
}

void ChangeNotifier::notify()
{
    if (0 == Blocked)
    {
        emit changed();
        return;
    }

    Changed = true;
}

void ChangeNotifier::forget()
{
    Changed = false;
}
