/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <algorithm>
#include <vector>

#include <QtCore/QBuffer>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QRect>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QImageWriter>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QX11Info>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "notify/notification.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "debug.h"

#include "gui/widgets/crop-image-widget.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "screenshot.h"

ScreenShot *screenShot;
NotifyEvent *ScreenShotImageSizeLimit = 0;

extern "C" int screenshot_init(bool firstLoad)
{
	kdebugf();

	ScreenShotImageSizeLimit = new NotifyEvent("ssSizeLimit", NotifyEvent::CallbackNotRequired, "ScreenShot images size limit");

	screenShot = new ScreenShot(firstLoad);
	NotificationManager::instance()->registerNotifyEvent(ScreenShotImageSizeLimit);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));
	MainConfigurationWindow::registerUiHandler(screenShot->configurationUiHandler());

	return 0;
}

extern "C" void screenshot_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));
	MainConfigurationWindow::unregisterUiHandler(screenShot->configurationUiHandler());
	NotificationManager::instance()->unregisterNotifyEvent(ScreenShotImageSizeLimit);

	delete ScreenShotImageSizeLimit;
	ScreenShotImageSizeLimit = 0;

	delete screenShot;
	screenShot = 0;
}

//-----------------------------------------------------------------------------------

class ScreenShotConfigurationUiHandler : public ConfigurationUiHandler
{
public:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
	{
		connect(mainConfigurationWindow->widget()->widgetById("screenshot/enableSizeLimit"), SIGNAL(toggled(bool)),
				mainConfigurationWindow->widget()->widgetById("screenshot/sizeLimit"), SLOT(setEnabled(bool)));

		QStringList opts;
		QList<QByteArray> byteArrayOpts = QImageWriter::supportedImageFormats();
		foreach (const QByteArray &opt, byteArrayOpts)
			opts.append(QString(opt));

		ConfigComboBox *formats = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("screenshot/formats"));
		if (formats)
			formats->setItems(opts, opts);
	}
};

//-----------------------------------------------------------------------------------

ScreenShot::ScreenShot(bool firstLoad) :
		QWidget(0, Qt::Tool | Qt::CustomizeWindowHint | Qt::FramelessWindowHint)
{
	kdebugf();
	minSize = 8;

	QHBoxLayout *layout = new QHBoxLayout(this);

	CropWidget = new CropImageWidget(this);
	layout->addWidget(CropWidget);

	ToolBox = new ScreenshotToolBox(this);
	hintTimer = new QTimer();
	connect(hintTimer, SIGNAL(timeout()), this, SLOT(updateHint()));

	// Chat windows menu
	menu = new QMenu();
	menu->addAction(tr("Simple shot"), this, SLOT(takeShot()));
	menu->addAction(tr("With chat window hidden"), this, SLOT(takeShotWithChatWindowHidden()));
	menu->addAction(tr("Window shot"), this, SLOT(takeWindowShot()));

	UiHandler = new ScreenShotConfigurationUiHandler();

	// Chat toolbar button
	screenShotAction = new ActionDescription(this,
		ActionDescription::TypeChat, "ScreenShotAction",
		this, SLOT(screenshotActionActivated(QAction *, bool)),
		"external_modules/module_screenshot-shot.png", "external_modules/module_screenshot-shot.png", tr("ScreenShot"), false, ""
	);

	if (firstLoad)
		ChatEditBox::addAction("ScreenShotAction");

	// Rest stuff
	buttonPressed = false;
	warnedAboutSize = false;

	createDefaultConfiguration();
}

ScreenShot::~ScreenShot()
{
	kdebugf();

	delete UiHandler;
	delete screenShotAction;

	hintTimer->stop();
	delete hintTimer;
	delete ToolBox;
	delete menu;
}

void ScreenShot::screenshotActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		QWidget *widget = widgets[widgets.size() - 1];
		menu->popup(widget->mapToGlobal(QPoint(0, widget->height())));
	}
}

void ScreenShot::mousePressEvent(QMouseEvent *e)
{
	kdebugf();

	if (e->button() != Qt::LeftButton)
		return;

	if (shotMode == SingleWindow)
	{
		releaseMouse();
		releaseKeyboard();

		hide();
		update();

		QTimer::singleShot(100, this, SLOT(takeWindowShot_Step2()));
	}
	else
	{
		region = QRect(e->pos(), e->pos());
		buttonPressed = true;

		int x = e->pos().x() + 50;
		int y = e->pos().y() + 50;

		QRect screen = QApplication::desktop()->screenGeometry();
		if (x + 150 > screen.width())
			x -= 150;

		if (y + 100 > screen.height())
			y -= 100;

		ToolBox->move(x, y);

		ToolBox->setGeometry("0x0");
		ToolBox->setFileSize("0 KB");
		ToolBox->show();
		hintTimer->start(1000);
	}
}

void ScreenShot::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);

	/*
	Q_UNUSED(e)

	if (!ShowPaintRect)
		return;

	QPainter painter(this);

	painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	painter.setPen(QPen(QBrush(Qt::black), 1, Qt::DashLine));
	painter.setBrush(Qt::NoBrush);

	painter.drawRect(region);
*/
// TODO: make it work again
/*
	QStyleOptionFocusRect styleOption;
	styleOption.initFrom(this);
	styleOption.rect = region;
	styleOption.state = QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange;
	styleOption.palette = colorGroup();
	styleOption.backgroundColor = palette().color(QPalette::Background);*/

// 	style()->drawPrimitive(QStyle::PE_FrameFocusRect, &styleOption, &painter);
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();

	if (!buttonPressed)
		return;

	hintTimer->stop();
	ToolBox->hide();

	// Uwalnianie myszki, klawiatury
	buttonPressed = false;
	releaseMouse();
	releaseKeyboard();

	// Normalizowanie prostok�ta do zrzutu
	region.setBottomRight(e->pos());
	region = region.normalized();

	// Zrzut
	ShowPaintRect = false;
	repaint();
	qApp->processEvents();

	QPixmap shot = QPixmap::grabWindow(winId(), region.x(), region.y(), region.width(), region.height());

	// Chowanie widgeta zrzutu i przywr�cenie kursora.
	hide();
	QApplication::restoreOverrideCursor();

	handleShot(shot);
}

void ScreenShot::handleShot(QPixmap p)
{
	if (!chatWidget)
		return;

	// Plik do zapisu:
	QString dirPath = config_file.readEntry("ScreenShot", "path", profilePath("images/"));

	QDir dir(dirPath);
	if (!dir.exists() && !dir.mkpath(dirPath))
	{
		MessageDialog::msg(tr("Unable to create direcotry %1 for storing screenshots!").arg(dirPath));
		return;
	}

	bool useShortJpg = config_file.readBoolEntry("ScreenShot", "use_short_jpg", false);

	QString ext = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toLower();
	if (useShortJpg && ext == "jpeg")
		ext = "jpg";

	QString path = QDir::cleanPath(
		dir.absolutePath() + "/" +
		config_file.readEntry("ScreenShot", "filenamePrefix", "shot") +
		QString::number(QDateTime::currentDateTime().toTime_t()) + "." +
		ext
	);

	const char *format = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toAscii();
	int quality = config_file.readNumEntry("ScreenShot", "quality", -1);
	if (!p.save(path, format, quality))
	{
		MessageDialog::msg(tr("Can't write file %1.\nAccess denied or other problem!").arg(path));
		return;
	}

	QFileInfo f(path);
	int size = f.size();

	if (size == 0)
	{
		MessageDialog::msg(tr("Screenshot %1 has 0 size!\nIt should be bigger.").arg(path));
		return;
	}

	if (shotMode == WithChatWindowHidden || shotMode == SingleWindow)
		restore(chatWidget);

	// Wklejanie [IMAGE] do okna Chat
	if (config_file.readBoolEntry("ScreenShot", "paste_clause", true))
	{
		// Sprawdzanie rozmiaru zrzutu wobec rozm�wc�w
		// TODO: 0.6.6
// 		UserListElements users = chatWidget->users()->toUserListElements();
// 		if (users.count() > 1)
// 			checkConferenceImageSizes(size);
// 		else
// 			if (!checkSingleUserImageSize(size))
// 				return;

		pasteImageClause(path);
	}

	chatWidget = 0;
	checkShotsSize();
}

void ScreenShot::pasteImageClause(const QString &path)
{
	chatWidget->edit()->insertPlainText(QString("[IMAGE ") + path + "]");
}

void ScreenShot::checkConferenceImageSizes(int size)
{
	Q_UNUSED(size)
	//TODO: 0.6.6, for now, assume it is ok
	/*
	UserListElements users = chatWidget->users()->toUserListElements();
	QStringList list;

	foreach (const UserListElement &user, users)
		if (user.protocolData("Gadu", "MaxImageSize").toInt() * 1024 < size)
			list.append(user.altNick());

	if (list.count() == 0)
		return;

	if (list.count() == users.count())
		MessageDialog::msg(tr("Image size is bigger than maximal image size\nset by all of conference contacts."), true);
	else
		MessageDialog::msg(tr("Image size is bigger than maximal image size\nset by some of conference contacts:\n%1.").arg(list.join(", ")), true);*/
}

bool ScreenShot::checkSingleUserImageSize(int size)
{
	Q_UNUSED(size)
	//TODO: 0.6.6, for now, assume it is ok
	return true;
	/*
	contacts = chatWidget->chat().contacts();

	if (users[0].protocolData("Gadu", "MaxImageSize").toInt() * 1024 >= size)
		return true;

	return MessageDialog::ask(tr("Image size is bigger than maximal image size set by %1. Send it anyway?").arg(users[0].altNick()));
	*/
}

void ScreenShot::keyPressEvent(QKeyEvent* e)
{
	kdebugf();

	if (e->key() == Qt::Key_Escape)
	{
		releaseMouse();
		releaseKeyboard();
		hide();
	}
}

void ScreenShot::takeShot()
{
	kdebugf();

	shotMode = Standard;

	QTimer::singleShot(100, this, SLOT(takeShot_Step2()));
	chatWidget->update();
	qApp->processEvents();
}

void ScreenShot::takeShotWithChatWindowHidden()
{
	shotMode = WithChatWindowHidden;

	wasMaximized = isMaximized(chatWidget);
	minimize(chatWidget);
	QTimer::singleShot(600, this, SLOT(takeShot_Step2()));
}

void ScreenShot::takeWindowShot()
{
	shotMode = SingleWindow;

	wasMaximized = isMaximized(chatWidget);
	minimize(chatWidget);

	takeShot_Step2();
}

void ScreenShot::takeShot_Step2()
{
	pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	CropWidget->setPixmap(pixmap);
	resize(pixmap.size());
	// setPixmap(pixmap);
	showFullScreen();
	show();
	setCursor(Qt::CrossCursor);

	QTimer::singleShot(100, this, SLOT(grabMouseSlot()));
}

void ScreenShot::grabMouseSlot()
{
	kdebugf();

	grabMouse();
	grabKeyboard();
}

void ScreenShot::mouseMoveEvent(QMouseEvent *e)
{
	kdebugf();
	if (!buttonPressed)
		return;

	region.setBottomRight(e->pos());

	QRect reg = region;
	reg = reg.normalized();

	ToolBox->setGeometry(
		QString("%1x%2")
			.arg(QString::number(reg.width()))
			.arg(QString::number(reg.height()))
		);

	ShowPaintRect = true;
	repaint();
}

void ScreenShot::updateHint()
{
	QBuffer buffer;

	QRect reg = region;
	reg = reg.normalized();

	QPixmap shot = QPixmap::grabWindow(winId(), reg.x(), reg.y(), reg.width(), reg.height());

	// TODO: cache + use configurationUpdated
	const char *format = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toAscii();
	int quality = config_file.readNumEntry("ScreenShot", "quality", -1);
	bool ret = shot.save(&buffer, format, quality);

	if (ret)
		ToolBox->setFileSize(QString::number(buffer.size()/1024) + " KB");
}

void ScreenShot::checkShotsSize()
{
	kdebugf();
	if (!config_file.readBoolEntry("ScreenShot", "dir_size_warns", true))
		return;

	int size = 0;

	int limit = config_file.readNumEntry("ScreenShot", "dir_size_limit", 10000);
	QDir dir(config_file.readEntry("ScreenShot", "path", profilePath("images")));

	QString prefix = config_file.readEntry("ScreenShot", "filenamePrefix", "shot");
	QStringList filters;
	filters << prefix + "*";
	QFileInfoList list = dir.entryInfoList(filters, QDir::Files);

	foreach (const QFileInfo &f, list)
		size += f.size();

	if (size/1024 >= limit)
	{
		Notification *notification = new Notification("ssSizeLimit", IconsManager::instance()->iconByPath("kadu_icons/kadu-blocking.png"));
		notification->setTitle(tr("ScreenShot size limit"));
		notification->setText(tr("Images size limit exceed: %1 KB").arg(size/1024));
		NotificationManager::instance()->notify(notification);
	}
}

void ScreenShot::takeWindowShot_Step2()
{
	kdebugf();
	QPixmap winPixmap = grabCurrent();
	handleShot(winPixmap);
}

void ScreenShot::restore(QWidget *w)
{
	// For tabs module
	while (w->parent())
		w = static_cast<QWidget *>(w->parent());

	if (wasMaximized)
		w->showMaximized();
	else
		w->showNormal();
}

void ScreenShot::minimize(QWidget* w)
{
	// For tabs module
	while (w->parent())
		w = static_cast<QWidget *>(w->parent());

	w->showMinimized();
}

bool ScreenShot::isMaximized(QWidget* w)
{
	// For tabs module
	while (w->parent())
		w = static_cast<QWidget *>(w->parent());

	return w->isMaximized();
}

void ScreenShot::createDefaultConfiguration()
{
	config_file.addVariable("ScreenShot", "fileFormat", "PNG");
	config_file.addVariable("ScreenShot", "use_short_jpg", true);
	config_file.addVariable("ScreenShot", "quality", -1);
	config_file.addVariable("ScreenShot", "path", profilePath("images/"));
	config_file.addVariable("ScreenShot", "filenamePrefix", "shot");
	config_file.addVariable("ScreenShot", "paste_clause", true);
	config_file.addVariable("ScreenShot", "dir_size_warns", true);
	config_file.addVariable("ScreenShot", "dir_size_limit", 10000);
}

//////////////////////////////////////////////////////////////////
// Code below is copied (and changed a little) from KSnapShot,
// copyrighted by Bernd Brandstetter <bbrand@freenet.de>.
// It's licenced with GPL.

static bool operator < ( const QRect& r1, const QRect& r2 )
{
    return r1.width() * r1.height() < r2.width() * r2.height();
}

QPixmap ScreenShot::grabCurrent()
{
    Window root;
    int y, x;
    uint w, h, border, depth;
    XGrabServer( QX11Info::display() );
    Window child = windowUnderCursor( /*includeDecorations*/true );
    XGetGeometry( QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth );
    Window parent;
    Window* children;
    unsigned int nchildren;
    if( XQueryTree( QX11Info::display(), child, &root, &parent,
                    &children, &nchildren ) != 0 ) {
        if( children != NULL )
            XFree( children );
        int newx, newy;
        Window dummy;
        if( XTranslateCoordinates( QX11Info::display(), parent, QX11Info::appRootWindow(),
            x, y, &newx, &newy, &dummy )) {
            x = newx;
            y = newy;
        }
    }
    QPixmap pm( grabWindow( child, x, y, w, h, border ) );
    XUngrabServer( QX11Info::display() );
    return pm;
}

Window ScreenShot::windowUnderCursor(bool includeDecorations)
{
    Window root;
    Window child;
    uint mask;
    int rootX, rootY, winX, winY;
    XGrabServer( QX11Info::display() );
    XQueryPointer( QX11Info::display(), QX11Info::appRootWindow(), &root, &child,
                   &rootX, &rootY, &winX, &winY, &mask );
    if( child == None )
        child = QX11Info::appRootWindow();
    if( !includeDecorations ) {
        Window real_child = findRealWindow( child );
        if( real_child != None ) // test just in case
            child = real_child;
    }
    return child;
}

QPixmap ScreenShot::grabWindow( Window child, int x, int y, uint w, uint h, uint border )
{
    QPixmap pm( QPixmap::grabWindow( QX11Info::appRootWindow(), x, y, w, h ) );

    int tmp1, tmp2;
    //Check whether the extension is available
    if ( XShapeQueryExtension( QX11Info::display(), &tmp1, &tmp2 ) ) {
        QBitmap mask( w, h );
        //As the first step, get the mask from XShape.
        int count, order;
        XRectangle* rects = XShapeGetRectangles( QX11Info::display(), child,
                                                 ShapeBounding, &count, &order );
        //The ShapeBounding region is the outermost shape of the window;
        //ShapeBounding - ShapeClipping is defined to be the border.
        //Since the border area is part of the window, we use bounding
        // to limit our work region
        if (rects) {
            //Create a QRegion from the rectangles describing the bounding mask.
            QRegion contents;
            for ( int pos = 0; pos < count; pos++ )
                contents += QRegion( rects[pos].x, rects[pos].y,
                                     rects[pos].width, rects[pos].height );
            XFree( rects );

            //Create the bounding box.
            QRegion bbox( 0, 0, w, h );

            if( border > 0 ) {
                contents.translate( border, border );
                contents += QRegion( 0, 0, border, h );
                contents += QRegion( 0, 0, w, border );
                contents += QRegion( 0, h - border, w, border );
                contents += QRegion( w - border, 0, border, h );
            }

            //Get the masked away area.
            QRegion maskedAway = bbox - contents;
            QVector<QRect> maskedAwayRects = maskedAway.rects();

            //Construct a bitmap mask from the rectangles
            QPainter p(&mask);
            p.fillRect(0, 0, w, h, Qt::color1);
            for (int pos = 0; pos < maskedAwayRects.count(); pos++)
                    p.fillRect(maskedAwayRects[pos], Qt::color0);
            p.end();

            pm.setMask(mask);
        }
    }

    return pm;
}

// Recursively iterates over the window w and its children, thereby building
// a tree of window descriptors. Windows in non-viewable state or with height
// or width smaller than minSize will be ignored.
void ScreenShot::getWindowsRecursive( std::vector<QRect>& windows, Window w, int rx, int ry, int depth )
{
    XWindowAttributes atts;
    XGetWindowAttributes( QX11Info::display(), w, &atts );
    if ( atts.map_state == IsViewable &&
         atts.width >= minSize && atts.height >= minSize ) {
        int x = 0, y = 0;
        if ( depth ) {
            x = atts.x + rx;
            y = atts.y + ry;
        }

        QRect r( x, y, atts.width, atts.height );
        if ( std::find( windows.begin(), windows.end(), r ) == windows.end() ) {
            windows.push_back( r );
        }

        Window root, parent;
        Window* children;
        unsigned int nchildren;

        if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
            for( unsigned int i = 0; i < nchildren; ++i ) {
                getWindowsRecursive( windows, children[ i ], x, y, depth + 1 );
            }
            if( children != NULL )
                XFree( children );
        }
    }
    if ( depth == 0 )
        std::sort( windows.begin(), windows.end() );
}

Window ScreenShot::findRealWindow( Window w, int depth )
{
    if( depth > 5 )
        return None;
    static Atom wm_state = XInternAtom( QX11Info::display(), "WM_STATE", False );
    Atom type;
    int format;
    unsigned long nitems, after;
    unsigned char* prop;
    if( XGetWindowProperty( QX11Info::display(), w, wm_state, 0, 0, False, AnyPropertyType,
        &type, &format, &nitems, &after, &prop ) == Success ) {
        if( prop != NULL )
            XFree( prop );
        if( type != None )
            return w;
    }
    Window root, parent;
    Window* children;
    unsigned int nchildren;
    Window ret = None;
    if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
        for( unsigned int i = 0;
             i < nchildren && ret == None;
             ++i )
            ret = findRealWindow( children[ i ], depth + 1 );
        if( children != NULL )
            XFree( children );
    }
    return ret;
}

// End of code copied from KSnapShot
//////////////////////////////////////////////////////////////////
