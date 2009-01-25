/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AWARE_OBJECT
#define AWARE_OBJECT

#include <QtCore/QList>

#include "exports.h"

#define KADU_AWARE_CLASS(class) template<> QList<class *> AwareObject<class>::Objects = QList<class *>();

template<class T>
class KADUAPI AwareObject
{

protected:
	static QList<T *> Objects;

public:
	AwareObject() { Objects.append(reinterpret_cast<T *>(this)); }
	virtual ~AwareObject() { Objects.remove(reinterpret_cast<T *>(this)); }

};

#endif // CONFIGURATION_AWARE_OBJECT
