/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class Hint;

class HintRepository : public QObject
{
	Q_OBJECT

	using Storage = std::vector<Hint *>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = Storage::iterator;

	Q_INVOKABLE explicit HintRepository(QObject *parent = nullptr);
	virtual ~HintRepository();

	bool isEmpty() const { return m_hints.empty(); }

	void addHint(Hint *hint);
	void removeHint(Hint *hint);

	Iterator begin() { return std::begin(m_hints); }
	Iterator end() { return std::end(m_hints); }

private:
	Storage m_hints;

};

inline HintRepository::Iterator begin(HintRepository *hintRepository)
{
	return hintRepository->begin();
}

inline HintRepository::Iterator end(HintRepository *hintRepository)
{
	return hintRepository->end();
}
