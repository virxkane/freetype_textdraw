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

#include "colorbox.h"

#include <QtGui/QPainter>

#include <QtWidgets/QLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QColorDialog>

#include "colorbox_private.h"

// class QColorBox
QColorBox::QColorBox(QWidget *parent)
 : QFrame(parent)
{
	setFrameStyle(QFrame::NoFrame);

	layout = new QHBoxLayout(this);
	comboBox = new QColorComboBox(this);
	layout->addWidget(comboBox, 0);
	tool = new QToolButton(this);
	tool->setText(tr("..."));
	layout->addWidget(tool, 0);
	customColor = QColor(0, 0, 0);
	defaultColor = QColor(0, 0, 0);
	fill();

	connect(comboBox, SIGNAL(activated(int)), SLOT(slotChanged(int)));
	connect(tool, SIGNAL(clicked()), this, SLOT(slotSelColor()));
}

#define COLORTBL_SZ 16

void QColorBox::fill()
{
	static const int color_tbl[COLORTBL_SZ][3] =
	{
		{ 255, 255, 255 },
		{ 192, 192, 192 },
		{ 128, 128, 128 },
		{   0,   0,   0 },
		{ 255,   0,   0 },
		{ 128,   0,   0 },
		{   0, 255,   0 },
		{   0, 128,   0 },
		{   0,   0, 255 },
		{   0,   0, 128 },
		{ 255, 255,   0 },
		{ 128, 128,   0 },
		{   0, 255, 255 },
		{   0, 128, 128 },
		{ 255,   0, 255 },
		{ 128,   0, 128 },
	};
	comboBox->insertItem(0, tr("default"));
	comboBox->setItemData(0, defaultColor, Qt::BackgroundRole);
	comboBox->setItemData(0, defaultColor, Qt::DecorationRole);
	QColor c;
	for (int i = 0; i < COLORTBL_SZ; i++)
	{
		c = QColor(color_tbl[i][0], color_tbl[i][1], color_tbl[i][2]);
		comboBox->insertItem(i + 1, /*c.name()*/QString());
		comboBox->setItemData(i + 1, c, Qt::BackgroundRole);
		comboBox->setItemData(i + 1, c, Qt::DecorationRole);
	}
	comboBox->insertItem(COLORTBL_SZ + 1, tr("custom"));
	comboBox->setItemData(COLORTBL_SZ + 1, customColor, Qt::BackgroundRole);
	comboBox->setItemData(COLORTBL_SZ + 1, customColor, Qt::DecorationRole);
}

QColorBox::~QColorBox()
{
}

QColor QColorBox::color()
{
	//return qVariantValue<QColor>(comboBox->itemData(comboBox->currentIndex(), Qt::BackgroundRole));
	return comboBox->itemData(comboBox->currentIndex(), Qt::BackgroundRole).value<QColor>();
}

void QColorBox::setColor(const QColor& c)
{
	int i;
	int ind = -1;
	QColor item_color;
	for (i = 0; i < COLORTBL_SZ + 1; i++)
	{
		//item_color = qVariantValue<QColor>(comboBox->itemData(comboBox->currentIndex(), Qt::BackgroundRole));
		item_color = (comboBox->itemData(comboBox->currentIndex(), Qt::BackgroundRole).value<QColor>());
		if (item_color == c)
		{
			ind = i;
			break;
		}
	}
	if (ind >= 0)
		comboBox->setCurrentIndex(ind);
	else
	{
		setCustomColor(c);
		comboBox->setCurrentIndex(COLORTBL_SZ + 1);
	}
}

void QColorBox::setCustomColor(const QColor& c)
{
	if (!c.isValid()) return;
	customColor = c;
	comboBox->setItemData(COLORTBL_SZ + 1, customColor, Qt::BackgroundRole);
	comboBox->setItemData(COLORTBL_SZ + 1, customColor, Qt::DecorationRole);
	if (comboBox->currentIndex() == COLORTBL_SZ + 1)
	{
		emit colorChanged(c);
		comboBox->repaint();
	}
}

void QColorBox::setDefaultColor(const QColor& c)
{
	if (!c.isValid()) return;
	defaultColor = c;
	comboBox->setItemData(0, defaultColor, Qt::BackgroundRole);
	comboBox->setItemData(0, defaultColor, Qt::DecorationRole);
	if (comboBox->currentIndex() == 0)
	{
		emit colorChanged(c);
		repaint();
	}
}

void QColorBox::slotSelColor()
{
	QColor sc = customColor;
	sc = QColorDialog::getColor(sc, this);
	if (sc.isValid())
	{
		//setCustomColor(sc);
		//comboBox->setCurrentItem(COLORTBL_SZ + 1);
		setColor(sc);
		emit colorChanged(sc);
	}
}

void QColorBox::slotChanged(int idx)
{
	QColor c = comboBox->itemData(idx, Qt::BackgroundRole).value<QColor>();
	emit colorChanged(c);
}

void QColorBox::languageChange()
{
	comboBox->setItemText(0, tr("default"));
	comboBox->setItemText(COLORTBL_SZ + 1, tr("custom"));
}
