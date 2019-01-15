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

#ifndef LOWVEVEL_TEXTRENDER_PRIVATE_H
#define LOWVEVEL_TEXTRENDER_PRIVATE_H

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef USE_HARFBUZZ
// HarfBuzz
#include <hb.h>
#endif

#include <QtCore/QString>

class GLowLevelTextRenderPrivate
{
	friend class GLowLevelTextRender;
public:
	GLowLevelTextRenderPrivate();
	virtual ~GLowLevelTextRenderPrivate();
	bool initFreetype();
	bool freetypeIsInited()
	{
		return m_ft_library != 0;
	}
	bool setFontFace(const QString& fileName);
	bool setFontPointSize(float size, int dpi = 96);
	bool setFontPixelSize(int size);
#ifdef USE_HARFBUZZ
	bool setKerning(bool kerning);
	bool setLigatures(bool liga);
#endif
	bool checklanguageSupport(const QString& langCode);
private:
	// Freetype fields
	FT_Library m_ft_library;
	FT_Face m_ft_face;
#ifdef USE_HARFBUZZ
	// HarfBuzz fields
	hb_buffer_t* m_hb_buffer;
	hb_font_t* m_hb_font;
	hb_feature_t* m_hb_features;
	int m_hb_features_count;
	int m_hb_feature_kern_pos;
	int m_hb_feature_liga_pos;
#endif
};

#endif // LOWVEVEL_TEXTRENDER_PRIVATE_H
