// Copyright (C) 2026 Clownacy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <functional>
#include <iostream>

#include <QObject>
#include <QString>

namespace Compression
{
	struct Format
	{
		const char *name, *extension_normal, *extension_moduled;
		std::function<bool(std::istream&, std::iostream&)> compress, decompress, compress_moduled, decompress_moduled;
	};

	class Worker : public QObject
	{
		Q_OBJECT

	public slots:
		void processFile(const Format *format, bool decompress, QString input_file_path, QString output_file_path, bool moduled);

	signals:
		void processingComplete(bool result, bool decompress);
	};

	const Format* FindFormatFromName(const QString &name);
	const Format* FindFormatFromExtension(const QString &extension, bool &moduled);
}

#endif // COMPRESSION_H
