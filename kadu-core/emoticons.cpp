/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextedit.h>
#include <qtooltip.h>

#include <algorithm>
#include <math.h>

#include "chat.h"
#include "debug.h"
#include "emoticons.h"
#include "kadu_text_browser.h"
#include "misc.h"

#define IMG_X_OFFSET 0
#define IMG_Y_OFFSET 2

EmoticonsManager::EmoticonsListItem::EmoticonsListItem() : alias(), anim(), stat()
{
}

EmoticonsManager::EmoticonsManager() : ThemesList(getSubDirs(dataPath("kadu/themes/emoticons"))),
		Aliases(), Selector(), walker(0)

{
	ThemesList.remove(".");
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

const QStringList& EmoticonsManager::themes() const
{
	return ThemesList;
}

void EmoticonsManager::setEmoticonsTheme(const QString& theme)
{
	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "theme: %s\n", theme.local8Bit().data());
	if (ThemesList.contains(theme))
		config_file.writeEntry("Chat", "EmoticonsTheme", theme);
	else
		config_file.writeEntry("Chat", "EmoticonsTheme", "penguins");
	if (!loadGGEmoticonTheme())
	{
		config_file.writeEntry("Chat", "EmoticonsTheme", "penguins");
		if (!loadGGEmoticonTheme() && ThemesList.size() > 0)
		{
			config_file.writeEntry("Chat", "EmoticonsTheme", ThemesList[0]);
			loadGGEmoticonTheme();
		}
	}
	kdebugf2();
}

QString EmoticonsManager::getQuoted(const QString& s, unsigned int& pos)
{
	QString r;
	++pos; // eat '"'

	int pos2 = s.find('"', pos);
	if (pos2 >= 0)
	{
		r = s.mid(pos, uint(pos2) - pos);
		pos = uint(pos2) + 1;// eat '"'
	}
	else
	{
		r = s.mid(pos);
		pos = s.length();
	}
	return r;
}

QString EmoticonsManager::fixFileName(const QString& path,const QString& fn)
{
	// sprawd¼ czy oryginalna jest ok
	if (QFile::exists(path + '/' + fn))
		return fn;
	// mo¿e ca³o¶æ lowercase?
	if (QFile::exists(path + '/' + fn.lower()))
		return fn.lower();
	// rozbij na nazwê i rozszerzenie
	QString name = fn.section('.', 0, 0);
	QString ext = fn.section('.', 1);
	// mo¿e rozszerzenie uppercase?
	if (QFile::exists(path + '/' + name + '.' + ext.upper()))
		return name + '.' + ext.upper();
	// nie umiemy poprawiæ, zwracamy oryginaln±
	return fn;
}

bool EmoticonsManager::loadGGEmoticonThemePart(QString subdir)
{
	kdebugmf(KDEBUG_FUNCTION_START, "subdir: %s\n", subdir.local8Bit().data());
	if (!subdir.isEmpty())
		subdir += '/';
	QString path = themePath() + '/' + subdir;
	QFile theme_file(path + "emots.txt");
	if (!theme_file.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "Error opening emots.txt file\n");
		return false;
	}
	QTextStream theme_stream(&theme_file);
	theme_stream.setCodec(codec_cp1250);
	while (!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line = theme_stream.readLine();
		kdebugm(KDEBUG_DUMP, "> %s\n", line.local8Bit().data());
		unsigned int lineLength = line.length();
		unsigned int i = 0;
		bool multi = false;
		QStringList aliases;
		if (i < lineLength && line[i] == '*')
			++i; // eat '*'
		if (i < lineLength && line[i] == '(')
		{
			multi = true;
			++i;
		}
		for(;;)
		{
			aliases.append(getQuoted(line, i));
			if (!multi || i >= lineLength || line[i] == ')')
				break;
			++i; // eat ','
		}
		if (multi)
			++i; // eat ')'
		++i; // eat ','
		item.anim = subdir + fixFileName(path, getQuoted(line, i));
		if (i < lineLength && line[i] == ',')
		{
			++i; // eat ','
			item.stat = subdir + fixFileName(path, getQuoted(line, i));
		}
		else
			item.stat = item.anim;
		CONST_FOREACH(alias, aliases)
		{
			item.alias = *alias;
			Aliases.push_back(item);
		}
		item.alias = aliases[0];
		Selector.append(item);
	}
	theme_file.close();
	kdebugf2();
	return true;
}

bool EmoticonsManager::loadGGEmoticonTheme()
{
	kdebugf();
	Aliases.clear();
	Selector.clear();
	bool something_loaded = false;
	if (loadGGEmoticonThemePart(QString::null))
		something_loaded = true;
	QStringList subdirs = getSubDirs(themePath());
	CONST_FOREACH(subdir, subdirs)
		if (loadGGEmoticonThemePart(*subdir))
			something_loaded = true;

	if ( something_loaded ) {
		// delete previous dictionary of emots
		if ( walker )
			delete walker;
		walker = new EmotsWalker();
		int i = 0;
		// put all emots into dictionary, to allow easy finding
		// their occurrences in text
		CONST_FOREACH( item, Aliases )
			walker -> insertString( item -> alias.lower(), i++ );
	}
	kdebugmf(KDEBUG_FUNCTION_END | KDEBUG_INFO, "loaded: %d\n", something_loaded);
	return something_loaded;
}

QString EmoticonsManager::themePath() const
{
	return dataPath("kadu/themes/emoticons/" + config_file.readEntry("Chat","EmoticonsTheme"));
}

void EmoticonsManager::expandEmoticons(HtmlDocument& doc, const QColor& bgcolor, EmoticonsStyle style)
{
	kdebugf();

	static bool emotsFound = false;
	const static QString emotTemplate("<img emoticon=\"1\" title=\"%1\" src=\"%2\" bgcolor=\"%3\" animated=\"%4\"/>");

	if (!walker)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: EMOTICONS NOT LOADED!\n");
		return;
	}

	if (!emotsFound && getSubDirs(dataPath("kadu/themes/emoticons")).isEmpty())
	{
		fprintf(stderr, "no emoticons in %s\n", dataPath("kadu/themes/emoticons").local8Bit().data());
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: NO EMOTICONS!\n");
		return;
	}
	emotsFound = true;

	// check in config if user wants animated emots
	bool animated = style == EMOTS_ANIMATED;

	kdebugm(KDEBUG_INFO, "Expanding emoticons...\n");
	// iterate through parsed html parts of message
	for(int e_i = 0; e_i < doc.countElements(); ++e_i)
	{
		// emots are not expanded in html tags
		if (doc.isTagElement(e_i))
			continue;

		// analyze text of this text part
		QString text = doc.elementText(e_i).lower();
		// variables storing position of last occurrence
		// of emot matching current emots dictionary
		unsigned int lastBegin = 10000;
		int lastEmot = -1;
		// intitialize automata for checking occurrences
		// of emots in text
		walker -> initWalking();
		for(unsigned int j = 0, textlength = text.length(); j < textlength; ++j)
		{
			// find out if there is some emot occurence when we
			// add current character
			int idx = walker -> checkEmotOccurrence( text[j] );
			// when some emot from dictionary is ending at current character
			if ( idx >= 0 )
				// check if there already was some occurence, whose
				// beginning is before beginning of currently found one
				if ( lastEmot >= 0 && lastBegin < j - Aliases[idx].alias.length() + 1 )
				{
					// if so, then replace that previous occurrence
					// with html tag
					QString new_text;
					if (animated)
						new_text = narg(emotTemplate, Aliases[lastEmot].alias, Aliases[lastEmot].anim, bgcolor.name(), QString::number(animated));
					else
						new_text = narg(emotTemplate, Aliases[lastEmot].alias, Aliases[lastEmot].stat, bgcolor.name(), QString::number(animated));

					doc.splitElement( e_i, lastBegin, Aliases[lastEmot].alias.length() );
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
			QString new_text;
			if (animated)
				new_text = narg(emotTemplate, Aliases[lastEmot].alias, Aliases[lastEmot].anim, bgcolor.name(), QString::number(animated));
			else
				new_text = narg(emotTemplate, Aliases[lastEmot].alias, Aliases[lastEmot].stat, bgcolor.name(), QString::number(animated));

			doc.splitElement( e_i, lastBegin, Aliases[lastEmot].alias.length() );
			doc.setElementValue( e_i, new_text, true );
		}
	}
	kdebugm(KDEBUG_DUMP, "Emoticons expanded, html is below:\n%s\n",doc.generateHtml().local8Bit().data());
	kdebugf2();
}

int EmoticonsManager::selectorCount() const
{
	return Selector.count();
}

QString EmoticonsManager::selectorString(int emot_num) const
{
	if (emot_num >= 0 && uint(emot_num) < Selector.count())
		return Selector[emot_num].alias;
	else
		return QString::null;
}

QString EmoticonsManager::selectorAnimPath(int emot_num) const
{
	if (emot_num >= 0 && uint(emot_num) < Selector.count())
		return themePath() + '/' + Selector[emot_num].anim;
	else
		return QString::null;
}

QString EmoticonsManager::selectorStaticPath(int emot_num) const
{
	if (emot_num >= 0 && uint(emot_num) < Selector.count())
		return themePath() + '/' + Selector[emot_num].stat;
	else
		return QString::null;
}

EmoticonsManager *emoticons;

EmoticonSelectorButton::EmoticonSelectorButton(
	QWidget* parent, const QString& emoticon_string,
	const QString& anim_path, const QString& static_path)
	: QToolButton(parent), EmoticonString(emoticon_string),
		AnimPath(anim_path), StaticPath(static_path), Movie(0)
{
	setPixmap(QPixmap(StaticPath));
	setAutoRaise(true);
	setMouseTracking(true);
	QToolTip::add(this,emoticon_string);
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

EmoticonSelectorButton::~EmoticonSelectorButton()
{
	if (Movie)
	{
		delete Movie;
		Movie = NULL;
	}
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
	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") != EMOTS_ANIMATED)
		return;
	if (Movie == NULL)
	{
		Movie = new QMovie(AnimPath);
		Movie->connectUpdate(this, SLOT(movieUpdate()));
	}
}

void EmoticonSelectorButton::leaveEvent(QEvent* e)
{
	QToolButton::leaveEvent(e);
	if (Movie != NULL)
	{
		delete Movie;
		Movie = NULL;
		setPixmap(QPixmap(StaticPath));
	}
}

EmoticonSelector::EmoticonSelector(QWidget *parent, const char *name, Chat * caller) :
	QWidget (parent, name, Qt::WType_Popup | Qt::WDestructiveClose), callingwidget(caller)
{
	int selector_count = emoticons->selectorCount();
	int selector_width = (int)sqrt((double)selector_count);
	int btn_width = 0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i = 0; i < selector_count; ++i)
	{
		EmoticonSelectorButton* btn = new EmoticonSelectorButton(
			this, emoticons->selectorString(i),
			emoticons->selectorAnimPath(i),
			emoticons->selectorStaticPath(i));
		btn_width = btn->sizeHint().width();
		grid->addWidget(btn, i / selector_width, i % selector_width);
		connect(btn, SIGNAL(clicked(const QString&)), this, SLOT(iconClicked(const QString&)));
	}
}

void EmoticonSelector::closeEvent(QCloseEvent *e)
{
	callingwidget->addEmoticon(QString::null);
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

#include <qdragobject.h>
#include <qpaintdevicemetrics.h>
#include <qglobal.h>
#include <qfeatures.h>

#define Q_DUMMY_COMPARISON_OPERATOR(C)

static inline bool is_printer( QPainter *p )
{
	if ( !p || !p->device() )
		return FALSE;
	return p->device()->devType() == QInternal::Printer;
}

static inline int scale( int value, QPainter *painter )
{
	if ( is_printer( painter ) ) {
		QPaintDeviceMetrics metrics( painter->device() );
#if defined(Q_WS_X11)
		value = value * metrics.logicalDpiY() / QPaintDevice::x11AppDpiY( painter->device()->x11Screen() );
#elif defined (Q_WS_WIN)
		HDC hdc = GetDC( 0 );
		int gdc = GetDeviceCaps( hdc, LOGPIXELSY );
		if ( gdc )
			value = value * metrics.logicalDpiY() / gdc;
		ReleaseDC( 0, hdc );
#elif defined (Q_WS_MAC)
		value = value * metrics.logicalDpiY() / 75; // ##### FIXME
#elif defined (Q_WS_QWS)
		value = value * metrics.logicalDpiY() / 75;
#endif
	}
	return value;
}

struct QPixmapInt
{
	QPixmapInt() : pm(), ref( 0 ) {}
	QPixmap pm;
	int	    ref;
	Q_DUMMY_COMPARISON_OPERATOR(QPixmapInt)
};

static QMap<QString, QPixmapInt> *pixmap_map = 0;

StaticTextItem::StaticTextItem(QTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
			QMimeSourceFactory &factory)
    : QTextCustomItem(p), reg(0), pm(), place(PlaceInline), tmpwidth(0), tmpheight(0), attributes(attr), imgId()
{
	width = height = 0;
	if ( attr.contains("width") )
		width = attr["width"].toInt();
	if ( attr.contains("height") )
		height = attr["height"].toInt();

	QString imageName = attr["src"];

	if (!imageName)
		imageName = attr["source"];

	if ( !imageName.isEmpty() )
	{
		imgId = QString( "%1,%2,%3,%4" ).arg( imageName ).arg( width ).arg( height ).arg( (ulong)&factory );
		if ( !pixmap_map )
			pixmap_map = new QMap<QString, QPixmapInt>;
		if ( pixmap_map->contains( imgId ) ) {
			QPixmapInt& pmi = pixmap_map->operator[](imgId);
			pm = pmi.pm;
			pmi.ref++;
			width = pm.width();
			height = pm.height();
		} else {
			QImage img;
			const QMimeSource* m = factory.data( imageName, context );
			if ( !m ) {
				qWarning("StaticTextItem: no mimesource for %s", imageName.latin1() );
			}
			else {
				if ( !QImageDrag::decode( m, img ) ) {
					qWarning("StaticTextItem: cannot decode %s", imageName.latin1() );
				}
			}

			if ( !img.isNull() ) {
				if ( width == 0 ) {
					width = img.width();
					if ( height != 0 ) {
						width = img.width() * height / img.height();
					}
				}
				if ( height == 0 ) {
					height = img.height();
					if ( width != img.width() ) {
						height = img.height() * width / img.width();
					}
				}
				if ( img.width() != width || img.height() != height ){
#ifndef QT_NO_IMAGE_SMOOTHSCALE
					img = img.smoothScale(width, height);
#endif
					width = img.width();
					height = img.height();
				}
				pm.convertFromImage( img );
			}
			if ( !pm.isNull() ) {
				QPixmapInt& pmi = pixmap_map->operator[](imgId);
				pmi.pm = pm;
				pmi.ref++;
			}
		}
		if ( pm.mask() ) {
			QRegion mask( *pm.mask() );
			QRegion all( 0, 0, pm.width(), pm.height() );
			reg = new QRegion( all.subtract( mask ) );
		}
    }

	if ( pm.isNull() && (width*height)==0 )
		width = height = 50;

	if ( attr["align"] == "left" )
		place = PlaceLeft;
	else if ( attr["align"] == "right" )
		place = PlaceRight;

	tmpwidth = width;
	tmpheight = height;
}

StaticTextItem::~StaticTextItem()
{
	if ( pixmap_map && pixmap_map->contains( imgId ) ) {
		QPixmapInt& pmi = pixmap_map->operator[](imgId);
		pmi.ref--;
		if ( !pmi.ref ) {
			pixmap_map->remove( imgId );
			if ( pixmap_map->isEmpty() ) {
				delete pixmap_map;
				pixmap_map = 0;
			}
		}
	}
	delete reg;
}

QString StaticTextItem::richText() const
{
	QMap<QString, QString>::ConstIterator it=attributes.find("title");
	if (it!=attributes.end())
		return *it;
	it=attributes.find("src");
	if (it!=attributes.end())
		return *it;

	QString s;
	s += "<img ";
	it = attributes.begin();
	for ( ; it != attributes.end(); ++it ) {
		s += it.key() + '=';
		if ( (*it).find( ' ' ) != -1 )
			s += '"' + *it + "\" ";
		else
			s += *it + ' ';
	}
	s += '>';
	return s;
}

void StaticTextItem::adjustToPainter( QPainter* p )
{
	width = scale( tmpwidth, p );
	height = scale( tmpheight, p );
}

#if !defined(Q_WS_X11)
#include <qbitmap.h>
#include <qcleanuphandler.h>
static QPixmap *qrt_selection = 0;
static QSingleCleanupHandler<QPixmap> qrt_cleanup_pixmap;
static void qrt_createSelectionPixmap( const QColorGroup &cg )
{
	qrt_selection = new QPixmap( 2, 2 );
	qrt_cleanup_pixmap.set( &qrt_selection );
	qrt_selection->fill( Qt::color0 );
	QBitmap m( 2, 2 );
	m.fill( Qt::color1 );
	QPainter p( &m );
	p.setPen( Qt::color0 );
	for ( int j = 0; j < 2; ++j ) {
		p.drawPoint( j % 2, j );
	}
	p.end();
	qrt_selection->setMask( m );
	qrt_selection->fill( cg.highlight() );
}
#endif

void StaticTextItem::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
//	kdebugm(KDEBUG_DUMP, "x:%d, y:%d, cx:%d, cy:%d, cw:%d, ch:%d, placement:%d, PlaceInline:%d, xpos:%d, ypos:%d\n",
//		x, y, cx, cy, cw, ch, placement(), PlaceInline, xpos, ypos);
	if ( placement() != PlaceInline ) {
		x = xpos;
		y = ypos;
	}
//	for (int i=QColorGroup::Foreground; i<QColorGroup::NColorRoles; i++)
//		kdebugm(KDEBUG_INFO, "%s\n", cg.color((QColorGroup::ColorRole)i).name().local8Bit().data());

	if ( pm.isNull() ) {
		pm=QPixmap(width, height);
		pm.fill(cg.base());
//		p->fillRect( x , y, width, height,  cg.base() );
//		return;
	}

	if ( is_printer( p ) ) {
		p->drawPixmap( QRect( x, y, width, height ), pm );
		return;
	}

	if ( placement() != PlaceInline && !QRect( xpos, ypos, width, height ).intersects( QRect( cx, cy, cw, ch ) ) )
		return;

	if ( placement() == PlaceInline )
		p->drawPixmap( x , y + (!attributes["src"].isEmpty() ? IMG_Y_OFFSET : 0), pm );
	else
		p->drawPixmap( cx , cy + (!attributes["src"].isEmpty() ? IMG_Y_OFFSET : 0), pm, cx - x, cy - y, cw, ch );

	if ( selected && placement() == PlaceInline && is_printer( p ) ) {
#if defined(Q_WS_X11)
		p->fillRect( QRect( QPoint( x, y ), pm.size() ), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
#else // in WIN32 Dense4Pattern doesn't work correctly (transparency problem), so work around it
		if ( !qrt_selection )
			qrt_createSelectionPixmap( cg );
		p->drawTiledPixmap( x, y, pm.width(), pm.height(), *qrt_selection );
#endif
	}
}

AnimTextItem::MovieCacheData::MovieCacheData(const QString &fileName) : movie(fileName), size(), count(1), runCount(0)
{
}

bool AnimatedLabel::mustPause = false;
AnimatedLabel::AnimatedLabel(AnimTextItem::MovieCacheData *data, const QString &tip, bool imageBackground,
	QScrollView *view, bool trueTransparency, const char *name) : QLabel(view->viewport(), name/*, WNoAutoErase*/),
	movieData(data), scrollView(view), tip(tip), imageBackground(imageBackground), paused(true), lastX(0), lastY(0),
	trueTransparency(trueTransparency)
{
//	kdebugf();
	setMovie(movieData->movie);
	if (movieData->runCount == 0)
		movieData->movie.pause();
	if (trueTransparency)
		setBackgroundMode(Qt::NoBackground);
//	kdebugf2();
}

AnimatedLabel::~AnimatedLabel()
{
	pauseMovie();
}

void AnimatedLabel::unpauseMovie()
{
	if (paused && !mustPause)
	{
		++movieData->runCount;
		movie()->unpause();
		paused = false;
	}
}

void AnimatedLabel::pauseMovie()
{
	if (!paused)
	{
		if (--movieData->runCount == 0)
			movie()->pause();
		paused = true;
	}
}

void AnimatedLabel::paintEvent(QPaintEvent *e)
{
//	kdebugf();
//	kdebugm(KDEBUG_INFO, "tip:%15s, count:%2d, runCount:%5d, y:%4d, visH:%4d, contY:%4d, contH:%4d, heigth:%2d %d %d\n", tip.local8Bit().data(), movieData->count, movieData->runCount, y(), scrollView->visibleHeight(), scrollView->contentsY(), scrollView->contentsHeight(), height(), y() > scrollView->visibleHeight(), y() + height() < 0);
	if (y() > scrollView->visibleHeight() || y() + height() < 0)
	{
//		kdebugm(KDEBUG_INFO, "hiding %s\n", tip.local8Bit().data());
		pauseMovie();
		hide();
	}
	else
	{
		if (mustPause)
			pauseMovie();
		else
			unpauseMovie();

		if (trueTransparency)
		{
			const QBitmap *mask = movie()->framePixmap().mask();
			if (mask)
				setMask(*mask);

			QLabel::paintEvent(e);
		}
		else
		{
			QPainter paint;
			setUpdatesEnabled(false);
			paint.begin(this);

			const QPixmap *bg = static_cast<QWidget *>(parent())->backgroundPixmap();
			if (bg)
				paint.drawTiledPixmap(0, 0, width(), height(), *bg, (lastX%bg->width()), (lastY%bg->height()));

			paint.drawPixmap(0, 0, movie()->framePixmap());

			paint.end();
			setUpdatesEnabled(true);
		}
	}
//	kdebugm(KDEBUG_INFO, "tip: %s\n", tip.local8Bit().data());
//	kdebugf2();
}

AnimTextItem::AnimTextItem(
	QTextDocument *p, QTextEdit* edit,
	const QString& filename, const QColor& bgcolor, const QString& tip)
	: QTextCustomItem(p), Edit(edit), Label(0),
	EditSize(), text(tip), FileName(filename)

{
	MovieCacheData *md;
	if (Movies == NULL)
		Movies = new MoviesCache();
	if (Movies->contains(filename))
	{
		md = (*Movies)[filename];
		++md->count;
		kdebugm(KDEBUG_INFO, "Movie %s loaded from cache\n", filename.local8Bit().data());
	}
	else
	{
		md = new MovieCacheData(filename);
		if (SizeCheckImage == NULL)
			SizeCheckImage = new QImage();

		SizeCheckImage->load(filename);
		md->size = SizeCheckImage->size();
		Movies->insert(filename, md);
		kdebugm(KDEBUG_INFO, "Movie %s loaded from file and cached\n", filename.local8Bit().data());
	}
	bool imageBG = edit->paper().pixmap() != 0;
	Label = new AnimatedLabel(md, tip, imageBG, edit, static_cast<KaduTextBrowser*>(Edit)->isTrueTransparencyEnabled());
	Edit->addChild(Label);

	width = md->size.width();
	height = md->size.height();
	QToolTip::add(Label, tip);
	Label->resize(md->size);
	if (!imageBG)
		Label->setPaletteBackgroundColor(bgcolor);
	Label->hide();
}

AnimTextItem::~AnimTextItem()
{
	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, " %p\n", Movies);
	delete Label;
	MovieCacheData *md = (*Movies)[FileName];
	--md->count;
	if (md->count == 0)
	{
		Movies->remove(FileName);
		delete md;
	}
	if (Movies->isEmpty())
	{
		delete SizeCheckImage;
		delete Movies;
		Movies = NULL;
		SizeCheckImage = NULL;
	}
	kdebugf2();
}

void AnimTextItem::draw(
	QPainter* /*p*/, int x, int y, int cx, int cy,
	int cw, int ch, const QColorGroup& /*cg*/,
	bool /*selected*/ )
{
//	kdebugm(KDEBUG_WARNING, "%s x:%d y:%d cx:%d cy:%d cw:%d ch:%d\n", text.local8Bit().data(), x, y, cx, cy, cw, ch);
//	kdebugm(KDEBUG_WARNING, "contX:%d contY:%d contW:%d contH:%d visW:%d visH:%d\n", Edit->contentsX(), Edit->contentsY(), Edit->contentsWidth(), Edit->contentsHeight(), Edit->visibleWidth(), Edit->visibleHeight());
//	if(EditSize==Edit->size())
	if (EditSize == Edit->size()  &&  ch != Edit->visibleHeight())
	{
//		kdebugm(KDEBUG_WARNING, "back\n\n");
		return;
	}
//	else
//		kdebugm(KDEBUG_WARNING, "\n");
//	if(Label->isVisible()&&EditSize==Edit->size())
//		return;

	EditSize = Edit->size();

	Label->lastX = x;
	if (x - cx > 0)
		Label->lastX = x - cx + Edit->visibleWidth() - cw + IMG_X_OFFSET;
	else
		Label->lastX = x - cx + IMG_X_OFFSET;


	// +IMG_Y_OFFSET dla lepszego efektu optycznego - emotikony s± bardziej wy¶rodkowane
	if (y - cy > 0)
		Label->lastY = y - cy + Edit->visibleHeight() - ch + IMG_Y_OFFSET;
	else
		Label->lastY = y - cy + IMG_Y_OFFSET;
	Label->move(Label->lastX, Label->lastY);

	Label->lastX += Edit->contentsX();
	Label->lastY += Edit->contentsY();
//	kdebugm(KDEBUG_WARNING, "%s, lastX:%d, lastY:%d\n", text.local8Bit().data(), Label->lastX, Label->lastY);


/*	QPoint u(x, y - cy);
	if (u.y() > 0)
		u += QPoint(0, Edit->visibleHeight() - ch);

//	Edit->moveChild(Label, u.x(), u.y());
	Label->move(u);*/

	if (Label->movieData->movie.framePixmap().isNull() && !Label->movieData->movie.running())
		Label->movieData->movie.step();
	Label->unpauseMovie();
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
	: QStyleSheet(parent, name), Path(path)
{
}

QTextCustomItem* AnimStyleSheet::tag(
	const QString& name, const QMap<QString,QString>& attr,
	const QString& context, const QMimeSourceFactory& factory,
	bool emptyTag, QTextDocument* doc) const
{
	if (name != "img")
		return QStyleSheet::tag(name, attr, context, factory, emptyTag, doc);
	if (attr["animated"] == "1")
	{
		if (attr["emoticon"] == "1")
			return new AnimTextItem(doc, (QTextEdit*)parent(), Path + '/' + attr["src"], QColor(attr["bgcolor"]), attr["title"]);
		else
			return new AnimTextItem(doc, (QTextEdit*)parent(),              attr["src"], QColor(attr["bgcolor"]), attr["title"]);
	}
	else
		return new StaticTextItem(doc, attr, context, (QMimeSourceFactory&)factory);
//		return QStyleSheet::tag(name,attr,context,factory,emptyTag,doc);
}

StaticStyleSheet::StaticStyleSheet(
	QTextEdit* parent, const QString& path, const char* name)
	: QStyleSheet(parent, name), Path(path)
{
}

QTextCustomItem* StaticStyleSheet::tag(
	const QString& name, const QMap<QString,QString>& attr,
	const QString& context, const QMimeSourceFactory& factory,
	bool emptyTag, QTextDocument* doc) const
{
	if (name != "img")
		return QStyleSheet::tag(name,attr,context,factory,emptyTag,doc);
	return new StaticTextItem(doc, attr, context, (QMimeSourceFactory&)factory);
}

PrefixNode::PrefixNode() : emotIndex(-1), childs()
{
}

/** create fresh emoticons dictionary, which will allow easy finding of occurrences
    of stored emots in text
*/
EmotsWalker::EmotsWalker() : root(new PrefixNode()), myPair(), positions(), lengths(), amountPositions(0)
{
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
PrefixNode* EmotsWalker::findChild( const PrefixNode* node, const QChar& c )
{
	myPair.first = c;
	// create variable 'position' with result of binary search in childs
	// of given node
	VAR( position, std::upper_bound ( node -> childs.constBegin(), node -> childs.constEnd(), myPair ) );

	if ( position != node -> childs.constEnd() && position -> first == c )
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
	CONST_FOREACH( ch, node -> childs ) {
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
	const PrefixNode* next;
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
			lengths[i] = lengths[--amountPositions];
			positions[i] = positions[amountPositions];
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
