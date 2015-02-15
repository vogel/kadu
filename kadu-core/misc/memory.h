/*
 * %kadu copyright begin%
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <cassert>
#include <memory>

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
struct not_owned_qobject_deleter
{
	void operator() (T *qobject)
	{
		if (!qobject)
			return;

		assert(qobject->parent() == nullptr);
		if (!qobject->parent())
			delete qobject;
	}
};

template<typename T>
class not_owned_qptr : public std::unique_ptr<T, not_owned_qobject_deleter<T>>
{

public:
	not_owned_qptr()
	{
	}

	not_owned_qptr(T *qobject) :
			std::unique_ptr<T, not_owned_qobject_deleter<T>>(qobject)
	{
		assert(qobject->parent() == nullptr);
	}
};

template<typename T, typename ...Args>
not_owned_qptr<T> make_not_owned(Args&& ...args)
{
	auto result = not_owned_qptr<T>(new T(std::forward<Args>(args)...));
	assert(result->parent() == nullptr);
	return result;
}

template<typename T>
struct owned_qobject_deleter
{
	void operator() (T *qobject)
	{
		if (!qobject)
			return;

		// assert(qobject->parent() != nullptr); could be already deleted here
		// parent will delete this object when needed
	}
};

template<typename T>
class owned_qptr : public std::unique_ptr<T, owned_qobject_deleter<T>>
{

public:
	owned_qptr()
	{
	}

	owned_qptr(T *qobject) :
			std::unique_ptr<T, owned_qobject_deleter<T>>(qobject)
	{
		assert(qobject->parent() != nullptr);
	}
};

template<typename T, typename ...Args>
owned_qptr<T> make_owned(Args&& ...args)
{
	auto result = owned_qptr<T>(new T(std::forward<Args>(args)...));
	assert(result->parent() != nullptr);
	return result;
}
