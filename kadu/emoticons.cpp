/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qtextcodec.h>

#include "emoticons.h"
#include "debug.h"

// dla config
#include "kadu.h"

EmoticonsManager::EmoticonsManager()
{
	ThemesList=QDir(QString(DATADIR)+"/apps/kadu/themes/emoticons").entryList();
	ThemesList.remove(".");
	ThemesList.remove("..");
};

const QStringList& EmoticonsManager::themes()
{
	return ThemesList;
};

void EmoticonsManager::setEmoticonsTheme(const QString& theme)
{
	if(ThemesList.contains(theme))
		config.emoticons_theme=theme;
	else
		config.emoticons_theme="kadubis";
	loadEmoticonsRegexpList();
	loadEmoticonsSelectorList();
};

void EmoticonsManager::loadEmoticonsRegexpList()
{
	EmoticonsRegexpList.clear();
	QFile emoticons_file(themePath()+"/emoticons_regexp");
	if(!emoticons_file.open(IO_ReadOnly))
	{
		kdebug("Error opening emoticons_regexp file\n");
		return;
	};
	QTextStream emoticons_stream(&emoticons_file);
	emoticons_stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	QString regexp;
	while(!emoticons_stream.atEnd())
	{
		regexp=emoticons_stream.readLine();
		if(regexp==""||regexp[0]=='#') continue;
		QString picname=emoticons_stream.readLine();
		EmoticonsRegexpListItem item;
		item.regexp=QRegExp(regexp);
		item.picname=picname;
		bool added=false;
		for(QValueList<EmoticonsRegexpListItem>::iterator i=EmoticonsRegexpList.begin(); i!=EmoticonsRegexpList.end(); i++)
			if(regexp.length()>=(*i).regexp.pattern().length())
			{
				EmoticonsRegexpList.insert(i,item);
				added=true;
				break;
			};
		if(!added)
			EmoticonsRegexpList.append(item);
		kdebug("EMOTICON REGEXP: %s=%s\n",(const char*)regexp.local8Bit(),(const char*)picname.local8Bit());
	};
};

void EmoticonsManager::loadEmoticonsSelectorList()
{
	EmoticonsSelectorList.clear();
	QFile emoticons_file(themePath()+"/emoticons_selector");
	if(!emoticons_file.open(IO_ReadOnly))
	{
		kdebug("Error opening emoticons_selector file\n");
		return;
	};
	QTextStream emoticons_stream(&emoticons_file);
	QString string;
	while(!emoticons_stream.atEnd())
	{
		string=emoticons_stream.readLine();
		if(string==""||string[0]=='#') continue;	
		QString picname=emoticons_stream.readLine();
		EmoticonsSelectorListItem item;
		item.string=string;
		item.picname=picname;
		EmoticonsSelectorList.append(item);
	};
};

QString EmoticonsManager::themePath()
{
	return QString(DATADIR)+"/apps/kadu/themes/emoticons/"+config.emoticons_theme;
};

void EmoticonsManager::expandEmoticons(QString& text)
{
	QString new_text;
	kdebug("Expanding emoticons...\n");
	for(int j=0; j<text.length(); j++)
	{
		bool emoticonFound=false;
		for(QValueList<EmoticonsRegexpListItem>::iterator i=EmoticonsRegexpList.begin(); i!=EmoticonsRegexpList.end(); i++)
		{
			if((*i).regexp.search(text,j)==j)
			{
				new_text+=QString("__escaped_lt__IMG SRC=")+(*i).picname+"__escaped_gt__";
				j+=(*i).regexp.matchedLength()-1;
				emoticonFound=true;
			};
		};
		if(!emoticonFound)
			new_text+=text[j];
	};
	text=new_text;
	kdebug("Emoticons expanded...\n");
};

int EmoticonsManager::emoticonsCount()
{
	return EmoticonsSelectorList.count();
};

QString EmoticonsManager::emoticonString(int emot_num)
{
	return EmoticonsSelectorList[emot_num].string;	
};

QString EmoticonsManager::emoticonPicPath(int emot_num)
{
	return QString(DATADIR)+"/apps/kadu/themes/emoticons/"+config.emoticons_theme+"/"+EmoticonsSelectorList[emot_num].picname;
};
				
EmoticonsManager emoticons;
