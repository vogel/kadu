/*
 * %kadu copyright begin%
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

#include "error.h"

Error::Error(ErrorSeverity severity, const QString &message) : Severity(severity), Message(message)
{
}

Error::Error(const Error &copyMe)
{
    Severity = copyMe.Severity;
    Message = copyMe.Message;
}

Error &Error::operator=(const Error &copyMe)
{
    Severity = copyMe.Severity;
    Message = copyMe.Message;

    return *this;
}

ErrorSeverity Error::severity() const
{
    return Severity;
}

QString Error::message() const
{
    return Message;
}
