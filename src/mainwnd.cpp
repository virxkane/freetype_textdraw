#include "mainwnd.h"
#include "ui_mainwnd.h"

#include <QtGui/QShowEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>

GMainWindow::GMainWindow(QWidget *parent)
 : QMainWindow(parent),
	ui(new Ui::GMainWindow)
{
	ui->setupUi(this);
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

		ui->previewWidget->setBackgroundColor(QColor::fromRgb(200, 240, 255));
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
	QString filter = tr("Font files (*.ttf *.otf *.pfa *.pfb)");
	QString path = QFileDialog::getOpenFileName(this, tr("Select font file..."), dir, filter);
	if (!path.isEmpty())
	{
		ui->fontLineEdit->setText(path);
		ui->previewWidget->setFontFace(path);
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
			mode = GLowLevelTextRender::AntialiasLCD;
			ui->gammaBox->setEnabled(true);
			break;
		case 3:
			mode = GLowLevelTextRender::AntialiasLCD_V;
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
