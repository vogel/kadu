/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHARED_BASE_H
#define SHARED_BASE_H

#include <QtCore/QExplicitlySharedDataPointer>

template<class T>
class SharedBase
{
	QExplicitlySharedDataPointer<T> Data;

protected:
	virtual void connectDataSignals() {}
	virtual void disconnectDataSignals() {}

public:
	explicit SharedBase(bool null = false) :
			Data(null ? 0 : new T())
	{
		connectDataSignals();
	}

	explicit SharedBase(T *data) :
			Data(data)
	{
		connectDataSignals();
	}

	SharedBase(const SharedBase &copy) :
			Data(copy.Data)
	{
		connectDataSignals();
	}

	virtual ~SharedBase()
	{
		disconnectDataSignals();
	}

	T * data() const
	{
		return Data.data();
	}

	bool isNull() const
	{
		return Data.data();
	}

	SharedBase<T> & operator = (const SharedBase<T> &copy)
	{
		disconnectDataSignals();
		Data = copy.Data;
		connectDataSignals();
	}

	bool operator == (const SharedBase<T> &compare) const
	{
		return Data == compare.Data;
	}

	bool operator != (const SharedBase<T> &compare) const
	{
		return Data != compare.Data;
	}

	int operator < (const SharedBase<T> &compare) const
	{
		return Data < compare.Data;
	}

	void store()
	{
		if (!isNull())
			Data->store();
	}

	void removeFromStorage()
	{
		if (!isNull())
			Data->removeFromStorage();
	}

};

#endif // SHARED_BASE_H
