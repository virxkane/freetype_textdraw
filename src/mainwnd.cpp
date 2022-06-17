/***************************************************************************
 *   Copyright (C) 2018-2021 by Chernov A.A.                               *
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

#include "mainwnd.h"
#include "ui_mainwnd.h"

#include <QtGui/QShowEvent>
#include <QtGui/QScreen>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>

GMainWindow::GMainWindow(QWidget *parent)
 : QMainWindow(parent),
	ui(new Ui::GMainWindow)
{
	ui->setupUi(this);

	// signals of custom widget not available in design editor
	connect(ui->textColorBox, SIGNAL(colorChanged(QColor)), this, SLOT(slot_textColorChanged(QColor)));
	connect(ui->backgroundColorBox, SIGNAL(colorChanged(QColor)), this, SLOT(slot_backgroundColorChanged(QColor)));

	ui->textColorBox->setDefaultColor(QColor(Qt::black));
	ui->backgroundColorBox->setDefaultColor(QColor::fromRgb(200, 240, 255));

#ifdef _WIN32
	// Override default font file
	ui->fontLineEdit->setText("c:/Windows/Fonts/NotoSerif-Regular.ttf");
#endif

	qreal dpr = screen()->devicePixelRatio();
	ui->fontSizeBox->setValue(ui->fontSizeBox->value()*dpr);
	ui->fontSizeBox->setMaximum(ui->fontSizeBox->maximum()*dpr);

	onFontChanged();
}

GMainWindow::~GMainWindow()
{
	delete ui;
}

void GMainWindow::showEvent(QShowEvent* event)
{
	static bool firstShow = true;
	QMainWindow::showEvent(event);
	if (firstShow)
	{
		ui->previewWidget->setFontFace(ui->fontLineEdit->text());
		ui->previewWidget->setFontPointSize(ui->fontSizeBox->value());
		slot_antialiasingChanged(ui->antialiasBox->currentIndex());
		slot_hintingChanged(ui->hintingBox->currentIndex());
		ui->previewWidget->setGammaCorrection(ui->gammaBox->value());
		ui->previewWidget->setKerning(ui->kerningBox->isChecked());
		ui->previewWidget->renderText(ui->sampleTextLineEdit->text());
#ifndef USE_HARFBUZZ
		ui->ligaturesBox->setEnabled(false);
#endif
		firstShow = false;
	}
}

void GMainWindow::slot_textEdited(const QString& text)
{
	ui->previewWidget->renderText(text);
}

void GMainWindow::slot_fontSizeChanged(double size)
{
	ui->previewWidget->setFontPointSize(size);
}

void GMainWindow::slot_fontFileEdited(const QString& text)
{
	ui->previewWidget->setFontFace(text);
}

void GMainWindow::slot_browseFont()
{
	QString dir = ui->fontLineEdit->text();
	QString filter = tr("Font files (*.ttf *.otf *.pfa *.pfb *.pcf *.pcf.gz)");
	QString path = QFileDialog::getOpenFileName(this, tr("Select font file..."), dir, filter);
	if (!path.isEmpty())
	{
		ui->fontLineEdit->setText(path);
		if (ui->previewWidget->setFontFace(path))
			onFontChanged();
	}
}

void GMainWindow::slot_antialiasingChanged(int index)
{
	GLowLevelTextRender::AntialiasingMode mode;
	switch (index)
	{
		case 0:
			mode = GLowLevelTextRender::AntialiasNone;
			ui->gammaBox->setEnabled(false);
			break;
		case 1:
			mode = GLowLevelTextRender::AntialiasGray;
			ui->gammaBox->setEnabled(true);
			break;
		case 2:
			mode = GLowLevelTextRender::AntialiasLCD_RGB;
			ui->gammaBox->setEnabled(true);
			break;
		case 3:
			mode = GLowLevelTextRender::AntialiasLCD_BGR;
			ui->gammaBox->setEnabled(true);
			break;
		case 4:
			mode = GLowLevelTextRender::AntialiasLCD_PenTile;
			ui->gammaBox->setEnabled(true);
			break;
		case 5:
			mode = GLowLevelTextRender::AntialiasLCD_V_RGB;
			ui->gammaBox->setEnabled(true);
			break;
		case 6:
			mode = GLowLevelTextRender::AntialiasLCD_V_BGR;
			ui->gammaBox->setEnabled(true);
			break;
		case 7:
			mode = GLowLevelTextRender::AntialiasLCD_V_PenTile;
			ui->gammaBox->setEnabled(true);
			break;
		default:
			mode = GLowLevelTextRender::AntialiasGray;
			ui->gammaBox->setEnabled(true);
			break;
	}
	ui->previewWidget->setAntialiasingMode(mode);
}

void GMainWindow::slot_hintingChanged(int index)
{
	GLowLevelTextRender::HintingMode mode;
	switch (index)
	{
		case 0:
			mode = GLowLevelTextRender::HintingNone;
			break;
		case 1:
			mode = GLowLevelTextRender::HintingByteCode;
			break;
		case 2:
			mode = GLowLevelTextRender::HintingAuto;
			break;
		default:
			mode = GLowLevelTextRender::HintingByteCode;
			break;
	}
	ui->previewWidget->setHintingMode(mode);
}

void GMainWindow::slot_gammaChanged(double gamma)
{
	ui->previewWidget->setGammaCorrection(gamma);
}

void GMainWindow::slot_kerningChanged(int state)
{
	ui->previewWidget->setKerning(ui->kerningBox->isChecked());
}

void GMainWindow::slot_ligaturesChanged(int state)
{
	ui->previewWidget->setAllowLigatures(ui->ligaturesBox->isChecked());
}

void GMainWindow::slot_textColorChanged(const QColor& c)
{
	ui->previewWidget->setTextColor(c);
}

void GMainWindow::slot_backgroundColorChanged(const QColor& c)
{
	ui->previewWidget->setBackgroundColor(c);
}

void GMainWindow::onFontChanged()
{
	QStringList langList = ui->previewWidget->getSupportedLanguages();
	QString langListStr;
	QStringList::const_iterator it = langList.begin();
	while (true)
	{
		if (it != langList.end())
			langListStr += *it;
		else
			break;
		++it;
		if (it != langList.end())
			langListStr += QString(",");
		else
			break;
	}
	ui->langList->setPlainText(langListStr);
}
