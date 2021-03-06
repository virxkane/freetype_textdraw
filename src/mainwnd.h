/***************************************************************************
 *   Copyright (C) 2018-2019 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QColor>
#include <QtWidgets/QMainWindow>

namespace Ui {
	class GMainWindow;
}

class GMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit GMainWindow(QWidget *parent = 0);
	~GMainWindow();
protected:
	virtual void showEvent(QShowEvent *event);
protected slots:
	void slot_textEdited(const QString& text);
	void slot_fontSizeChanged(double size);
	void slot_fontFileEdited(const QString& text);
	void slot_browseFont();
	void slot_antialiasingChanged(int index);
	void slot_hintingChanged(int index);
	void slot_gammaChanged(double gamma);
	void slot_kerningChanged(int state);
	void slot_ligaturesChanged(int state);
	void slot_textColorChanged(const QColor& c);
	void slot_backgroundColorChanged(const QColor& c);

	void onFontChanged();
private:
	Ui::GMainWindow *ui;
};

#endif // MAINWINDOW_H
