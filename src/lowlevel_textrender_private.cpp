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

#include "lowlevel_textrender_private.h"

#include <math.h>

#include "fc-lang-data.h"

#ifdef USE_HARFBUZZ
// HarfBuzz-Freetype
#include <hb-ft.h>
#endif

#include <QtCore/QFileInfo>

#include <QtCore/QDebug>

GLowLevelTextRenderPrivate::GLowLevelTextRenderPrivate()
{
	m_ft_library = 0;
	m_ft_face = 0;
	initFreetype();
#ifdef USE_HARFBUZZ
	m_hb_buffer = hb_buffer_create();
	m_hb_font = 0;
	m_hb_features = 0;
	m_hb_features_count = 0;
	m_hb_feature_kern_pos = -1;
	m_hb_feature_liga_pos = -1;
#endif
}

GLowLevelTextRenderPrivate::~GLowLevelTextRenderPrivate()
{
#ifdef USE_HARFBUZZ
	if (m_hb_features)
		free(m_hb_features);
	if (m_hb_buffer)
		hb_buffer_destroy(m_hb_buffer);
	if (m_hb_font)
		hb_font_destroy(m_hb_font);
#endif
	if (m_ft_face)
		FT_Done_Face(m_ft_face);
	if (m_ft_library)
		FT_Done_FreeType(m_ft_library);
}

bool GLowLevelTextRenderPrivate::initFreetype()
{
	if (m_ft_library)
		return true;

	bool res = false;
	FT_Error error = FT_Init_FreeType(&m_ft_library);
	if (FT_Err_Ok == error)
	{
		qDebug() << "Init Freetype OK.";
		res = true;
	}
	else
	{
		qDebug() << "Failed to init Freetype!";
	}
	return res;
}

bool GLowLevelTextRenderPrivate::setFontFace(const QString& fileName)
{
	bool res = false;
	if (0 == m_ft_library)
		return res;
	FT_Error error;

	if (m_ft_face)
	{
		FT_Done_Face(m_ft_face);
		m_ft_face = 0;
	}
#ifdef USE_HARFBUZZ
	if (m_hb_font)
	{
		hb_font_destroy(m_hb_font);
		m_hb_font = 0;
	}
#endif

	error = FT_New_Face(m_ft_library, fileName.toLocal8Bit().data(), 0, &m_ft_face);
#if 1
	if (FT_Err_Ok == error)
	{
		if (fileName.endsWith(".pfa") || fileName.endsWith(".pfb"))
		{
			QString kernFile = fileName.left(fileName.length() - 4);
			kernFile += QString(".afm");
			QFileInfo fi(kernFile);
			if (fi.exists())
			{
				error = FT_Attach_File(m_ft_face, kernFile.toLocal8Bit().data());
				qDebug() << "attached kern file: \"" << kernFile << "\" res =" << error;
			}
		}
	}
#endif
	if (FT_Err_Ok == error)
	{
		// OK
		qDebug() << "Font face created OK.";
		qDebug() << "family_name: " << m_ft_face->family_name;
		qDebug() << "style_flags: " << m_ft_face->style_flags;
		qDebug() << "style_name: " << m_ft_face->style_name;
		if (FT_HAS_KERNING(m_ft_face))
			qDebug() << "Font face have freetype compatible kerning table";
		else
			qDebug() << "Font face DON'T have freetype compatible kerning table!";
		qDebug() << "faces count:" << m_ft_face->num_faces;
#ifdef USE_HARFBUZZ
		m_hb_font = hb_ft_font_create(m_ft_face, 0);
#endif
		res = true;
	}
	else
	{
		qDebug() << "font face creation error!";
	}
	return res;
}

bool GLowLevelTextRenderPrivate::setFontPointSize(float size, int dpi)
{
	if (!m_ft_face)
		return false;
	bool res = false;
	FT_F26Dot6 height = (FT_F26Dot6)(size*64.0f);
	FT_F26Dot6 width = height;
	FT_Error error = FT_Set_Char_Size(m_ft_face, width, height, dpi, dpi);
	if (FT_Err_Invalid_Pixel_Size == error)
	{
		qDebug() << "fixed size font, can't be scaled...";
		if (m_ft_face->num_fixed_sizes > 0 && m_ft_face->available_sizes != NULL)
		{
			// Find nearest fixed size
			int idx = 0;
			int height_px = (int)((float)dpi*size/72.0);
			int prev_delta = abs(m_ft_face->available_sizes[0].height - height_px);
			int delta;
			for (int i = 1; i < m_ft_face->num_fixed_sizes; i++)
			{
				delta = abs(m_ft_face->available_sizes[i].height - height_px);
				if (delta < prev_delta)
				{
					idx = i;
				}
				else
					break;
			}
			error = FT_Select_Size(m_ft_face, idx);
		}
	}
	if (FT_Err_Ok == error)
	{
#ifdef USE_HARFBUZZ
		if (m_hb_font)
		{
			hb_font_destroy(m_hb_font);
			m_hb_font = 0;
		}
		m_hb_font = hb_ft_font_create(m_ft_face, 0);
#endif
		res = true;
	}
	return res;
}

bool GLowLevelTextRenderPrivate::setFontPixelSize(int size)
{
	if (!m_ft_face)
		return false;
	bool res = false;
	FT_Error error = FT_Set_Pixel_Sizes(m_ft_face, 0, size);
	if (FT_Err_Invalid_Pixel_Size == error)
	{
		qDebug() << "fixed size font, can't be scaled...";
		if (m_ft_face->num_fixed_sizes > 0 && m_ft_face->available_sizes != NULL)
		{
			// Find nearest fixed size
			int idx = 0;
			int prev_delta = abs(m_ft_face->available_sizes[0].height - size);
			int delta;
			for (int i = 1; i < m_ft_face->num_fixed_sizes; i++)
			{
				delta = abs(m_ft_face->available_sizes[i].height - size);
				if (delta < prev_delta)
				{
					idx = i;
				}
				else
					break;
			}
			error = FT_Select_Size(m_ft_face, idx);
		}
	}
	if (FT_Err_Ok == error)
	{
#ifdef USE_HARFBUZZ
		if (m_hb_font)
		{
			hb_font_destroy(m_hb_font);
			m_hb_font = 0;
		}
		m_hb_font = hb_ft_font_create(m_ft_face, 0);
#endif
		res = true;
	}
	return res;
}

#ifdef USE_HARFBUZZ

bool GLowLevelTextRenderPrivate::setKerning(bool kerning)
{
	hb_feature_t* pfeature;
	if (-1 == m_hb_feature_kern_pos)
	{
		// memory realloc
		void* tmp = realloc(m_hb_features, sizeof(hb_feature_t)*(m_hb_features_count + 1));
		if (tmp)
		{
			m_hb_features = (hb_feature_t*)tmp;
		}
		else
			return false;
		m_hb_feature_kern_pos = m_hb_features_count;
		m_hb_features_count++;
	}
	pfeature = &m_hb_features[m_hb_feature_kern_pos];
	if (kerning)
		hb_feature_from_string("+kern", -1, pfeature);
	else
		hb_feature_from_string("-kern", -1, pfeature);
	return true;
}

bool GLowLevelTextRenderPrivate::setLigatures(bool liga)
{
	if (-1 == m_hb_feature_liga_pos)
	{
		// memory realloc
		void* tmp = realloc(m_hb_features, sizeof(hb_feature_t)*(m_hb_features_count + 5));
		if (tmp)
		{
			m_hb_features = (hb_feature_t*)tmp;
		}
		else
			return false;
		m_hb_feature_liga_pos = m_hb_features_count;
		m_hb_features_count += 5;
	}
	hb_feature_t *pfeature1 = &m_hb_features[m_hb_feature_liga_pos];
	hb_feature_t *pfeature2 = &m_hb_features[m_hb_feature_liga_pos + 1];
	hb_feature_t *pfeature3 = &m_hb_features[m_hb_feature_liga_pos + 2];
	hb_feature_t *pfeature4 = &m_hb_features[m_hb_feature_liga_pos + 3];
	hb_feature_t *pfeature5 = &m_hb_features[m_hb_feature_liga_pos + 4];
	if (liga)
	{
		// See https://en.wikipedia.org/wiki/List_of_typographic_features
		hb_feature_from_string("+liga", -1, pfeature1); // Standard Ligatures
		hb_feature_from_string("+rlig", -1, pfeature2); // Required Ligatures
		hb_feature_from_string("+clig", -1, pfeature3); // Contextual ligatures
		hb_feature_from_string("+calt", -1, pfeature4); // Contextual alternates
		hb_feature_from_string("+rclt", -1, pfeature5); // Required Contextual Alternates
	}
	else
	{
		hb_feature_from_string("-liga", -1, pfeature1);
		hb_feature_from_string("-rlig", -1, pfeature2);
		hb_feature_from_string("-clig", -1, pfeature3);
		hb_feature_from_string("-calt", -1, pfeature4);
		hb_feature_from_string("-rclt", -1, pfeature5);
	}
	return true;
}

#endif	// USE_HARFBUZZ

#define CODE_IN_RANGE		0xF0F0FFFF		// By defifnition this is invalid code point.

bool GLowLevelTextRenderPrivate::checklanguageSupport(const QString& langCode)
{
	bool fullSupport = false;
	bool partialSupport = false;
	const struct fc_lang_rec* lang_ptr = get_fc_lang_data();
	int i;
	bool found = false;
	for (i = 0; i < get_fc_lang_data_size(); i++)
	{
		if (langCode.compare(lang_ptr->lang_code) == 0)
		{
			found = true;
			break;
		}
		lang_ptr++;
	}
	if (found)
	{
		unsigned int codePoint = 0;
		unsigned int tmp;
		unsigned int first, second = 0;
		bool inRange = false;
		FT_UInt glyphIndex;
		fullSupport = true;
		for (i = 0; ; )
		{
			// get codePoint
			if (inRange && codePoint < second)
			{
				codePoint++;
			}
			else
			{
				if (i >= lang_ptr->char_set_sz)
					break;
				tmp = lang_ptr->char_set[i];
				if (CODE_IN_RANGE == tmp)
				{
					if (i + 2 < lang_ptr->char_set_sz)
					{
						i++;
						first = lang_ptr->char_set[i];
						i++;
						second = lang_ptr->char_set[i];
						inRange = true;
						codePoint = first;
						i++;
					}
					else
					{
						// broken language char set
						//qDebug() << "broken language char set";
						fullSupport = false;
						break;
					}
				}
				else
				{
					codePoint = tmp;
					inRange = false;
					i++;
				}
			}
			// check codePoint in current font
			glyphIndex = FT_Get_Char_Index(m_ft_face, codePoint);
			if (glyphIndex != 0)
			{
				partialSupport = true;
			}
			else
			{
				fullSupport = false;
			}
			//qDebug() << "checking codePoint" << hex << codePoint << "in language char set" << langCode << (glyphIndex != 0 ? " yes" : "no");
		}
		if (fullSupport)
			qDebug() << "Font have full support of language" << langCode;
		else if (partialSupport)
			qDebug() << "Font have partial support of language" << langCode;
		else
			qDebug() << "Font DON'T have support of language" << langCode;
	}
	return fullSupport;
}
