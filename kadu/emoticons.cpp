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
#include <algorithm>
#include <qcursor.h>

#include "emoticons.h"
#include "debug.h"
#include "config_file.h"
#include "kadu-config.h"

// general purpose macros to make things shorter /////////////////
#define VAR(v,x)	__typeof(x) v=x
#define FOREACH(i,c) for(VAR(i, (c).begin()); i!=(c).end(); ++i)
//////////////////////////////////////////////////////////////////


EmoticonsManager::EmoticonsManager()
{
	ThemesList=getSubDirs(dataPath("kadu/themes/emoticons"));
	ThemesList.remove(".");
	walker = NULL;
}

EmoticonsManager::~EmoticonsManager()
{
	if (walker)
		delete walker;
}

QStringList EmoticonsManager::getSubDirs(const QString& path)
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList subdirs=dir.entryList();
	subdirs.remove(".");
	subdirs.remove("..");
	return subdirs;
}

const QStringList& EmoticonsManager::themes()
{
	return ThemesList;
}

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
		}
}

QString EmoticonsManager::getQuoted(const QString& s, unsigned int& pos)
{
	QString r;
	++pos; // eat '"'
	while(s[pos]!="\"")
	{
		r+=s[pos];
		++pos;
	}
	++pos; // eat '"'
	return r;
}

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
}

bool EmoticonsManager::loadGGEmoticonThemePart(QString subdir)
{
	if(subdir!="")
		subdir+="/";
	QString path=themePath()+"/"+subdir;
	QFile theme_file(path+"emots.txt");
	if(!theme_file.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "Error opening emots.txt file\n");
		return false;
	}
	QTextStream theme_stream(&theme_file);
	theme_stream.setCodec(QTextCodec::codecForName("CP1250"));
	while(!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line=theme_stream.readLine();
		unsigned int i=0;
		bool multi=false;
		QStringList aliases;
		if(line[i]=='*')
			++i; // eat '*'
		if(line[i]=='(')
		{
			multi=true;
			++i;
		}
		for(;;)
		{
			aliases.append(getQuoted(line, i));
			if((!multi)||line[i]==')')
				break;
			++i; // eat ','
		}
		if(multi)
			++i; // eat ')'
		++i; // eat ','
		item.anim=subdir+fixFileName(path,getQuoted(line,i));
		if(i<line.length()&&line[i]==',')
		{
			++i; // eat ','
			item.stat=subdir+fixFileName(path,getQuoted(line,i));
		}
		else
			item.stat=item.anim;
		for(i=0; i<aliases.size(); ++i)
		{
			item.alias=aliases[i];
			Aliases.push_back(item);
		}
		item.alias=aliases[0];
		Selector.append(item);
	}
	theme_file.close();
	kdebugf2();
	return true;
}

bool EmoticonsManager::loadGGEmoticonTheme()
{
	Aliases.clear();
	Selector.clear();
	bool something_loaded=false;
	if(loadGGEmoticonThemePart(""))
		something_loaded=true;
	QStringList subdirs=getSubDirs(themePath());
	for(unsigned int i=0; i<subdirs.size(); ++i)
		if(loadGGEmoticonThemePart(subdirs[i]))
			something_loaded=true;

	if ( something_loaded ) {
		// delete previous dictionary of emots
		if ( walker )
			delete walker;
		walker = new EmotsWalker();
		int i = 0;
		// put all emots into dictionary, to allow easy finding
		// their occurrences in text
		FOREACH( item, Aliases )
			walker -> insertString( item -> alias.lower(), i++ );
	}

	return something_loaded;
}

QString EmoticonsManager::themePath()
{
	return dataPath("kadu/themes/emoticons/"+config_file.readEntry("Chat","EmoticonsTheme"));
}

void EmoticonsManager::expandEmoticons(HtmlDocument& doc,const QColor& bgcolor)
{
	kdebugf();

	if(getSubDirs(dataPath("kadu/themes/emoticons")).size()==0)
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "EmoticonsManager::expandEmoticons() end: NO EMOTICONS!\n");
		return;
	}
	
	// check in config if user wants animated emots
	bool animated = (EmoticonsStyle) config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_ANIMATED;

	kdebugm(KDEBUG_INFO, "Expanding emoticons...\n");
	// iterate through parsed html parts of message
	for(int e_i = 0; e_i < doc.countElements(); ++e_i)
	{
		// emots are not expanded in html tags
		if(doc.isTagElement(e_i))
			continue;

		// analyze text of this text part
		QString text = doc.elementText(e_i);
		// variables storing position of last occurrence
		// of emot matching current emots dictionary
		unsigned int lastBegin = 10000;
		int lastEmot = -1;
		// intitialize automata for checking occurrences
		// of emots in text
		walker -> initWalking();
		for(unsigned int j = 0; j < text.length(); ++j)
		{
			// find out if there is some emot occurence when we
			// add current character
			int idx = walker -> checkEmotOccurrence( text[j].lower() );
			// when some emot from dictionary is ending at current character
			if ( idx >= 0 )
				// check if there already was some occurence, whose
				// beginning is before beginning of currently found one
				if ( lastEmot >= 0 && lastBegin < j - Aliases[idx].alias.length() + 1 )
				{
					// if so, then replace that previous occurrence
					// with html tag
					QString new_text="<img emoticon=\"1\" title=\""+Aliases[lastEmot].alias+"\" src=\"";
					if( animated )
						new_text += Aliases[lastEmot].anim;
					else
						new_text += Aliases[lastEmot].stat;
					new_text += QString("\" bgcolor=\"%1\" animated=\"%2\"/>").arg(bgcolor.name()).arg(animated);
					doc.splitElement( e_i, lastBegin, 
					Aliases[lastEmot].alias.length() );
					doc.setElementValue( e_i, new_text, true );
					// our analysis will begin directly after 
					// occurrence of previous emot
					lastEmot = -1;
					break;
				}
				else
				{
					// this is first occurrence in current text part
					lastEmot = idx;
					lastBegin = j - Aliases[lastEmot].alias.length() + 1;
				}
		}
		// this is the case, when only one emot was found in current text part
		if ( lastEmot >= 0 )
		{
			QString new_text="<img emoticon=\"1\" title=\""+Aliases[lastEmot].alias+"\" src=\"";
			if( animated )
				new_text += Aliases[lastEmot].anim;
			else
				new_text += Aliases[lastEmot].stat;
			new_text += QString("\" bgcolor=\"%1\" animated=\"%2\"/>").arg(bgcolor.name()).arg(animated);
			doc.splitElement( e_i, lastBegin, 
			Aliases[lastEmot].alias.length() );
			doc.setElementValue( e_i, new_text, true );
		}
	}
	kdebugm(KDEBUG_DUMP, "Emoticons expanded, html is below:\n%s\n",doc.generateHtml().local8Bit().data());
	kdebugf2();
}

int EmoticonsManager::selectorCount()
{
	return Selector.count();
}

QString EmoticonsManager::selectorString(int emot_num)
{
	return Selector[emot_num].alias;
}

QString EmoticonsManager::selectorAnimPath(int emot_num)
{
	return themePath()+"/"+Selector[emot_num].anim;
}

QString EmoticonsManager::selectorStaticPath(int emot_num)
{
	return themePath()+"/"+Selector[emot_num].stat;
}
				
EmoticonsManager *emoticons;

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
}

void EmoticonSelectorButton::buttonClicked()
{
	emit clicked(EmoticonString);
}

void EmoticonSelectorButton::movieUpdate()
{
	setPixmap(Movie->framePixmap());
}

void EmoticonSelectorButton::enterEvent(QEvent* e)
{
	QToolButton::enterEvent(e);
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_ANIMATED)
		return;
	if(Movie==NULL)
	{
		Movie=new QMovie(AnimPath);
		Movie->connectUpdate(this, SLOT(movieUpdate()));
	}
}

void EmoticonSelectorButton::leaveEvent(QEvent* e)
{
	QToolButton::leaveEvent(e);
	if(Movie!=NULL)
	{
		delete Movie;
		Movie=NULL;
		setPixmap(QPixmap(StaticPath));
	}
}

EmoticonSelector::EmoticonSelector(QWidget *parent, const char *name, Chat * caller) : QWidget (parent, name,Qt::WType_Popup|Qt::WDestructiveClose)
{
	callingwidget = caller;
	
	int selector_count=emoticons->selectorCount();
	int selector_width=(int)sqrt((double)selector_count);
	int btn_width=0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<selector_count; ++i)
	{
		EmoticonSelectorButton* btn = new EmoticonSelectorButton(
			this,emoticons->selectorString(i),
			emoticons->selectorAnimPath(i),
			emoticons->selectorStaticPath(i));
		btn_width=btn->sizeHint().width();
		grid->addWidget(btn, i/selector_width, i%selector_width);
		connect(btn,SIGNAL(clicked(const QString&)),this,SLOT(iconClicked(const QString&)));
	}
}

void EmoticonSelector::closeEvent(QCloseEvent *e) {
	callingwidget->addEmoticon("");
	QWidget::closeEvent(e);
}

void EmoticonSelector::iconClicked(const QString& emoticon_string)
{
	callingwidget->addEmoticon(emoticon_string);
	close();
}

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
}

AnimTextItem::AnimTextItem(
	QTextDocument *p, QTextEdit* edit,
	const QString& filename, const QColor& bgcolor, const QString& tip)
	: QTextCustomItem(p)
{
	FileName=filename;
	text=tip;
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
		(*Movies)[filename].count++;
		kdebugm(KDEBUG_INFO, "Movie %s loaded from cache\n",filename.local8Bit().data());
	}
	else
	{
		md.count=1;
		md.movie=QMovie(filename);
		if(SizeCheckImage==NULL)
			SizeCheckImage=new QImage();
		SizeCheckImage->load(filename);
		md.size=SizeCheckImage->size();
		Movies->insert(filename,md);
		kdebugm(KDEBUG_INFO, "Movie %s loaded from file and cached\n",filename.local8Bit().data());
	}
	//
	Label->setMovie(md.movie);
	width=md.size.width();
	height=md.size.height();
	QToolTip::add(Label, tip);
	Label->resize(md.size);
	Label->setPaletteBackgroundColor(bgcolor);
}

AnimTextItem::~AnimTextItem()
{
	kdebugf();
	delete Label;
	MovieCacheData &md=(*Movies)[FileName];
	md.count--;
	if (md.count==0)
		Movies->remove(FileName);
	kdebugf2();
}

void AnimTextItem::draw(
	QPainter* p, int x, int y, int /*cx*/, int cy,
	int /*cw*/, int /*ch*/, const QColorGroup& /*cg*/,
	bool /*selected*/ )
{
	if(Label->isVisible()&&EditSize==Edit->size())
		return;
	EditSize=Edit->size();

	QPoint u;	
	if (config_file.readBoolEntry("General", "ForceUseParagraphs"))
	{
		//p->fillRect(x, y, width, height, QColor(0,0,0));
		u = QPoint(x, y - cy);
	}
	else
	{
		u = p->xForm(QPoint(x,y));
		if(Edit->contentsY()==0)
			u += Edit->paragraphRect(0).topLeft();
	}
	
	Label->move(u);
	Label->show();
}

QString AnimTextItem::richText() const
{
	return text;
}

QImage* AnimTextItem::SizeCheckImage=NULL;
AnimTextItem::MoviesCache* AnimTextItem::Movies=NULL;

AnimStyleSheet::AnimStyleSheet(
	QTextEdit* parent, const QString& path, const char* name )
	: QStyleSheet(parent, name)
{
	Path=path;
}

QTextCustomItem* AnimStyleSheet::tag(
	const QString& name, const QMap<QString,QString>& attr,
	const QString& context, const QMimeSourceFactory& factory,
	bool emptyTag, QTextDocument* doc) const
{
	if (name!="img")
		return QStyleSheet::tag(name,attr,context,factory,emptyTag,doc);
	if (attr["animated"]=="1")
	{
		if (attr["emoticon"]=="1")
			return new AnimTextItem(doc,(QTextEdit*)parent(),Path+"/"+attr["src"],QColor(attr["bgcolor"]),attr["title"]);
		else
			return new AnimTextItem(doc,(QTextEdit*)parent(),         attr["src"],QColor(attr["bgcolor"]),attr["title"]);
	}
	else
		return QStyleSheet::tag(name,attr,context,factory,emptyTag,doc);
}

/** create fresh emoticons dictionary, which will allow easy finding of occurrences
    of stored emots in text
*/
EmotsWalker::EmotsWalker() 
{
	root = new PrefixNode();
	root -> emotIndex = -1;
	myPair.second = NULL;
}

/** deletes entire dictionary of emots */
EmotsWalker::~EmotsWalker() 
{
	removeChilds( root );
	delete root;
}

/** find node in prefix tree, which is direct successor of given node with
    edge marked by given character
    return NULL if there is none
*/
PrefixNode* EmotsWalker::findChild( PrefixNode* node, const QChar& c ) 
{
	myPair.first = c;
	// create variable 'position' with result of binary search in childs
	// of given node
	VAR( position, std::upper_bound ( node -> childs.begin(), node -> childs.end(), myPair ) );  

	if ( position != node -> childs.end() && position -> first == c )
		return position -> second;
	else
		return NULL;
}

/** add successor to given node with edge marked by given characted
    (building of prefix tree)
*/
PrefixNode* EmotsWalker::insertChild( PrefixNode* node, const QChar& c ) 
{
	PrefixNode* newNode = new PrefixNode();
	newNode -> emotIndex = -1;

	// create child with new node
	VAR( newPair, qMakePair( c, newNode ) );
	// insert new child into childs of current node, performing binary
	// search to find correct position for it
	node -> childs.insert( std::upper_bound( node -> childs.begin(), node -> childs.end(), newPair ), newPair );
	return newNode;
}

/** recursively delete all childs of given node */
void EmotsWalker::removeChilds( PrefixNode* node ) 
{
	FOREACH( ch, node -> childs ) {
		removeChilds( ch -> second );
		delete ch -> second;
	}
}

/** adds given string (emot) to dictionary of emots, giving it
    number, which will be used later to notify occurrences of
    emot in analyzed text
*/
void EmotsWalker::insertString( const QString& str, int num ) 
{
	PrefixNode *child, *node = root;
	unsigned int len = str.length();
	unsigned int pos = 0;
  
	// it adds string to prefix tree character after character
	while ( pos < len ) {
		child = findChild( node, str[pos] );
		if ( child == NULL )
			child = insertChild( node, str[pos] );
		node = child;
		++pos;
	}

	if ( node -> emotIndex == -1 ) 
		node -> emotIndex = num;
}

/** return number of emot, which occurre in analyzed text just
    after adding given character (thus ending on this character)
    beginning of text analysis is turned on by 'initWalking()'
    if no emot occures, -1 is returned
*/
int EmotsWalker::checkEmotOccurrence( const QChar& c ) 
{
	PrefixNode* next;
	int result = -1, resultLen = -1;

	if ( amountPositions < positions.size() ) {
		lengths[amountPositions] = 0;
		positions[amountPositions++] = root;
	}
	else {
		++amountPositions;
		positions.push_back( root );
		lengths.push_back( 0 );
	}

	for (int i = amountPositions - 1; i >= 0; --i) {
		next = findChild( positions[i], c );
		if ( next == NULL ) {
			lengths[i] = lengths[amountPositions - 1];
			positions[i] = positions[--amountPositions];
		}
		else {
			positions[i] = next;
			++lengths[i];
			if ( result == -1 || 
				( next -> emotIndex >= 0 &&
				( next -> emotIndex < result || resultLen < lengths[i] ) ) )
			{
				resultLen = lengths[i];
				result = next -> emotIndex;
			}
		}
	}
	return result;
}

/** clear internal structures responsible for analyzing text, it allows 
    begin of new text analysis
*/
void EmotsWalker::initWalking() 
{
	amountPositions = 0;
}
