/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <memory>

class QLockFile;
class QString;

class LongLivedLockFile
{
public:
	explicit LongLivedLockFile(const QString &fileName, int timeout = 10);
	LongLivedLockFile(const LongLivedLockFile &) = delete;
	~LongLivedLockFile();

	LongLivedLockFile & operator = (const LongLivedLockFile &) = delete;

	bool isLocked() const;

private:
	std::unique_ptr<QLockFile> m_lockFile;

};
