/***************************************************************************
 *   Copyright (C) 2019-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 2 of the License, or     *
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

#ifndef FCLANGCAT_H
#define FCLANGCAT_H

#ifdef __cplusplus
extern "C" {
#endif

struct fc_lang_catalog
{
	const char* lang_code;
	const unsigned int char_set_sz;
	const unsigned int* char_set;
};

// FontConfig languages symbols database.
// Language code is a locale name in 2 or 3 letter code in ISO 639 and ISO 3166-1 alpha-2.
extern const struct fc_lang_catalog fc_lang_cat[];
extern const unsigned int fc_lang_cat_sz;

/**
 * @brief Find language in database by code
 * @param lang_code language code in 2 or 3 letter code in ISO 639 and ISO 3166-1 alpha-2
 * @return Pointer to fc_lang_catalog instance if language found, NULL otherwise.
 */
const struct fc_lang_catalog* fc_lang_cat_find(const char* lang_code);

#ifdef __cplusplus
}
#endif

#endif // FCLANGCAT_H
