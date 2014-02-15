/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <memory>

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
class qobject_ptr
{

public:
	explicit qobject_ptr(T *qobject = nullptr) noexcept :
			m_qobject{qobject}
	{
	}

	qobject_ptr(const qobject_ptr &) = delete;

	qobject_ptr(qobject_ptr &&moveMe) noexcept :
			m_qobject{nullptr}
	{
		*this = std::move(moveMe);
	}

	~qobject_ptr()
	{
		if (m_qobject && !m_qobject->parent())
			delete m_qobject;
	}

	qobject_ptr & operator = (const qobject_ptr &) = delete;

	qobject_ptr & operator = (qobject_ptr &&moveMe) noexcept
	{
		if (this == std::addressof(moveMe))
			return *this;

		using std::swap;
		swap(m_qobject, moveMe.m_qobject);
		return *this;
	}

	T * operator -> () const noexcept
	{
		return m_qobject;
	}

	typename std::add_lvalue_reference<T>::type operator * () const
	{
		return *m_qobject;
	}

	T * get() const noexcept
	{
		return m_qobject;
	}

private:
	T *m_qobject;

};

template<typename T, typename ...Args>
qobject_ptr<T> make_qobject(Args&& ...args)
{
	return qobject_ptr<T>(new T(std::forward<Args>(args)...));
}
