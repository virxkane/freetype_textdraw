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

#ifndef CLOCALEDATA_H
#define CLOCALEDATA_H

#include <QtCore/QString>

class CLocaleData
{
public:
	CLocaleData(const QString& langtag);
	bool isValid() const {
		return m_isValid;
	}
	QString langTag() const;
	const QString& langCode() const {
		return m_lang_code;
	}
	const QString& langPart2B() const {
		return m_lang_part2b;
	}
	const QString& langPart2T() const {
		return m_lang_part2t;
	}
	const QString& langPart1() const {
		return m_lang_part1;
	}
	const QString& langName() const {
		return m_lang_name;
	}
	const QString& scriptCode() const {
		return m_script_code;
	}
	const QString& scriptName() const {
		return m_script_name;
	}
	const QString& scriptAlias() const {
		return m_script_name;
	}
	unsigned int scriptNumeric() const {
		return m_script_num;
	}
	const QString& regionAlpha2() const {
		return m_region_alpha2;
	}
	const QString& regionAlpha3() const {
		return m_region_alpha3;
	}
	const QString& regionName() const {
		return m_region_name;
	}
	unsigned int regionNumeric() const {
		return m_region_num;
	}
	int calcMatch(const CLocaleData& other) const;
protected:
	void parseTag(const QString& langtag);
private:
	bool m_isValid;

	QString m_langtag_src;			// full langtag (source)
	QString m_lang_code;			// ISO 639-3 lang id/code
	QString m_lang_part2b;			// Equivalent 639-2 identifier of the bibliographic application code set, if there is one
	QString m_lang_part2t;			// Equivalent 639-2 identifier of the terminology application code set, if there is one
	QString m_lang_part1;			// Equivalent 639-1 identifier, if there is one
	QString m_lang_name;			// Reference language name

	QString m_script_code;			// script code
	QString m_script_name;			// script name (english)
	QString m_script_alias;			// script alias
	unsigned int m_script_num;		// script: numeric

	QString m_region_name;			// region name (english)
	QString m_region_alpha2;		// region: alpha2 code
	QString m_region_alpha3;		// region: alpha2 code
	unsigned int m_region_num;		// script: numeric
};

#endif // CLOCALEDATA_H
