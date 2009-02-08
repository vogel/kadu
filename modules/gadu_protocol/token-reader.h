/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOKEN_READER_H
#define TOKEN_READER_H

class QPixmap;
class QString;

class TokenReader
{
public:
	virtual QString readToken(const QPixmap &tokenPixmap) = 0;

};

#endif // TOKEN_READER_H
