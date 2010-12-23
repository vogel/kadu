#ifndef ACTIVATE_H
#define ACTIVATE_H

class QWidget;

#include "exports.h"

bool KADUAPI _isActiveWindow( QWidget *window );
void KADUAPI _activateWindow( QWidget *window );

/**
 * Returns true if passed widget's window is active or fully visible to the user
 * (i.e., it is on the active desktop, is not covered by any other window and is
 * placed within desktop borders).
 *
 * NOTE: It is currently supported only on X11. On other platforms it is equivalent to QWidget::isActiveWindow().
 */
bool KADUAPI _isWindowActiveOrFullyVisible( QWidget *window );

#endif // ACTIVATE_H
