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
	ThemesList=getSubDirs(QString(DATADIR)+"/apps/kadu/themes/emoticons");	ThemesList.remove(".");
};

QStringList EmoticonsManager::getSubDirs(const QString& path)
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList subdirs=dir.entryList();
	subdirs.remove(".");
	subdirs.remove("..");
	return subdirs;
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
		config.emoticons_theme="gadu-gadu";
	if(!loadGGEmoticonTheme())
		if(config.emoticons_theme!="gadu-gadu")
		{
			config.emoticons_theme="gadu-gadu";
			loadGGEmoticonTheme();
		};
};

QString EmoticonsManager::getQuoted(const QString& s,int& pos)
{
	QString r;
	pos++; // eat '"'
	while(s[pos]!="\"")
	{
		r+=s[pos];
		pos++;
	};
	pos++; // eat '"'
	return r;
};

QString EmoticonsManager::fixFileName(const QString& path,const QString& fn)
{
	// sprawd¼ czy oryginalna jest ok
	if(QFile::exists(path+"/"+fn))
		return fn;
	// mo¿e ca³o¶æ lowercase?
	if(QFile::exists(path+"/"+fn.lower()))
		return fn.lower();	
	// rozbij na nazwê i rozszerzenie
	QString name=fn.section('.',0,0);
	QString ext=fn.section('.',1);
	// mo¿e rozszerzenie uppercase?
	if(QFile::exists(path+"/"+name+"."+ext.upper()))
		return name+"."+ext.upper();
	// nie umiemy poprawiæ, zwracamy oryginaln±
	return fn;
};

bool EmoticonsManager::loadGGEmoticonThemePart(QString subdir)
{
	if(subdir!="")
		subdir+="/";
	QString path=themePath()+"/"+subdir;
	QFile theme_file(path+"emots.txt");
	if(!theme_file.open(IO_ReadOnly))
	{
		kdebug("Error opening emots.txt file\n");
		return false;
	};
	QTextStream theme_stream(&theme_file);
	theme_stream.setCodec(QTextCodec::codecForName("CP1250"));
	while(!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line=theme_stream.readLine();
		int i=0;
		bool multi=false;
		QStringList aliases;
		if(line[i]=='*')
			i++; // eat '*'
		if(line[i]=='(')
		{
			multi=true;
			i++;
		};
		for(;;)
		{
			aliases.append(getQuoted(line,i));
			if((!multi)||line[i]==')')
				break;
			i++; // eat ','
		};
		if(multi)
			i++; // eat ')'
		i++; // eat ','
		item.anim=subdir+fixFileName(path,getQuoted(line,i));
		if(i<line.length()&&line[i]==',')
		{
			i++; // eat ','
			item.stat=subdir+fixFileName(path,getQuoted(line,i));
		}
		else
			item.stat=item.anim;
		for(int i=0; i<aliases.size(); i++)
		{
			item.alias=aliases[i];
			Aliases.append(item);
		};
		item.alias=aliases[0];
		Selector.append(item);
	};
	return true;
};

bool EmoticonsManager::loadGGEmoticonTheme()
{
	Aliases.clear();
	Selector.clear();
	bool something_loaded=false;
	if(loadGGEmoticonThemePart(""))
		something_loaded=true;
	QStringList subdirs=getSubDirs(themePath());
	for(int i=0; i<subdirs.size(); i++)
		if(loadGGEmoticonThemePart(subdirs[i]))
			something_loaded=true;
	return something_loaded;
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
		QValueList<EmoticonsListItem>::iterator e=Aliases.end();
		for(QValueList<EmoticonsListItem>::iterator i=Aliases.begin(); i!=Aliases.end(); i++)
		{
			if(text.mid(j,(*i).alias.length())==(*i).alias)
				if(e==Aliases.end()||(*i).alias.length()>(*e).alias.length())
					e=i;
		};
		if(e!=Aliases.end())
		{
			new_text+=QString("__escaped_lt__IMG src=")+(*e).anim+" bgcolor="+bgcolor.name()+"__escaped_gt__";
			j+=(*e).alias.length()-1;
		}
		else
			new_text+=text[j];
	};
	text=new_text;
	kdebug("Emoticons expanded...\n");
};

int EmoticonsManager::selectorCount()
{
	return Selector.count();
};

QString EmoticonsManager::selectorString(int emot_num)
{
	return Selector[emot_num].alias;
};

QString EmoticonsManager::selectorAnimPath(int emot_num)
{
	return themePath()+"/"+Selector[emot_num].anim;
};

QString EmoticonsManager::selectorStaticPath(int emot_num)
{
	return themePath()+"/"+Selector[emot_num].stat;
};
				
EmoticonsManager emoticons;

EmoticonSelectorButton::EmoticonSelectorButton(
	QWidget* parent,const QString& emoticon_string,
	const QString& anim_path,const QString& static_path)
	: QToolButton(parent)
{
	EmoticonString = emoticon_string;
	AnimPath = anim_path;	
	StaticPath = static_path;
	Movie = NULL;
	setPixmap(QPixmap(StaticPath));
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
		Movie=new QMovie(AnimPath);
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
		setPixmap(QPixmap(StaticPath));
	};
};

/* the icon selector itself */
EmoticonSelector::EmoticonSelector(QWidget *parent, const char *name, Chat * caller) : QWidget (parent, name,Qt::WType_Popup)
{
	callingwidget = caller;
	setWFlags(Qt::WDestructiveClose);
	
	int selector_count=emoticons.selectorCount();
	int selector_width=(int)sqrt((double)selector_count);
	int btn_width=0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<selector_count; i++)
	{
		EmoticonSelectorButton* btn = new EmoticonSelectorButton(
			this,emoticons.selectorString(i),
			emoticons.selectorAnimPath(i),
			emoticons.selectorStaticPath(i));
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
