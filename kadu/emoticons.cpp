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
#include "config_file.h"

EmoticonsManager::EmoticonsManager()
{
	ThemesList=getSubDirs(QString(DATADIR)+"/kadu/themes/emoticons");	ThemesList.remove(".");
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
		config_file.writeEntry("Chat","EmoticonsTheme",theme);
	else
		config_file.writeEntry("Chat","EmoticonsTheme","gadu-gadu");
	if(!loadGGEmoticonTheme())
		if(config_file.readEntry("Chat","EmoticonsTheme")!="gadu-gadu")
		{
			config_file.writeEntry("Chat","EmoticonsTheme","gadu-gadu");
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
			aliases.append(getQuoted(line, i));
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
	return QString(DATADIR)+"/kadu/themes/emoticons/"+config_file.readEntry("Chat","EmoticonsTheme");
};

void EmoticonsManager::expandEmoticons(HtmlDocument& doc,const QColor& bgcolor)
{
	kdebug("Expanding emoticons...\n");
	for(int e_i=0; e_i<doc.countElements(); e_i++)
	{
		if(doc.isTagElement(e_i)) continue;
		QString text=doc.elementText(e_i);
		for(int j=0; j<text.length(); j++)
		{
			QValueList<EmoticonsListItem>::iterator e=Aliases.end();
			for(QValueList<EmoticonsListItem>::iterator i=Aliases.begin(); i!=Aliases.end(); i++)
			{
				if(text.mid(j,(*i).alias.length()).lower()==(*i).alias.lower())
					if(e==Aliases.end()||(*i).alias.length()>(*e).alias.length())
						e=i;
			};
			if(e!=Aliases.end())
			{
				QString new_text="<IMG src=\"";
				if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
					new_text+=(*e).anim;
				else
					new_text+=(*e).stat;			
				new_text+="\" bgcolor="+bgcolor.name()+">";
				doc.splitElement(e_i,j,(*e).alias.length());
				doc.setElementValue(e_i,new_text,true);
				break;
			}
		};
	};
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
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_ANIMATED)
		return;
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

void EmoticonSelector::alignTo(QWidget* w)
{
	// oblicz pozycjê widgetu do którego równamy
	QPoint w_pos = w->mapToGlobal(QPoint(0,0));
	// oblicz rozmiar selektora
	QSize e_size = sizeHint();
	// oblicz rozmiar pulpitu
	QSize s_size = QApplication::desktop()->size();
	// oblicz dystanse od widgetu do lewego brzegu i do prawego
	int l_dist = w_pos.x();
	int r_dist = s_size.width() - (w_pos.x() + w->width());
	// oblicz pozycjê w zale¿no¶ci od tego czy po lewej stronie
	// jest wiêcej miejsca czy po prawej
	int x;
	if (l_dist >= r_dist)
		x = w_pos.x() - e_size.width();
	else
		x = w_pos.x() + w->width();
	// oblicz pozycjê y - centrujemy w pionie
	int y = w_pos.y() + w->height()/2 - e_size.height()/2;
	// je¶li wychodzi poza doln± krawêd¼ to równamy do niej
	if (y + e_size.height() > s_size.height())
		y = s_size.height() - e_size.height();
	// je¶li wychodzi poza górn± krawêd¼ to równamy do niej
	if (y < 0)
		y = 0;
	// ustawiamy selektor na wyliczonej pozycji
	move(x, y);
};

AnimTextItem::AnimTextItem(
	QTextDocument *p, QTextEdit* edit,
	const QString& filename, const QColor& bgcolor )
	: QTextCustomItem(p)
{
	Edit=edit;
	Label=new QLabel(Edit->viewport());
	Edit->addChild(Label);
	//
	MovieCacheData md;
	if(Movies==NULL)
		Movies=new MoviesCache();
	if(Movies->contains(filename))
	{
		md=(*Movies)[filename];
		kdebug("Movie %s loaded from cache\n",filename.local8Bit().data());
	}
	else
	{
		md.movie=QMovie(filename);
		if(SizeCheckImage==NULL)
			SizeCheckImage=new QImage();
		SizeCheckImage->load(filename);
		md.size=SizeCheckImage->size();
		Movies->insert(filename,md);
		kdebug("Movie %s loaded from file and cached\n",filename.local8Bit().data());
	};
	//
	Label->setMovie(md.movie);
	width=md.size.width();
	height=md.size.height();
	Label->resize(md.size);
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
AnimTextItem::MoviesCache* AnimTextItem::Movies=NULL;

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
