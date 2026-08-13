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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <filesystem>

#include <QString>

namespace Utilities
{
	std::filesystem::path PathFromQString(const QString &string);
	QString QStringFromPath(const std::filesystem::path &path);

	QString ReplaceFileExtension(const QString &path, const char *extension);
	QString GetFileExtension(const QString &path);
}

#endif // UTILITIES_H
