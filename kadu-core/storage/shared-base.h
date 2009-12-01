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
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "storage/storable-object.h"

#define KaduSharedBase_PropertyRead(type, name, capitalized_name) \
	type name() const;
#define KaduSharedBase_PropertyWrite(type, name, capitalized_name) \
	void set##capitalized_name(type name) const;
#define KaduSharedBase_Property(type, name, capitalized_name) \
	KaduSharedBase_PropertyRead(type, name, capitalized_name) \
	KaduSharedBase_PropertyWrite(type, name, capitalized_name)

#define KaduSharedBase_PropertyBoolRead(capitalized_name) \
	bool is##capitalized_name() const;
#define KaduSharedBase_PropertyBoolWrite(capitalized_name) \
	void set##capitalized_name(bool name) const;
#define KaduSharedBase_PropertyBool(capitalized_name) \
	KaduSharedBase_PropertyBoolRead(capitalized_name) \
	KaduSharedBase_PropertyBoolWrite(capitalized_name)

#define KaduSharedBase_PropertyReadDef(class_name, type, name, capitalized_name, default) \
	type class_name::name() const\
	{\
		return isNull()\
			? default\
			: data()->name();\
	}

#define KaduSharedBase_PropertyWriteDef(class_name, type, name, capitalized_name, default) \
	void class_name::set##capitalized_name(type name) const\
	{\
		if (!isNull())\
			data()->set##capitalized_name(name);\
	}

#define KaduSharedBase_PropertyDef(class_name, type, name, capitalized_name, default) \
	KaduSharedBase_PropertyReadDef(class_name, type, name, capitalized_name, default) \
	KaduSharedBase_PropertyWriteDef(class_name, type, name, capitalized_name, default)

#define KaduSharedBase_PropertyBoolReadDef(class_name, capitalized_name, default) \
	bool class_name::is##capitalized_name() const\
	{\
		return isNull()\
			? default\
			: data()->is##capitalized_name();\
	}

#define KaduSharedBase_PropertyBoolWriteDef(class_name, capitalized_name, default) \
	void class_name::set##capitalized_name(bool value) const\
	{\
		if (!isNull())\
			data()->set##capitalized_name(value);\
	}

#define KaduSharedBase_PropertyBoolDef(class_name, capitalized_name, default) \
	KaduSharedBase_PropertyBoolReadDef(class_name, capitalized_name, default) \
	KaduSharedBase_PropertyBoolWriteDef(class_name, capitalized_name, default)

template<class T>
class SharedBase
{
	QExplicitlySharedDataPointer<T> Data;

protected:
	void setData(T *data)
	{
		Data = data;
	}

public:
	SharedBase() :
			Data(0)
	{
	}

	explicit SharedBase(T *data) :
			Data(data)
	{
	}

	SharedBase(const SharedBase &copy) :
			Data(copy.Data)
	{
	}

	virtual ~SharedBase()
	{
	}

	operator const QObject * () const // allow using SharedBase classes like QObject *
	{
		return Data.data();
	}

	operator T * () const // allow using SharedBase classes like Shared *
	{
		return Data.data();
	}

	operator bool () const // for ! and ifs
	{
		return Data.data();
	}

	T * data() const
	{
		return Data.data();
	}

	bool isNull() const
	{
		return !Data.data();
	}

	void clone(const SharedBase<T> &copy)
	{
		Data = copy.Data;
	}

	SharedBase<T> & operator = (const SharedBase<T> &copy)
	{
		clone(copy);
		return *this;
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

	void blockUpdatedSignal()
	{
		if (!isNull())
			Data->blockUpdatedSignal();
	}

	void unblockUpdatedSignal()
	{
		if (!isNull())
			Data->unblockUpdatedSignal();
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

	KaduSharedBase_Property(QUuid, uuid, Uuid)

};

template<class T>
KaduSharedBase_PropertyReadDef(SharedBase<T>, QUuid, uuid, Uuid, QUuid());
template<class T>
KaduSharedBase_PropertyWriteDef(SharedBase<T>, QUuid, uuid, Uuid, QUuid());

template<class T>
uint qHash(const SharedBase<T> &sharedBase)
{
	return qHash(sharedBase.uuid().toString());
}

#endif // SHARED_BASE_H
