/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSER_TOKEN_H
#define PARSER_TOKEN_H

#include <QtCore/QString>

struct ParserToken
{
	enum ParserTokenType
	{
		PT_STRING,
		PT_CHECK_ALL_NOT_NULL,
		PT_CHECK_ANY_NULL,
		PT_CHECK_FILE_EXISTS,
		PT_CHECK_FILE_NOT_EXISTS,
		PT_EXECUTE,
		PT_VARIABLE,
		PT_ICONPATH,
		PT_EXTERNAL_VARIABLE,
		PT_EXECUTE2
	} type;

	QString content;
	ParserToken() : type(PT_STRING), content() {}
};

#endif // PARSER_TOKEN_H
