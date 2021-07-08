/***************************************************************************
 *   Copyright (C) 2021 by Chernov A.A.                                    *
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

#include "clocaledata.h"

#include <QtCore/QStringList>

#include "iso-639-2_data.h"
#include "iso-639-3_data.h"
#include "iso-3166-1_data.h"
#include "iso-15924_data.h"

CLocaleData::CLocaleData(const QString& langtag)
{
	parseTag(langtag);
}

QString CLocaleData::langTag() const
{
	if (m_lang_code.isEmpty())
		return QString();

	QString tag;

	// part 1: main lanuage
	if (!m_lang_part1.isEmpty())
		tag = m_lang_part1;
	else
		tag = m_lang_code;

	// part2: script
	if (!m_script_code.isEmpty())
		tag.append(QString("-%1").arg(m_script_code));

	// part 3: region/country
	if (!m_region_alpha2.isEmpty())
		tag.append(QString("-%1").arg(m_region_alpha2));
	else 	if (!m_region_alpha3.isEmpty())
		tag.append(QString("-%1").arg(m_region_alpha3));

	return tag;
}

int CLocaleData::calcMatch(const CLocaleData& other) const
{
	if (m_lang_code.compare(other.langCode(), Qt::CaseSensitive) != 0)
		return 0;
	int match = 100;
	if (m_script_num != 0 && other.scriptNumeric() != 0) {
		if (m_script_num == other.scriptNumeric())
			match += 10;
		else
			match -= 10;
	}
	if (m_region_num != 0 && other.regionNumeric() != 0) {
		if (m_region_num == other.regionNumeric())
			match += 1;
		else
			match -= 1;
	}
	return match;
}

void CLocaleData::parseTag(const QString& langtag)
{
	m_langtag_src = langtag;
	QString tag = langtag;
	bool lang_ok = false;
	bool script_ok = true;
	bool region_ok = true;

	// 0. clear all fields
	m_isValid = false;
	m_lang_code = QString();
	m_lang_part2b = QString();
	m_lang_part2t = QString();
	m_lang_part1 = QString();
	m_lang_name = QString();

	m_script_code = QString();
	m_script_name = QString();
	m_script_alias = QString();
	m_script_num = 0;

	m_region_name = QString();
	m_region_alpha2 = QString();
	m_region_alpha3 = QString();
	m_region_num = 0;

	// 1. replace '_' with '-'
	tag = tag.replace(QChar('_'), QChar('-'), Qt::CaseSensitive);

	// 2. split to components
	QStringList list = tag.split('-', Qt::KeepEmptyParts);

	// 3. Analyze each part

	/* pass state codes:
	 * 0 - start
	 * 1 - language parsed
	 * 2 - script parsed
	 * 3 - region parsed
	 */
	int state = 0;
	int i;
	bool part_parsed;
	QStringList::const_iterator it;
	for (it = list.begin(); it != list.end(); ++it) {
		const QString& part = *it;
		if (part.isEmpty())
			continue;
		part_parsed = false;
		while (!part_parsed) {
			switch (state) {
				case 0: {
					// search language code in ISO-639-3
					const struct iso639_3_rec* ptr = &iso639_3_data[0];
					const struct iso639_3_rec* rec3 = NULL;
					const struct iso639_2_rec* rec2 = NULL;
					for (i = 0; i < ISO639_3_DATA_SZ; i++) {
						if (part.compare(ptr->id, Qt::CaseInsensitive) == 0) {
							rec3 = ptr;
							break;
						} else if (part.compare(ptr->part1, Qt::CaseInsensitive) == 0) {
							rec3 = ptr;
							break;
						} else if (part.compare(ptr->part2b, Qt::CaseInsensitive) == 0) {
							rec3 = ptr;
							break;
						}
						// don't test Part/2T, ISO-639-3 id already use Part/2T
						ptr++;
					}
					if (!rec3) {
						// if not found by code => search by full name
						ptr = &iso639_3_data[0];
						for (i = 0; i < ISO639_3_DATA_SZ; i++) {
							if (part.compare(ptr->ref_name, Qt::CaseInsensitive) == 0) {
								rec3 = ptr;
								break;
							}
							ptr++;
						}
					}
					if (!rec3) {
						// if not found in ISO-639-3 => search in ISO-639-2 (for scope "collective")
						// search only by id
						const struct iso639_2_rec* ptr = &iso639_2_data[0];
						for (i = 0; i < ISO639_2_DATA_SZ; i++) {
							if (part.compare(ptr->id, Qt::CaseInsensitive) == 0) {
								rec2 = ptr;
								break;
							}
							ptr++;
						}
					}
					if (rec3 || rec2)
						lang_ok = true;
					else if (ISO639_3_UND_INDEX >= 0)
						rec3 = &iso639_3_data[ISO639_3_UND_INDEX];
					if (rec3) {
						m_lang_code = QString(rec3->id);
						m_lang_part2b = rec3->part2b != NULL ? QString(rec3->part2b) : QString();
						m_lang_part2t = rec3->part2t != NULL ? QString(rec3->part2t) : QString();
						m_lang_part1 = rec3->part1 != NULL ? QString(rec3->part1) : QString();
						m_lang_name = QString(rec3->ref_name);
					} else if (rec2) {
						m_lang_code = QString(rec2->id);
						m_lang_part2b = QString();
						m_lang_part2t = rec2->part2t != NULL ? QString(rec2->part2t) : QString();
						m_lang_part1 = rec2->part1 != NULL ? QString(rec2->part1) : QString();
						m_lang_name = QString(rec2->ref_name);
					}
					part_parsed = true;
					state = 1;
					break;
				}
				case 1: {
					// search script in ISO-15924
					if (part.length() == 4) {
						script_ok = false;
						const struct iso15924_rec* ptr = &iso15924_data[0];
						const struct iso15924_rec* rec = NULL;
						for (i = 0; i < ISO15924_DATA_SZ; i++) {
							if (part.compare(ptr->code, Qt::CaseInsensitive) == 0) {
								rec = ptr;
								break;
							}
							ptr++;
						}
						if (rec) {
							// script is found
							m_script_code = QString(rec->code);
							m_script_name = rec->name != NULL ? QString(rec->name) : QString();
							m_script_alias = rec->alias != NULL ? QString(rec->alias) : QString();
							m_script_num = rec->num;
							part_parsed = true;
							script_ok = true;
						}
					}
					state = 2;
					break;
				}
				case 2:
					// search region/country code in ISO-3166-1
					region_ok = false;
					const struct iso3166_1_rec* ptr = &iso3166_1_data[0];
					const struct iso3166_1_rec* rec = NULL;
					for (i = 0; i < ISO3166_1_DATA_SZ; i++) {
						if (part.compare(ptr->alpha2, Qt::CaseInsensitive) == 0) {
							rec = ptr;
							break;
						} else if (part.compare(ptr->alpha3, Qt::CaseInsensitive) == 0) {
							rec = ptr;
							break;
						} else if (part.compare(ptr->name, Qt::CaseInsensitive) == 0) {
							rec = ptr;
							break;
						}
						ptr++;
					}
					if (rec) {
						// region is found
						m_region_name = QString(rec->name);
						m_region_alpha2 = QString(rec->alpha2);
						m_region_alpha3 = QString(rec->alpha3);
						m_region_num = rec->num;
						region_ok = true;
					}
					part_parsed = true;
					state = 1;
					break;
			}
		}
		if (3 == state) {
			// ignore all trailing tags
			break;
		}
	}
	m_isValid = lang_ok && script_ok && region_ok;
}
