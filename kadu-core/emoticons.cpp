/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <QtGui/QPainter>
#include <QtGui/QScrollBar>
#include <QtGui/QVBoxLayout>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include <math.h>
#include <algorithm>

#include "configuration/configuration-file.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"

#include "debug.h"
#include "misc/misc.h"

#include "emoticons.h"

#define IMG_Y_OFFSET 2

EmoticonsManager * EmoticonsManager::Instance = 0;

EmoticonsManager * EmoticonsManager::instance()
{
	if (Instance == 0)
		Instance = new EmoticonsManager();
	return Instance;
}

EmoticonsManager::EmoticonsListItem::EmoticonsListItem()
{
}

EmoticonsManager::EmoticonsManager() :
		Themes("emoticons", "emots.txt"), Aliases(), Selector(), walker(0)

{
	kdebugf();

	setPaths(config_file.readEntry("Chat", "EmoticonsPaths").split(QRegExp("(;|:|&)"), QString::SkipEmptyParts));
	setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	kdebugf2();
}

EmoticonsManager::~EmoticonsManager()
{
	if (walker)
		delete walker;
}

void EmoticonsManager::configurationUpdated()
{
	kdebugf();

	setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	kdebugf2();
}

void EmoticonsManager::setEmoticonsTheme(const QString &theme)
{
	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "theme: %s\n", qPrintable(theme));

	QStringList themeList = themes();
	if (themeList.contains(theme))
	{
		config_file.writeEntry("Chat", "EmoticonsTheme", theme);
		setTheme(theme);
	}
	else
	{
		config_file.writeEntry("Chat", "EmoticonsTheme", "penguins");
		setTheme("penguins");
	}

	if (!loadGGEmoticonTheme())
	{
		config_file.writeEntry("Chat", "EmoticonsTheme", "penguins");
		if (!loadGGEmoticonTheme() && (themeList.size() > 0))
		{
			config_file.writeEntry("Chat", "EmoticonsTheme", themeList[0]);
			loadGGEmoticonTheme();
		}
	}

	kdebugf2();
}

QString EmoticonsManager::getQuoted(const QString &s, unsigned int &pos)
{
	QString r;
	++pos; // eat '"'

	int pos2 = s.indexOf('"', pos);
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

bool EmoticonsManager::loadGGEmoticonThemePart(const QString &subdir)
{
	kdebugmf(KDEBUG_FUNCTION_START, "subdir: %s\n", qPrintable(subdir));

	QString dir = subdir;

	if (!dir.isEmpty() && !dir.endsWith("/"))
		dir += '/';
	QString path = themePath() + '/' + dir;
	QFile theme_file(path + ConfigName);
	if (!theme_file.open(QIODevice::ReadOnly))
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "Error opening %s file\n",
			qPrintable(theme_file.fileName()));
		return false;
	}
	QTextStream theme_stream(&theme_file);
	theme_stream.setCodec(codec_cp1250);
	while (!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line = theme_stream.readLine();
		kdebugm(KDEBUG_DUMP, "> %s\n", qPrintable(line));
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
		item.anim = path + fixFileName(path, getQuoted(line, i));
		if (i < lineLength && line[i] == ',')
		{
			++i; // eat ','
			item.stat = path + fixFileName(path, getQuoted(line, i));
		}
		else
			item.stat = item.anim;

		foreach(const QString &alias, aliases)
		{
			item.alias = alias;
			item.escapedAlias = alias;
			HtmlDocument::escapeText(item.escapedAlias);
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
	foreach(const QString &subdir, subdirs)
		if (loadGGEmoticonThemePart(subdir))
			something_loaded = true;

	if (something_loaded) {
		// delete previous dictionary of emots
		if (walker)
			delete walker;
		walker = new EmotsWalker();
		int i = 0;
		// put all emots into dictionary, to allow easy finding
		// their occurrences in text
		foreach(const EmoticonsListItem &item, Aliases)
			walker->insertString(item.alias.toLower(), i++);
	}

	kdebugmf(KDEBUG_FUNCTION_END | KDEBUG_INFO, "loaded: %d\n", something_loaded);
	return something_loaded;
}

void EmoticonsManager::expandEmoticons(HtmlDocument &doc, EmoticonsStyle style)
{
	kdebugf();

	if (EmoticonsStyleNone == style)
		return;

	static bool emotsFound = false;
	const static QString emotTemplate("<img emoticon=\"1\" alt=\"%1\" title=\"%1\" src=\"file:///%2\" />");

	if (!walker)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: EMOTICONS NOT LOADED!\n");
		return;
	}

	if (!emotsFound && getSubDirs(dataPath("kadu/themes/emoticons")).isEmpty())
	{
		fprintf(stderr, "no emoticons in %s\n", qPrintable(dataPath("kadu/themes/emoticons")));
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: NO EMOTICONS!\n");
		return;
	}
	emotsFound = true;

	// check in config if user wants animated emots
	bool animated = style == EmoticonsStyleAnimated;

	kdebugm(KDEBUG_INFO, "Expanding emoticons...\n");
	// iterate through parsed html parts of message
	for(int e_i = 0; e_i < doc.countElements(); ++e_i)
	{
		// emots are not expanded in html tags
		if (doc.isTagElement(e_i))
			continue;

		// analyze text of this text part
		QString text = doc.elementText(e_i).toLower();
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
			int idx = walker -> checkEmotOccurrence(text[j]);
			// when some emot from dictionary is ending at current character
			if (idx >= 0)
			{
				// check if there already was some occurence, whose
				// beginning is before beginning of currently found one
				if (lastEmot >= 0 && lastBegin < j - Aliases[idx].alias.length() + 1)
				{
					// if so, then replace that previous occurrence
					// with html tag
					QString new_text;
					new_text = narg(emotTemplate, Aliases[lastEmot].escapedAlias, animated ? Aliases[lastEmot].anim : Aliases[lastEmot].stat);

					doc.splitElement(e_i, lastBegin, Aliases[lastEmot].alias.length());
					doc.setElementValue(e_i, new_text, true);
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
		}
		// this is the case, when only one emot was found in current text part
		if (lastEmot >= 0)
		{
			QString new_text;
			new_text = narg(emotTemplate, Aliases[lastEmot].escapedAlias, animated ? Aliases[lastEmot].anim : Aliases[lastEmot].stat);

			doc.splitElement(e_i, lastBegin, Aliases[lastEmot].alias.length());
			doc.setElementValue(e_i, new_text, true);
		}
	}
	kdebugm(KDEBUG_DUMP, "Emoticons expanded, html is below:\n%s\n", qPrintable(doc.generateHtml()));
	kdebugf2();
}

int EmoticonsManager::selectorCount() const
{
	return Selector.count();
}

QString EmoticonsManager::selectorString(int emot_num) const
{
	if ((emot_num >= 0) && (emot_num < Selector.count()))
		return Selector[emot_num].alias;
	else
		return QString::null;
}

QString EmoticonsManager::selectorAnimPath(int emot_num) const
{
	if ((emot_num >= 0 && (emot_num)) < (Selector.count()))
		return Selector[emot_num].anim;
	else
		return QString::null;
}

QString EmoticonsManager::selectorStaticPath(int emot_num) const
{
	if ((emot_num >= 0) && ((emot_num) < Selector.count()))
		return Selector[emot_num].stat;
	else
		return QString::null;
}

EmoticonSelectorButton::EmoticonSelectorButton(const QString &emoticon_string, const QString &anim_path, const QString &static_path, QWidget *parent) :
		QLabel(parent), EmoticonString(emoticon_string), AnimPath(anim_path), StaticPath(static_path)
{
	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") != EmoticonsStyleAnimated)
		AnimPath = StaticPath;

	QPixmap p(StaticPath);
	if (((EmoticonsScaling)config_file.readNumEntry("Chat","EmoticonsScaling") & EmoticonsScalingStatic) && (p.height() > 18))
		p = p.scaledToHeight(18, Qt::SmoothTransformation);
	setPixmap(p);
	setMouseTracking(true);
	setMargin(4);
	setFixedSize(sizeHint());
}

void EmoticonSelectorButton::buttonClicked()
{
	emit clicked(EmoticonString);
}

void EmoticonSelectorButton::mouseMoveEvent(QMouseEvent *e)
{
	QLabel::mouseMoveEvent(e);
	MovieViewer *viewer = new MovieViewer(this);
	connect(viewer, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	viewer->show();
}

EmoticonSelectorButton::MovieViewer::MovieViewer(EmoticonSelectorButton *parent) :
		QLabel(parent, Qt::Popup)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(parent->sizeHint());
	setAlignment(Qt::AlignCenter);
	setMouseTracking(true);
	setToolTip(parent->EmoticonString);

	QString style =
		"QLabel {"
		"	background-color: #fbe333;"
		"	padding: 4px;"
		"}";
	setStyleSheet(style);

	QMovie *movie = new QMovie(parent->AnimPath);
	setMovie(movie);
	if((EmoticonsScaling)config_file.readNumEntry("Chat","EmoticonsScaling") & EmoticonsScalingAnimated)
		movie->setScaledSize(parent->pixmap()->size());
	movie->start();

	// center on parent
	QPoint new_pos = parent->mapToGlobal(QPoint(0, 0));
	new_pos += QPoint(parent->sizeHint().width() / 2, parent->sizeHint().height() / 2);
	new_pos -= QPoint(sizeHint().width() / 2, sizeHint().height() / 2);
	move(new_pos);
}

void EmoticonSelectorButton::MovieViewer::mouseMoveEvent(QMouseEvent *e)
{
	QLabel::mouseMoveEvent(e);
	if (!rect().contains(e->globalPos() - mapToGlobal(QPoint(0, 0))))
		close();
}

void EmoticonSelectorButton::MovieViewer::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);
	emit clicked();
}

EmoticonSelector::EmoticonSelector(const QWidget *activatingWidget, QWidget *parent) :
		QScrollArea(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Popup);
	setFrameStyle(QFrame::NoFrame);

	int selector_count = EmoticonsManager::instance()->selectorCount();
	if (selector_count == 0)
	{
		close();
		return;
	}

	QWidget *mainwidget = new QWidget(this);

	addEmoticonButtons(selector_count, mainwidget);
	setWidget(mainwidget);
	calculatePositionAndSize(activatingWidget, mainwidget);
}

void EmoticonSelector::addEmoticonButtons(int num_emoticons, QWidget *mainwidget)
{
	int selector_width = 460;
	int total_height = 0, cur_width = 0, btn_width = 0;
	EmoticonSelectorButton *btns[num_emoticons];
	QVBoxLayout *layout = new QVBoxLayout(mainwidget);
	QHBoxLayout *row = 0;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	for (int i = 0; i < num_emoticons; ++i)
	{
		btns[i] = new EmoticonSelectorButton(
			EmoticonsManager::instance()->selectorString(i),
			EmoticonsManager::instance()->selectorAnimPath(i),
			EmoticonsManager::instance()->selectorStaticPath(i),
			mainwidget);
		btn_width = btns[i]->sizeHint().width();

		if (cur_width + btn_width >= selector_width)
			cur_width = 0;

		if (cur_width == 0)
			total_height += btns[i]->sizeHint().height() + 1;
		cur_width += btn_width;

		connect(btns[i], SIGNAL(clicked(const QString &)), this, SLOT(iconClicked(const QString &)));
	}

	if (total_height < selector_width - 80)
		selector_width = sqrt(selector_width * total_height) * 1.1;
	else if (total_height > selector_width + 40)
		selector_width += 40;

	cur_width = 0;
	for (int i = 0; i < num_emoticons; ++i)
	{
		btn_width = btns[i]->sizeHint().width();

		if (cur_width + btn_width >= selector_width)
			cur_width = 0;

		if (cur_width == 0)
		{
			row = new QHBoxLayout;
			layout->addLayout(row);
		}

		row->addWidget(btns[i]);
		cur_width += btn_width;
	}
	if (row)
		row->setAlignment(Qt::AlignLeft); // align the last row to left
}

void EmoticonSelector::calculatePositionAndSize(const QWidget *activatingWidget, const QWidget *mainwidget)
{
	QPoint w_pos = activatingWidget->mapToGlobal(QPoint(0,0));
	QSize s_size = QApplication::desktop()->size();
	QSize e_size = mainwidget->sizeHint();

	bool is_on_left;
	bool hscroll_needed = false;
	int x, max_width;
	int width = e_size.width();
	int hscrollbar_height = horizontalScrollBar()->sizeHint().height();
	int vscrollbar_width = verticalScrollBar()->sizeHint().width();

	// if the distance to the left edge of the screen equals or is bigger than the distance to the right edge
	if (w_pos.x() >= s_size.width() - (w_pos.x() + activatingWidget->width()))
	{
		is_on_left = true;
		x = w_pos.x() - e_size.width();
		max_width = w_pos.x();
		if (x < 0)
		{
			width = w_pos.x();
			x = 0;
			hscroll_needed = true;
		}
	}
	else
	{
		is_on_left = false;
		x = w_pos.x() + activatingWidget->width();
		max_width = s_size.width() - x;
		if (x + e_size.width() > s_size.width())
		{
			width = s_size.width() - (w_pos.x() + activatingWidget->width());
			hscroll_needed = true;
		}
	}

	int height = e_size.height() + (hscroll_needed ? hscrollbar_height : 0);
	// center vertically
	int y = w_pos.y() + activatingWidget->height()/2 - height/2;
	// if we exceed the bottom edge of the screen, let's align the widget to it
	if (y + height > s_size.height())
		y = s_size.height() - height;
	// if we exceed the top edge of the screen, let's align the widget to it
	if (y < 0)
		y = 0;
	if (height > s_size.height())
	{
		height = s_size.height();

		// due to vertical scrollbar added, width of the widget has been increased, so we have to reposition it
		int add_width = vscrollbar_width;
		if (width + add_width > max_width)
			add_width = max_width - width;
		width += add_width;
		if (is_on_left)
			x -= add_width;
	}

	setFixedSize(QSize(width, height));
	move(x, y);
}

void EmoticonSelector::iconClicked(const QString &emoticon_string)
{
	emit emoticonSelect(emoticon_string);
	close();
}

bool EmoticonSelector::event(QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress && !rect().contains(static_cast<QMouseEvent*>(e)->globalPos() - mapToGlobal(QPoint(0, 0))))
	{
		close();
		return true;
	}
	return QScrollArea::event(e);
}

PrefixNode::PrefixNode() :
		emotIndex(-1), childs()
{
}

/** create fresh emoticons dictionary, which will allow easy finding of occurrences
    of stored emots in text
*/
EmotsWalker::EmotsWalker() :
		root(new PrefixNode()), myPair(), positions(), lengths(), amountPositions(0)
{
	myPair.second = NULL;
}

/** deletes entire dictionary of emots */
EmotsWalker::~EmotsWalker()
{
	removeChilds(root);
	delete root;
}

/** find node in prefix tree, which is direct successor of given node with
    edge marked by given character
    return NULL if there is none
*/
PrefixNode* EmotsWalker::findChild(const PrefixNode* node, const QChar &c)
{
	myPair.first = c;
	// create variable 'position' with result of binary search in childs
	// of given node
	QList<Prefix>::const_iterator position = std::upper_bound (node -> childs.constBegin(), node -> childs.constEnd(), myPair);

	if (position != node -> childs.constEnd() && position -> first == c)
		return position -> second;
	else
		return NULL;
}

/** add successor to given node with edge marked by given characted
    (building of prefix tree)
*/
PrefixNode* EmotsWalker::insertChild(PrefixNode* node, const QChar &c)
{
	PrefixNode* newNode = new PrefixNode();

	// create child with new node
	Prefix newPair = qMakePair(c, newNode);
	// insert new child into childs of current node, performing binary
	// search to find correct position for it
	node -> childs.insert(std::upper_bound(node -> childs.begin(), node -> childs.end(), newPair), newPair);
	return newNode;
}

/** recursively delete all childs of given node */
void EmotsWalker::removeChilds(PrefixNode *node)
{
	foreach(const Prefix &ch, node->childs) {
		removeChilds(ch.second);
		delete ch.second;
	}
}

/** adds given string (emot) to dictionary of emots, giving it
    number, which will be used later to notify occurrences of
    emot in analyzed text
*/
void EmotsWalker::insertString(const QString &str, int num)
{
	PrefixNode *child, *node = root;
	unsigned int len = str.length();
	unsigned int pos = 0;

	// it adds string to prefix tree character after character
	while (pos < len) {
		child = findChild(node, str[pos]);
		if (child == NULL)
			child = insertChild(node, str[pos]);
		node = child;
		++pos;
	}

	if (node -> emotIndex == -1)
		node -> emotIndex = num;
}

/** return number of emot, which occurre in analyzed text just
    after adding given character (thus ending on this character)
    beginning of text analysis is turned on by 'initWalking()'
    if no emot occures, -1 is returned
*/
int EmotsWalker::checkEmotOccurrence(const QChar &c)
{
	const PrefixNode* next;
	int result = -1, resultLen = -1;

	if (amountPositions < positions.size())
	{
		lengths[amountPositions] = 0;
		positions[amountPositions++] = root;
	}
	else
	{
		++amountPositions;
		positions.push_back(root);
		lengths.push_back(0);
	}

	for (int i = amountPositions - 1; i >= 0; --i) {
		next = findChild(positions[i], c);
		if (next == NULL) {
			lengths[i] = lengths[--amountPositions];
			positions[i] = positions[amountPositions];
		}
		else {
			positions[i] = next;
			++lengths[i];
			if (result == -1 ||
				(next -> emotIndex >= 0 &&
				(next -> emotIndex < result || resultLen < lengths[i])))
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
