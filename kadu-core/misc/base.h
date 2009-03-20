#ifndef KADU_BASE_H
#define KADU_BASE_H

#include <QtCore/QProcess>
#include <QtCore/QRect>
#include <QtCore/QTextCodec>
#include <QtCore/QVariant>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QFont>
#include <QtGui/QFontInfo>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>

#include <time.h>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "chat/chat_manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "config_file.h"
#include "debug.h"
#include "exports.h"
#include "html_document.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "message_box.h"
#include "protocols/status.h"
#include "../modules/gadu_protocol/gadu-protocol.h"

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <sys/timeb.h>
#undef MessageBox
#else
#include <sys/time.h>
#include <pwd.h>
#endif

#endif
