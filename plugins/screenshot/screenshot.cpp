/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QTimer>

#include "activate.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "icons/icons-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/custom-input.h"
#include "windows/message-dialog.h"

#include "configuration/screen-shot-configuration.h"
#include "gui/widgets/screenshot-widget.h"
#include "pixmap-grabber.h"
#include "screenshot-notification-service.h"
#include "screenshot-taker.h"

#include "screen-shot-saver.h"

#include "screenshot.h"
#include "screenshot.moc"

ScreenShot::ScreenShot(ScreenShotConfiguration *screenShotConfiguration, ChatWidget *chatWidget)
        : m_screenShotConfiguration{screenShotConfiguration}, Mode{}, MyScreenshotTaker{}, MyChatWidget{chatWidget}
{
}

ScreenShot::~ScreenShot()
{
    if (MyChatWidget)
        _activateWindow(m_configuration, MyChatWidget->window());

    delete MyScreenshotTaker;
    MyScreenshotTaker = 0;
}

void ScreenShot::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ScreenShot::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ScreenShot::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void ScreenShot::setScreenshotNotificationService(ScreenshotNotificationService *screenshotNotificationService)
{
    m_screenshotNotificationService = screenshotNotificationService;
}

void ScreenShot::init()
{
    MyScreenshotTaker = m_pluginInjectedFactory->makeInjected<ScreenshotTaker>(MyChatWidget);
    connect(MyScreenshotTaker, SIGNAL(screenshotTaken(QPixmap, bool)), this, SLOT(screenshotTaken(QPixmap, bool)));
    connect(MyScreenshotTaker, SIGNAL(screenshotNotTaken()), this, SLOT(screenshotNotTaken()));

    // Rest stuff
    warnedAboutSize = false;
}

void ScreenShot::takeStandardShot()
{
    MyScreenshotTaker->takeStandardShot();
}

void ScreenShot::takeShotWithChatWindowHidden()
{
    MyScreenshotTaker->takeShotWithChatWindowHidden();
}

void ScreenShot::takeWindowShot()
{
    MyScreenshotTaker->takeWindowShot();
}

void ScreenShot::screenshotTaken(QPixmap screenshot, bool needsCrop)
{
    if (!needsCrop)
    {
        screenshotReady(screenshot);
        return;
    }

    ScreenshotWidget *screenshotWidget = new ScreenshotWidget();
    connect(screenshotWidget, SIGNAL(pixmapCaptured(QPixmap)), this, SLOT(screenshotReady(QPixmap)));
    connect(screenshotWidget, SIGNAL(canceled()), this, SLOT(screenshotNotTaken()));

    screenshotWidget->setPixmap(screenshot);
    screenshotWidget->setShotMode(Mode);
    screenshotWidget->showFullScreen();
    screenshotWidget->show();
    QCoreApplication::processEvents();   // ensure window was shown, otherwise it won't be activated
    _activateWindow(m_configuration, screenshotWidget);
}

void ScreenShot::screenshotNotTaken()
{
    deleteLater();
}

void ScreenShot::screenshotReady(QPixmap p)
{
    auto saver = new ScreenShotSaver(m_iconsManager, m_screenShotConfiguration, this);
    auto screenShotPath = saver->saveScreenShot(p);

    if (m_screenShotConfiguration->pasteImageClauseIntoChatWidget())
    {
        pasteImageClause(screenShotPath);
        if (!checkImageSize(saver->size()))
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
                tr("Image size is bigger than maximal image size for this chat."));
    }

    deleteLater();
}

void ScreenShot::pasteImageClause(const QString &path)
{
    MyChatWidget->edit()->insertHtml(QString("<img src='%1' />").arg(path));
}

bool ScreenShot::checkImageSize(long int size)
{
    Q_UNUSED(size)

    ContactSet contacts = MyChatWidget->chat().contacts();
    for (auto const &contact : contacts)
        if (contact.maximumImageSize() * 1024 < size)
            return false;

    return true;
}

void ScreenShot::checkShotsSize()
{
    if (!m_screenShotConfiguration->warnAboutDirectorySize())
        return;

    long size = 0;

    long limit = m_screenShotConfiguration->directorySizeLimit();
    QDir dir(m_screenShotConfiguration->imagePath());

    QString prefix = m_screenShotConfiguration->fileNamePrefix();
    QStringList filters;
    filters << prefix + '*';
    QFileInfoList list = dir.entryInfoList(filters, QDir::Files);

    for (auto const &f : list)
        size += f.size();

    if (size / 1024 >= limit)
        m_screenshotNotificationService->notifySizeLimit(size);
}

#undef Bool
