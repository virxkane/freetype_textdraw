#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
private:
	Ui::GMainWindow *ui;
};

#endif // MAINWINDOW_H
