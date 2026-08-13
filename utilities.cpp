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

#include "utilities.h"

namespace Utilities {

std::filesystem::path PathFromQString(const QString &string)
{
	const auto &utf8_string = string.toUtf8();
	return std::u8string_view(reinterpret_cast<const char8_t*>(utf8_string.data()), utf8_string.size());
};

QString QStringFromPath(const std::filesystem::path &path)
{
	const auto &utf8_string = path.u8string();
	return QString::fromUtf8(reinterpret_cast<const char*>(utf8_string.data()), utf8_string.size());
};

QString ReplaceFileExtension(const QString &path, const char* const extension)
{
	return Utilities::QStringFromPath(Utilities::PathFromQString(path).replace_extension(extension));
}

}
