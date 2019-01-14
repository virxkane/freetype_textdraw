/***************************************************************************
 *   Copyright (C) 2002-2019 by Chernov A.A.                               *
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

#include "colorbox_private.h"

#include <QtWidgets/QStylePainter>

#define MARGIN	6

QColorComboBox::QColorComboBox(QWidget* parent)
 : QComboBox(parent)
{
}

QColorComboBox::~QColorComboBox()
{
}

void QColorComboBox::paintEvent(QPaintEvent*)
{
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));

	// draw the combobox frame, focusrect and selected etc.
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);

	// draw the icon and text
	//painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
	QString text = currentText();
	QColor color = itemData(currentIndex(), Qt::BackgroundRole).value<QColor>();
	QPen pen = painter.pen();
	if (color.isValid())
	{
	QBrush brush(color, Qt::SolidPattern);
	painter.setPen(Qt::NoPen);
	painter.fillRect(opt.rect.left() + MARGIN, opt.rect.top() + MARGIN, opt.rect.width() - 2*MARGIN, opt.rect.height() - 2*MARGIN, brush);
	}
	pen.setColor(QColor(255 - color.red(), 255 - color.green(), 255 - color.blue()));
	painter.setPen(pen);
	QRect r = opt.rect;
	r.setLeft(r.left() + MARGIN);
	r.setWidth(r.width() - 2*MARGIN);
	painter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
}
