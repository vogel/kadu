/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PREDICATE_OBJECT
#define PREDICATE_OBJECT

#include <QtCore/QList>

#include "exports.h"

#define KADU_PREDICATE_CLASS(class) template<> QList<class *> PredicateObject<class>::Objects = QList<class *>();

template<class T>
class KADUAPI PredicateObject
{

protected:
	static QList<T *> Objects;

public:
	PredicateObject() { Objects.append(reinterpret_cast<T *>(this)); }
	virtual ~PredicateObject() { Objects.removeAll(reinterpret_cast<T *>(this)); }

};

#endif // PREDICATE_OBJECT
