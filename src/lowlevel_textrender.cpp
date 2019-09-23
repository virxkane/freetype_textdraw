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

#include "lowlevel_textrender.h"
#include "lowlevel_textrender_private.h"

#include "fc-lang-cat.h"

#include <QtGui/QPainter>

#include <QtCore/QDebug>

#include <math.h>

GLowLevelTextRender::GLowLevelTextRender(QWidget *parent)
 : QWidget(parent)
{
	m_d = new GLowLevelTextRenderPrivate;
	m_d->setFontFace("/usr/share/fonts/noto/NotoSerif-Regular.ttf");
	m_d->setFontPointSize(12.0f, 96);
	m_fontSize = 12.0f;
	m_antialiasingMode = AntialiasGray;
	m_hintingMode = HintingAuto;
	m_useKerning = false;
	m_textColor = QColor(Qt::black);
	m_backgroundColor = QColor(Qt::white);
	m_gamma = 1.0;

	m_offscreen = 0;
}

GLowLevelTextRender::~GLowLevelTextRender()
{
	if (m_offscreen) delete m_offscreen;
	delete m_d;
}

bool GLowLevelTextRender::setFontFace(const QString& fileName)
{
	bool res = m_d->setFontFace(fileName);
	if (res)
		res = m_d->setFontPointSize(m_fontSize);
	if (res)
		renderText();
	else
		qDebug() << "Failed to load font:" << fileName;
	return res;
}

bool GLowLevelTextRender::setFontPointSize(float size, int dpi)
{
	bool res = m_d->setFontPointSize(size, dpi);
	if (res)
	{
		m_fontSize = size;
		renderText();
	}
	return res;
}

bool GLowLevelTextRender::setFontPixelSize(int size)
{
	bool res = m_d->setFontPixelSize(size);
	if (res)
		renderText();
	return res;
}

void GLowLevelTextRender::setAntialiasingMode(GLowLevelTextRender::AntialiasingMode mode)
{
	m_antialiasingMode = mode;
	renderText();
}

void GLowLevelTextRender::setHintingMode(GLowLevelTextRender::HintingMode hinting)
{
	m_hintingMode = hinting;
	renderText();
}

void GLowLevelTextRender::setKerning(bool kerning)
{
	m_useKerning = kerning;
	renderText();
}

void GLowLevelTextRender::setAllowLigatures(bool ligatures)
{
	m_allowLigatures = ligatures;
	renderText();
}

QStringList GLowLevelTextRender::getSupportedLanguages()
{
	QStringList list;

	qDebug() << "number of records in fc_lang_cat =" << fc_lang_cat_sz;
	int sz = sizeof(fc_lang_catalog)*fc_lang_cat_sz;

	struct fc_lang_catalog* langPtr = fc_lang_cat;
	for (int i = 0; i < fc_lang_cat_sz; i++)
	{
		sz += strlen(langPtr->lang_code) + 1;
		sz += langPtr->char_set_sz*sizeof(unsigned int);
		if (m_d->checklanguageSupport(langPtr->lang_code))
			list.append(QString(langPtr->lang_code));
		langPtr++;
	}
	qDebug() << "sizeof(fc_lang_cat) =" << sz << "bytes = " << sz/1024 << "kb";
	return list;
}

bool GLowLevelTextRender::renderText(const QString& text)
{
	m_text = text;
	return renderText();
}

bool GLowLevelTextRender::renderText()
{
	QSize sz = size();
	if (sz.width() < 100 || sz.height() < 100)
		return false;
	if (!m_d->freetypeIsInited())
		return false;
	if (!m_d->m_ft_face)
		return false;
	if (m_text.isEmpty())
		return false;
	if (m_offscreen)
	{
		if (m_offscreen->size() != sz)
		{
			delete m_offscreen;
			m_offscreen = 0;
		}
	}
	if (!m_offscreen)
	{
		m_offscreen = new QImage(sz.width(), sz.height(), QImage::Format_RGB32);
	}
	m_offscreen->fill(m_backgroundColor);

	QPainter p(m_offscreen);
	QImage* glyphImage = new QImage(20, 20, QImage::Format_ARGB32);

	int kerning = 0;

#define HARFBUZZ_SHAPING	1

#ifdef USE_HARFBUZZ
#if HARFBUZZ_SHAPING
	// HarfBuzz code part
	hb_buffer_reset(m_d->m_hb_buffer);
	hb_buffer_add_utf16(m_d->m_hb_buffer, m_text.utf16(), m_text.length(), 0, m_text.length());
	//hb_buffer_set_content_type(m_d->m_hb_buffer, HB_BUFFER_CONTENT_TYPE_UNICODE);
	hb_buffer_guess_segment_properties(m_d->m_hb_buffer);

	// Setup features
	m_d->setKerning(m_useKerning);
	m_d->setLigatures(m_allowLigatures);

	// shape
	hb_shape(m_d->m_hb_font, m_d->m_hb_buffer, m_d->m_hb_features, m_d->m_hb_features_count);

	unsigned int glyph_count = hb_buffer_get_length(m_d->m_hb_buffer);
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(m_d->m_hb_buffer, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(m_d->m_hb_buffer, &glyph_count);

	if (glyph_count != m_text.length())
	{
		qDebug() << "glyph count (" << glyph_count << ") not equal to text length (" << m_text.length() << ")";
		//qDebug() << "exiting!";
		//return false;
	}
#endif
#endif

	FT_Error error;
	int pen_x, pen_y;
	FT_UInt glyph_index;
	FT_UInt prev_glyph_index;
	pen_x = 10;
	pen_y = 10 + m_fontSize;
	FT_Render_Mode ft_render_mode;
	switch (m_antialiasingMode)
	{
		case AntialiasNone:
			ft_render_mode = FT_RENDER_MODE_MONO;
			break;
		case AntialiasGray:
			ft_render_mode = FT_RENDER_MODE_NORMAL;
			break;
		case AntialiasLCD:
			ft_render_mode = FT_RENDER_MODE_LCD;
			break;
		case AntialiasLCD_V:
			ft_render_mode = FT_RENDER_MODE_LCD_V;
			break;
		default:
			ft_render_mode = FT_RENDER_MODE_NORMAL;
			break;
	}
	FT_Int32 ft_load_flag = 0;
	switch (m_hintingMode)
	{
		case HintingNone:
			ft_load_flag = FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
			break;
		case HintingByteCode:
			ft_load_flag = FT_LOAD_DEFAULT;
			break;
		case HintingAuto:
			ft_load_flag = FT_LOAD_FORCE_AUTOHINT;
			break;
		default:
			ft_load_flag = FT_LOAD_DEFAULT;
			break;
	}
#ifdef USE_HARFBUZZ
	for (int i = 0; i < glyph_count; i++)
#else
	for (int i = 0; i < m_text.length(); i++)
#endif
	{
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
		glyph_index = glyph_info[i].codepoint;
		qDebug() << "glyph index" << i << ": codepoint =" << m_text[i].unicode();
		qDebug() << "glyph index" << i << ": glyph index =" << glyph_info[i].codepoint;
#else
		/* retrieve glyph index from character code */
		glyph_index = FT_Get_Char_Index(m_d->m_ft_face, m_text.at(i).unicode());
#endif

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Glyph(m_d->m_ft_face, glyph_index, ft_load_flag);
		if (0 != error)
			continue;  /* ignore errors */

		/* convert to an anti-aliased bitmap */
		error = FT_Render_Glyph(m_d->m_ft_face->glyph, ft_render_mode);
		if (0 != error)
			continue;

		if (glyphImage->width() < m_d->m_ft_face->glyph->bitmap.width ||
				glyphImage->height() < m_d->m_ft_face->glyph->bitmap.rows)
		{
			// extend glyph image buffer
			delete glyphImage;
			glyphImage = new QImage(m_d->m_ft_face->glyph->bitmap.width, m_d->m_ft_face->glyph->bitmap.rows, QImage::Format_ARGB32);
		}
		if (m_d->m_ft_face->glyph->bitmap.buffer)
		{
			if (FT_PIXEL_MODE_MONO == m_d->m_ft_face->glyph->bitmap.pixel_mode)	// 1-bit gray (mono)
			{
				QRgb* dptr;
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					int g_x, g_r, k, bitNo;
					unsigned char* sptr;
					unsigned char mask;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						sptr = m_d->m_ft_face->glyph->bitmap.buffer + g_r*m_d->m_ft_face->glyph->bitmap.pitch;
						dptr = (QRgb*)glyphImage->scanLine(g_r);
						for (k = 0; k < m_d->m_ft_face->glyph->bitmap.pitch; k++, sptr++)
						{
							for (bitNo = 0; bitNo < 8; bitNo++)
							{
								g_x = k*8 + (7 - bitNo);
								if (g_x < m_d->m_ft_face->glyph->bitmap.width)
								{
									mask = (1 << bitNo);
									// glyph color at (g_x, g_y) in sptr[k]{7 - bitNo}
									if (*sptr & mask)
										dptr[g_x] = qRgba(m_textColor.red(), m_textColor.green(), m_textColor.blue(), 255);
									else
										dptr[g_x] = qRgba(0, 0, 0, 0);
								}
							}
						}
					}
				}
			}
			else if (FT_PIXEL_MODE_GRAY == m_d->m_ft_face->glyph->bitmap.pixel_mode)	// 8-bit gray
			{
				//glyphImage->fill(QColor(Qt::white));
				QRgb* dptr;
				double alpha;
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					int g_x, g_r;
					unsigned char* sptr;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						sptr = m_d->m_ft_face->glyph->bitmap.buffer + g_r*m_d->m_ft_face->glyph->bitmap.pitch;
						dptr = (QRgb*)glyphImage->scanLine(g_r);
						for (g_x = 0; g_x < m_d->m_ft_face->glyph->bitmap.width; g_x++)
						{
							// now in ptr[g_x] - alpha
							// in dptr[g_x] - target RGBA values
							// transform 0 - 255 range to 0.0 - 1.0
							alpha = ((double)sptr[g_x])/255.0;
							// apply inverse gamma correction
							alpha = 1.0 - pow(1.0 - alpha, m_gamma);
							dptr[g_x] = qRgba(m_textColor.red(), m_textColor.green(), m_textColor.blue(), (int)(alpha*255.0));
							//dptr[g_x] += qRgba(m_backgroundColor.red(), m_backgroundColor.green(), m_backgroundColor.blue(), (int)((1.0 - alpha)*255.0));
						}
					}
				}
			}
			else
				qDebug() << "Unsupported bitmap pixel mode (" << (int)m_d->m_ft_face->glyph->bitmap.pixel_mode << ")";
#if USE_HARFBUZZ && !HARFBUZZ_SHAPING
			// get kerning
			if (m_useKerning)
			{
				if (i > 0)
				{
					//kerning = hb_font_get_glyph_h_kerning(m_d->m_hb_font, m_text[i - 1].unicode(), m_text[i].unicode());
					kerning = hb_font_get_glyph_h_kerning(m_d->m_hb_font, prev_glyph_index, glyph_index);
					kerning >>= 6;
					qDebug() << "i =" << i << "kerning =" << kerning;
				}
			}
#endif
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
			p.drawImage(pen_x + m_d->m_ft_face->glyph->bitmap_left + (glyph_pos[i].x_offset >> 6),
						pen_y - m_d->m_ft_face->glyph->bitmap_top + (glyph_pos[i].y_offset >> 6),
						*glyphImage, 0, (glyph_pos[i].y_offset >> 6),
						m_d->m_ft_face->glyph->bitmap.width,
						m_d->m_ft_face->glyph->bitmap.rows);
#else
			FT_Vector delta;
			if (m_useKerning)
			{
				if (i > 0)
				{
					error = FT_Get_Kerning(m_d->m_ft_face, prev_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
					if (error)
						kerning = 0;
					else
						kerning = delta.x >> 6;
				}
			}
			p.drawImage(pen_x + m_d->m_ft_face->glyph->bitmap_left + kerning,
						pen_y - m_d->m_ft_face->glyph->bitmap_top,
						*glyphImage, 0, 0,
						m_d->m_ft_face->glyph->bitmap.width,
						m_d->m_ft_face->glyph->bitmap.rows);
#endif
			prev_glyph_index = glyph_index;
		}
		/* increment pen position */
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
		qDebug() << "glyph index" << i << ": ft_bitmap_left =" << m_d->m_ft_face->glyph->bitmap_left;
		qDebug() << "glyph index" << i << ": hb_x_offset =" << (glyph_pos[i].x_offset >> 6);
		qDebug() << "glyph index" << i << ": hb_x_advance =" << (glyph_pos[i].x_advance >> 6);
		qDebug() << "glyph index" << i << ": ft_x_advance=" << (m_d->m_ft_face->glyph->advance.x >> 6);

		pen_x += (glyph_pos[i].x_advance >> 6);
		pen_y += (glyph_pos[i].y_advance >> 6);
#else
		qDebug() << "glyph index" << i << "x_offset=" << (m_d->m_ft_face->glyph->bitmap_left);
		qDebug() << "glyph index" << i << "x_advance=" << (m_d->m_ft_face->glyph->advance.x >> 6);
		if (m_useKerning)
		{
			qDebug() << "glyph index" << i << "kerning=" << kerning;
		}

		pen_x += m_d->m_ft_face->glyph->advance.x >> 6;
		pen_x += kerning;
		pen_y += m_d->m_ft_face->glyph->advance.y >> 6; /* not useful for now */
#endif
	}
	update();
	return true;
}

void GLowLevelTextRender::setBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
	renderText();
}

void GLowLevelTextRender::setGammaCorrection(double gamma)
{
	m_gamma = gamma;
	renderText();
}

void GLowLevelTextRender::setTextColor(const QColor& textColor)
{
	m_textColor = textColor;
	renderText();
}

void GLowLevelTextRender::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	if (m_offscreen)
		p.drawImage(0, 0, *m_offscreen, 0, 0);
}
