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
#include <qtooltip.h>
#include <math.h>

#include "emoticons.h"
#include "debug.h"
#include "config_dialog.h"

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
	emoticons_stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
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

void EmoticonsManager::expandEmoticons(QString& text,const QColor& bgcolor)
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
				new_text+=QString("__escaped_lt__IMG src=")+(*i).picname+" bgcolor="+bgcolor.name()+"__escaped_gt__";
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

EmoticonSelectorButton::EmoticonSelectorButton(
	QWidget* parent,const QString& emoticon_string,
	const QString& file_path)
	: QToolButton(parent)
{
	EmoticonString = emoticon_string;
	EmoticonPath = file_path;
	Movie=NULL;
	setPixmap(QPixmap(EmoticonPath));
	setAutoRaise(true);
	setMouseTracking(true);
	QToolTip::add(this,emoticon_string);
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
};

void EmoticonSelectorButton::buttonClicked()
{
	emit clicked(EmoticonString);
};

void EmoticonSelectorButton::movieUpdate()
{
	setPixmap(Movie->framePixmap());
};

void EmoticonSelectorButton::enterEvent(QEvent* e)
{
	QToolButton::enterEvent(e);
	if(Movie==NULL)
	{
		Movie=new QMovie(EmoticonPath);
		Movie->connectUpdate(this, SLOT(movieUpdate()));
	};
};

void EmoticonSelectorButton::leaveEvent(QEvent* e)
{
	QToolButton::leaveEvent(e);
	if(Movie!=NULL)
	{
		delete Movie;
		Movie=NULL;
		setPixmap(QPixmap(EmoticonPath));
	};
};

/* the icon selector itself */
EmoticonSelector::EmoticonSelector(QWidget *parent, const char *name, Chat * caller) : QWidget (parent, name,Qt::WType_Popup)
{
	callingwidget = caller;
	setWFlags(Qt::WDestructiveClose);
	
	int emoticons_count=emoticons.emoticonsCount();
	int selector_width=(int)sqrt((double)emoticons_count);
	int btn_width=0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<emoticons_count; i++)
	{
		EmoticonSelectorButton* btn = new EmoticonSelectorButton(this,emoticons.emoticonString(i),emoticons.emoticonPicPath(i));
		btn_width=btn->sizeHint().width();
		grid->addWidget(btn, i/selector_width, i%selector_width);
		connect(btn,SIGNAL(clicked(const QString&)),this,SLOT(iconClicked(const QString&)));
	};
};

void EmoticonSelector::closeEvent(QCloseEvent *e) {
	callingwidget->addEmoticon("");
	QWidget::closeEvent(e);
};

void EmoticonSelector::iconClicked(const QString& emoticon_string)
{
	callingwidget->addEmoticon(emoticon_string);
	close();
};

AnimTextItem::AnimTextItem(
	QTextDocument *p, QTextEdit* edit,
	const QString& filename, const QColor& bgcolor )
	: QTextCustomItem(p)
{
	Edit=edit;
	Label=new QLabel(Edit->viewport());
	Edit->addChild(Label);
	Label->setMovie(QMovie(filename));
	if(SizeCheckImage==NULL)
		SizeCheckImage=new QImage();
	SizeCheckImage->load(filename);
	width=SizeCheckImage->width();
	height=SizeCheckImage->height();
	Label->resize(SizeCheckImage->size());
	Label->setPaletteBackgroundColor(bgcolor);
};

AnimTextItem::~AnimTextItem()
{
	delete Label;
};

void AnimTextItem::draw(
	QPainter* p, int x, int y, int cx, int cy,
	int cw, int ch, const QColorGroup& cg,
	bool selected )
{
//	p->fillRect(x,y,width,height,Label->paletteBackgroundColor());
	if(Label->isVisible()&&EditSize==Edit->size())
		return;
	EditSize=Edit->size();
	QPoint u=p->xForm(QPoint(x,y));
	if(Edit->contentsY()==0)
		u+=Edit->paragraphRect(0).topLeft();
	Label->move(u);
	Label->show();
};

QImage* AnimTextItem::SizeCheckImage=NULL;

AnimStyleSheet::AnimStyleSheet(
	QTextEdit* parent, const QString& path, const char* name )
	: QStyleSheet(parent, name)
{
	Path=path;
};

QTextCustomItem* AnimStyleSheet::tag(
	const QString& name, const QMap<QString,QString>& attr,
	const QString& context, const QMimeSourceFactory& factory,
	bool emptyTag, QTextDocument* doc) const
{
	if(name!="img")
		return QStyleSheet::tag(name,attr,context,factory,emptyTag,doc);
	return new AnimTextItem(doc,(QTextEdit*)parent(),Path+"/"+attr["src"],QColor(attr["bgcolor"]));
};
