// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (c) 2018 by Чернов А.А.                                     *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "mainwnd.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	GMainWindow wnd;
	wnd.show();

	int app_ret = app.exec();

	return app_ret;
}
