/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "emoticons.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

EmoticonsManager::EmoticonsManager()
{
	setEmoticonsStyle("");
};

void EmoticonsManager::setEmoticonsStyle(const QString& style)
{
	CurrentStyle=style;
	loadEmoticonsRegexpList();
	loadEmoticonsSelectorList();
};

void EmoticonsManager::loadEmoticonsRegexpList()
{
	QFile emoticons_file(QString(DATADIR)+"/apps/kadu/images/"+CurrentStyle+"/emoticons_regexp");
	emoticons_file.open(IO_ReadOnly);
	QTextStream emoticons_stream(&emoticons_file);
	EmoticonsRegexpList.clear();
	QString regexp;
	while(!(regexp=emoticons_stream.readLine()).isNull())
	{
		QString picname=emoticons_stream.readLine();
		EmoticonsRegexpListItem item;
		item.regexp=regexp;
		item.picname=picname;
		EmoticonsRegexpList.append(item);
	};
};

void EmoticonsManager::loadEmoticonsSelectorList()
{
	QFile emoticons_file(QString(DATADIR)+"/apps/kadu/images/"+CurrentStyle+"/emoticons_selector");
	emoticons_file.open(IO_ReadOnly);
	QTextStream emoticons_stream(&emoticons_file);
	EmoticonsSelectorList.clear();
	QString string;
	while(!(string=emoticons_stream.readLine()).isNull())
	{
		QString picname=emoticons_stream.readLine();
		EmoticonsSelectorListItem item;
		item.string=string;
		item.picname=picname;
		EmoticonsSelectorList.append(item);
	};
};

void EmoticonsManager::expandEmoticons(QString& text)
{
	for(QValueList<EmoticonsRegexpListItem>::iterator i=EmoticonsRegexpList.begin(); i!=EmoticonsRegexpList.end(); i++)
		text.replace(QRegExp((*i).regexp),QString("__escaped_lt__IMG SRC=")+(*i).picname+" /__escaped_gt__");
};

int EmoticonsManager::emoticonsCount()
{
	return EmoticonsSelectorList.count();
};

QString EmoticonsManager::emoticonString(int emot_num)
{
	return EmoticonsSelectorList[emot_num].string;	
};

QString EmoticonsManager::emoticonPicName(int emot_num)
{
	return EmoticonsSelectorList[emot_num].picname;
};
				
EmoticonsManager emoticons;
