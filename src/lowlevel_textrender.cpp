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

#include "lowlevel_textrender.h"
#include "lowlevel_textrender_private.h"

#include "fc-lang-data.h"

#include <QtGui/QPainter>
#include <QtGui/QScreen>

#include <QtCore/QDebug>

#include <math.h>

#include FT_LCD_FILTER_H
#include FT_CONFIG_OPTIONS_H

//#define _DEBUG_DRAW

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

	qDebug() << "number of records in fc_lang_data =" << get_fc_lang_data_size();
	int sz = sizeof(fc_lang_rec)*get_fc_lang_data_size();

	const struct fc_lang_rec* lang_ptr = get_fc_lang_data();
	for (int i = 0; i < get_fc_lang_data_size(); i++)
	{
		sz += strlen(lang_ptr->lang_code) + 1;
		sz += lang_ptr->char_set_sz*sizeof(unsigned int);
		if (m_d->checklanguageSupport(lang_ptr->lang_code))
			list.append(QString(lang_ptr->lang_code));
		lang_ptr++;
	}
	qDebug() << "sizeof(fc_lang_data) =" << sz << "bytes = " << sz/1024 << "kb";
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
	qreal dpr = screen()->devicePixelRatio();
	sz *= dpr;
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
		m_offscreen->setDevicePixelRatio(dpr);
	}
	m_offscreen->fill(m_backgroundColor);

	QVector<uint> ucs4_data = m_text.toUcs4();

#define HARFBUZZ_SHAPING	1

#ifdef USE_HARFBUZZ
#if HARFBUZZ_SHAPING
	// HarfBuzz code part
	hb_buffer_reset(m_d->m_hb_buffer);
	//hb_buffer_add_utf16(m_d->m_hb_buffer, m_text.utf16(), m_text.length(), 0, m_text.length());
	hb_buffer_add_utf32(m_d->m_hb_buffer, ucs4_data.data(), ucs4_data.size(), 0, ucs4_data.size());
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
	FT_UInt glyph_index;
	FT_UInt prev_glyph_index;
	int pen_x = 10;
	int pen_y = 10 + m_d->m_ft_face->size->metrics.y_ppem;
	int glyph_pos_x, glyph_pos_y;
	FT_Render_Mode ft_render_mode;
	switch (m_antialiasingMode)
	{
		case AntialiasNone:
			ft_render_mode = FT_RENDER_MODE_MONO;
			break;
		case AntialiasGray:
			ft_render_mode = FT_RENDER_MODE_NORMAL;
			break;
		case AntialiasLCD_RGB:
		case AntialiasLCD_BGR:
		case AntialiasLCD_PenTile:
			ft_render_mode = FT_RENDER_MODE_LCD;
			break;
		case AntialiasLCD_V_RGB:
		case AntialiasLCD_V_BGR:
		case AntialiasLCD_V_PenTile:
			ft_render_mode = FT_RENDER_MODE_LCD_V;
			break;
		default:
			ft_render_mode = FT_RENDER_MODE_NORMAL;
			break;
	}
#ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
	// ClearType-style LCD rendering
	switch (m_antialiasingMode)
	{
		case AntialiasLCD_RGB:
		case AntialiasLCD_BGR:
		case AntialiasLCD_V_RGB:
		case AntialiasLCD_V_BGR:
			// ClearType-style LCD rendering
			qDebug() << "Using ClearType-style LCD rendering";
#if 1
			//error = FT_Library_SetLcdFilter(m_d->m_ft_library, FT_LCD_FILTER_NONE);
			error = FT_Library_SetLcdFilter(m_d->m_ft_library, FT_LCD_FILTER_DEFAULT);
			//error = FT_Library_SetLcdFilter(m_d->m_ft_library, FT_LCD_FILTER_LIGHT);
			if (0 != error)
				qDebug() << "FT_Library_SetLcdFilter() failed, error =" << error;
#else
			{
				//unsigned char weights[5] = { 0x08, 0x4D, 0x56, 0x4D, 0x08 };		// default
				unsigned char weights[5] = { 0x00, 0x55, 0x56, 0x55, 0x00 };		// light
				//unsigned char weights[5] = { 0x00, 0x55, 0x55, 0x55, 0x00 };		// Boxy 3-tap filter {0, ⅓, ⅓, ⅓, 0}
				error = FT_Library_SetLcdFilterWeights(m_d->m_ft_library, weights);
				if (0 != error)
					qDebug() << "FT_Library_SetLcdWeights() failed, error =" << error;
			}
#endif
			break;
	}
#else
	// Harmony LCD rendering
	switch (m_antialiasingMode)
	{
		case AntialiasLCD_RGB:
		case AntialiasLCD_V_RGB:
			{
				qDebug() << "Using Harmony LCD rendering";
				// {{-⅓, 0}, {0, 0}, {⅓, 0}}
				FT_Vector sub[3] = { {-21, 0}, {0, 0}, {21, 0} };
				error = FT_Library_SetLcdGeometry(m_d->m_ft_library, sub);
				if (0 != error)
					qDebug() << "FT_Library_SetLcdGeometry() failed, error =" << error;
			}
			break;
		case AntialiasLCD_BGR:
		case AntialiasLCD_V_BGR:
			{
				qDebug() << "Using Harmony LCD rendering";
				// {{⅓, 0}, {0, 0}, {-⅓, 0}}
				FT_Vector sub[3] = { {21, 0}, {0, 0}, {-21, 0} };
				error = FT_Library_SetLcdGeometry(m_d->m_ft_library, sub);
				if (0 != error)
					qDebug() << "FT_Library_SetLcdGeometry() failed, error =" << error;
			}
			break;
		case AntialiasLCD_PenTile:
		case AntialiasLCD_V_PenTile:
			{
				qDebug() << "Using Harmony LCD rendering";
				// {{-⅙, ¼}, {-⅙, -¼}, {⅓, 0}}
				FT_Vector sub[3] = { {-11, 16}, {-11, -16}, {22, 0} };
				error = FT_Library_SetLcdGeometry(m_d->m_ft_library, sub);
				if (0 != error)
					qDebug() << "FT_Library_SetLcdGeometry() failed, error =" << error;
			}
			break;
	}
#endif
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
	if (FT_HAS_COLOR(m_d->m_ft_face))
		ft_load_flag |= FT_LOAD_COLOR;
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
	for (int i = 0; i < glyph_count; i++)
#else
	int kerning = 0;
	for (int i = 0; i < ucs4_data.size(); i++)
#endif
	{
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
		glyph_index = glyph_info[i].codepoint;
#ifdef _DEBUG_DRAW
		qDebug() << "glyph index" << i << ": codepoint = " << QString("U+%1").arg(ucs4_data[glyph_info[i].cluster], 4, 16, QChar('0'));
		qDebug() << Qt::dec << "glyph index" << i << ": glyph index =" << glyph_index;
#endif
#else
		/* retrieve glyph index from character code */
		glyph_index = FT_Get_Char_Index(m_d->m_ft_face, ucs4_data.at(i));
#ifdef _DEBUG_DRAW
		qDebug() << "glyph index" << i << ": codepoint = " << QString("U+%1").arg(ucs4_data[i], 4, 16, QChar('0'));
		qDebug() << "glyph index" << i << ": glyph index =" << glyph_index;
#endif
#endif
		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Glyph(m_d->m_ft_face, glyph_index, ft_load_flag);
		if (0 != error)
			continue;  /* ignore errors */
		/* convert to an anti-aliased bitmap */
		error = FT_Render_Glyph(m_d->m_ft_face->glyph, ft_render_mode);
		if (0 != error)
			continue;
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
#elif !USE_HARFBUZZ
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
#endif
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
		glyph_pos_x = pen_x + m_d->m_ft_face->glyph->bitmap_left + (glyph_pos[i].x_offset >> 6);
		glyph_pos_y = pen_y - m_d->m_ft_face->glyph->bitmap_top - (glyph_pos[i].y_offset >> 6);
#else
		glyph_pos_x = pen_x + m_d->m_ft_face->glyph->bitmap_left + kerning;
		glyph_pos_y = pen_y - m_d->m_ft_face->glyph->bitmap_top;
#endif
		if (m_d->m_ft_face->glyph->bitmap.buffer)
		{
			switch (m_d->m_ft_face->glyph->bitmap.pixel_mode)
			{
			case FT_PIXEL_MODE_MONO:		// 1-bit gray (mono)
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					QRgb* dptr;
					int g_x, g_r, k, bitNo;
					unsigned char* sptr = m_d->m_ft_face->glyph->bitmap.buffer;
					unsigned char mask;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						int y = glyph_pos_y + g_r;
						if (y >= 0 && y < m_offscreen->height())
						{
							dptr = (QRgb*)m_offscreen->scanLine(y);
							for (k = 0; k < m_d->m_ft_face->glyph->bitmap.pitch; k++, sptr++)
							{
								for (bitNo = 0; bitNo < 8; bitNo++)
								{
									g_x = k*8 + (7 - bitNo);
									int x = glyph_pos_x + g_x;
									if (x >= 0 && x < m_offscreen->width())
									{
										if (g_x < m_d->m_ft_face->glyph->bitmap.width)
										{
											mask = (1 << bitNo);
											// glyph color at (g_x, g_y) in sptr[k]{7 - bitNo}
											if (*sptr & mask)
												dptr[x] = m_textColor.rgba();
											//else
											//	dptr[x] = m_backgroundColor.rgba();
										}
									}
								}
							}
						}
					}
				}
				break;
			case FT_PIXEL_MODE_GRAY:		// 8-bit gray
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					QRgb* dptr;
					int alpha;
					int g_x, g_r;
					unsigned char* sptr = m_d->m_ft_face->glyph->bitmap.buffer;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						int y = glyph_pos_y + g_r;
						if (y >= 0 && y < m_offscreen->height())
						{
							dptr = (QRgb*)m_offscreen->scanLine(y);
							for (g_x = 0; g_x < m_d->m_ft_face->glyph->bitmap.width; g_x++)
							{
								int x = glyph_pos_x + g_x;
								if (x >= 0 && x < m_offscreen->width())
								{
									// now in sptr[g_x] - alpha
									// in dptr[x] - target RGBA values
									alpha = sptr[g_x];
									// apply inverse gamma correction
									alpha = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha)/255.0, m_gamma)));
									// blending function (OVER operator)
									// See https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_glyph
									int r = (alpha*m_textColor.red() + (255 - alpha)*qRed(dptr[x]))/255;
									int g = (alpha*m_textColor.green() + (255 - alpha)*qGreen(dptr[x]))/255;
									int b = (alpha*m_textColor.blue() + (255 - alpha)*qBlue(dptr[x]))/255;
									dptr[x] = qRgba(r, g, b, m_textColor.alpha());
								}
							}
						}
						sptr += m_d->m_ft_face->glyph->bitmap.pitch;
					}
				}
				break;
			case FT_PIXEL_MODE_LCD:			// 24-bit RGB/BGR
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					QRgb* dptr;
					int alpha_r;
					int alpha_g;
					int alpha_b;
					int g_x, g_r;
					int dg_x;
					unsigned char* sptr = m_d->m_ft_face->glyph->bitmap.buffer;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						int y = glyph_pos_y + g_r;
						if (y >= 0 && y < m_offscreen->height())
						{
							dptr = (QRgb*)m_offscreen->scanLine(y);
							for (g_x = 0, dg_x = 0; g_x < m_d->m_ft_face->glyph->bitmap.width; g_x += 3, dg_x++)
							{
								int x = glyph_pos_x + dg_x;
								if (x >= 0 && x < m_offscreen->width())
								{
									// now in sptr[g_x]   - alpha (red)
									//     in sptr[g_x+1] - alpha (green)
									//     in sptr[g_x+2] - alpha (blue)
									// in dptr[x] - target RGBA values
#ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
									if (AntialiasLCD_BGR == m_antialiasingMode)
									{
										alpha_r = sptr[g_x+2];
										alpha_g = sptr[g_x+1];
										alpha_b = sptr[g_x];
									}
									else			// assumed RGB
#endif
									{
										alpha_r = sptr[g_x];
										alpha_g = sptr[g_x+1];
										alpha_b = sptr[g_x+2];
									}
									// apply inverse gamma correction
									alpha_r = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_r)/255.0, m_gamma)));
									alpha_g = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_g)/255.0, m_gamma)));
									alpha_b = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_b)/255.0, m_gamma)));
									// blending function (OVER operator)
									// See https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_glyph
									int r = (alpha_r*m_textColor.red() + (255 - alpha_r)*qRed(dptr[x]))/255;
									int g = (alpha_g*m_textColor.green() + (255 - alpha_g)*qGreen(dptr[x]))/255;
									int b = (alpha_b*m_textColor.blue() + (255 - alpha_b)*qBlue(dptr[x]))/255;
									dptr[x] = qRgba(r, g, b, m_textColor.alpha());
								}
							}
						}
						sptr += m_d->m_ft_face->glyph->bitmap.pitch;
					}
				}
				break;
			case FT_PIXEL_MODE_LCD_V:		// vertical 24-bit RGB/BGR
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					QRgb* dptr;
					int alpha_r;
					int alpha_g;
					int alpha_b;
					int g_x, g_r;
					int dg_r;
					unsigned char* sptr;
					for (g_x = 0; g_x < m_d->m_ft_face->glyph->bitmap.width; g_x++)
					{
						int x = glyph_pos_x + g_x;
						if (x >= 0 && x < m_offscreen->width())
						{
							sptr = m_d->m_ft_face->glyph->bitmap.buffer;
							for (g_r = 0, dg_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r+=3, dg_r++)
							{
								int y = glyph_pos_y + dg_r;
								if (y >= 0 && y < m_offscreen->height())
								{
									dptr = (QRgb*)m_offscreen->scanLine(y);
									// now in sptr[g_x]   - alpha (red)
									//     in sptr[g_x+1] - alpha (green)
									//     in sptr[g_x+2] - alpha (blue)
									// in dptr[x] - target RGBA values
#ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
									if (AntialiasLCD_V_BGR == m_antialiasingMode)
									{
										alpha_r = sptr[g_x + 2*m_d->m_ft_face->glyph->bitmap.pitch];
										alpha_g = sptr[g_x + m_d->m_ft_face->glyph->bitmap.pitch];
										alpha_b = sptr[g_x];
									}
									else			// assumed RGB
#endif
									{
										alpha_r = sptr[g_x];
										alpha_g = sptr[g_x + m_d->m_ft_face->glyph->bitmap.pitch];
										alpha_b = sptr[g_x + 2*m_d->m_ft_face->glyph->bitmap.pitch];
									}
									// apply inverse gamma correction
									alpha_r = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_r)/255.0, m_gamma)));
									alpha_g = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_g)/255.0, m_gamma)));
									alpha_b = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha_b)/255.0, m_gamma)));
									// blending function (OVER operator)
									// See https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_glyph
									int r = (alpha_r*m_textColor.red() + (255 - alpha_r)*qRed(dptr[x]))/255;
									int g = (alpha_g*m_textColor.green() + (255 - alpha_g)*qGreen(dptr[x]))/255;
									int b = (alpha_b*m_textColor.blue() + (255 - alpha_b)*qBlue(dptr[x]))/255;
									dptr[x] = qRgba(r, g, b, m_textColor.alpha());
								}
								sptr += 3*m_d->m_ft_face->glyph->bitmap.pitch;
							}
						}
					}
				}
				break;
			case FT_PIXEL_MODE_BGRA:		// 32-bit BGRA
				if (m_d->m_ft_face->glyph->bitmap.pitch > 0)
				{
					QRgb* dptr;
					int alpha;
					int g_x, g_r;
					unsigned char* sptr = m_d->m_ft_face->glyph->bitmap.buffer;
					for (g_r = 0; g_r < m_d->m_ft_face->glyph->bitmap.rows; g_r++)
					{
						int y = glyph_pos_y + g_r;
						if (y >= 0 && y < m_offscreen->height())
						{
							dptr = (QRgb*)m_offscreen->scanLine(y);
							for (g_x = 0; g_x < m_d->m_ft_face->glyph->bitmap.width; g_x++)
							{
								int x = glyph_pos_x + g_x;
								if (x >= 0 && x < m_offscreen->width())
								{
									// now in sptr[4*g_x]   - blue
									//     in sptr[4*g_x+1] - green
									//     in sptr[4*g_x+2] - red
									//     in sptr[4*g_x+3] - alpha
									// in dptr[x] - target RGBA values
									// transform 0 - 255 range to 0.0 - 1.0
									alpha = sptr[4*g_x + 3];
									// apply inverse gamma correction
									alpha = (int)(255.0*(1.0 - pow(1.0 - ((double)alpha)/255.0, m_gamma)));
									int r = sptr[4*g_x+2];
									int g = sptr[4*g_x+1];
									int b = sptr[4*g_x];
									// blending function (OVER operator)
									// See https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_glyph
									r = (alpha*r + (255 - alpha)*qRed(dptr[x]))/255;
									g = (alpha*g + (255 - alpha)*qGreen(dptr[x]))/255;
									b = (alpha*b + (255 - alpha)*qBlue(dptr[x]))/255;
									dptr[x] = qRgba(r, g, b, alpha);
									dptr[x] = qUnpremultiply(dptr[x]);
								}
							}
						}
						sptr += m_d->m_ft_face->glyph->bitmap.pitch;
					}
				}
				break;
			default:
				qDebug() << "Unsupported bitmap pixel mode (" << (int)m_d->m_ft_face->glyph->bitmap.pixel_mode << ")";
				break;
			}
			prev_glyph_index = glyph_index;
		}
		/* increment pen position */
#if USE_HARFBUZZ && HARFBUZZ_SHAPING
#ifdef _DEBUG_DRAW
		qDebug() << "glyph index" << i << ": ft_bitmap_left =" << m_d->m_ft_face->glyph->bitmap_left;
		qDebug() << "glyph index" << i << ": hb_x_offset =" << (glyph_pos[i].x_offset >> 6);
		qDebug() << "glyph index" << i << ": hb_x_advance =" << (glyph_pos[i].x_advance >> 6);
		qDebug() << "glyph index" << i << ": hb_y_offset =" << (glyph_pos[i].y_offset >> 6);
		qDebug() << "glyph index" << i << ": ft_x_advance=" << (m_d->m_ft_face->glyph->advance.x >> 6);
#endif

		pen_x += (glyph_pos[i].x_advance >> 6);
		pen_y += (glyph_pos[i].y_advance >> 6);
#else
#ifdef _DEBUG_DRAW
		qDebug() << "glyph index" << i << "x_offset=" << (m_d->m_ft_face->glyph->bitmap_left);
		qDebug() << "glyph index" << i << "x_advance=" << (m_d->m_ft_face->glyph->advance.x >> 6);
		if (m_useKerning)
		{
			qDebug() << "glyph index" << i << "kerning=" << kerning;
		}
#endif

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
