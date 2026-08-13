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

#include "compression.h"

#include <fstream>

#include "mdcomp/comper.hh"
#include "mdcomp/comperx.hh"
#include "mdcomp/enigma.hh"
#include "mdcomp/kosinski.hh"
#include "mdcomp/kosplus.hh"
#include "mdcomp/nemesis.hh"
#include "mdcomp/rocket.hh"
#include "mdcomp/saxman.hh"

#include "utilities.h"

namespace Compression {

static const Format formats[] = {
	{
		"Kosinski",
		".kos", ".kosm",
		[](auto &input, auto &output)
		{
			return kosinski::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosinski::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosinski::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosinski::moduled_decode(input, output);
		}
	},
	{
		"Enigma",
		".eni", ".enim",
		[](auto &input, auto &output)
		{
			return enigma::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return enigma::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return enigma::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return enigma::moduled_decode(input, output);
		}
	},
	{
		"Nemesis",
		".nem", ".nemm",
		[](auto &input, auto &output)
		{
			return nemesis::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return nemesis::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return nemesis::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return nemesis::moduled_decode(input, output);
		}
	},
	{
		"Saxman",
		".sax", ".saxm",
		[](auto &input, auto &output)
		{
			return saxman::encode(input, output, true);
		},
		[](auto &input, auto &output)
		{
			return saxman::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return saxman::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return saxman::moduled_decode(input, output);
		}
	},
	{
		"Saxman (No Header)",
		".sax", ".saxm",
		[](auto &input, auto &output)
		{
			return saxman::encode(input, output, false);
		},
		[](auto &input, auto &output)
		{
			// Must determine size manually.
			input.seekg(0, std::ios::end);
			const auto size = input.tellg();
			input.seekg(0);

			return saxman::decode(input, output, size);
		},
		[](auto &input, auto &output)
		{
			return saxman::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return saxman::moduled_decode(input, output);
		}
	},
	{
		"Comper",
		".comp", ".compm",
		[](auto &input, auto &output)
		{
			return comper::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comper::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comper::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comper::moduled_decode(input, output);
		}
	},
	{
		"Comper-X",
		".compx", ".compxm",
		[](auto &input, auto &output)
		{
			return comperx::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comperx::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comperx::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comperx::moduled_decode(input, output);
		}
	},
	{
		"Rocket",
		".rock", ".rockm",
		[](auto &input, auto &output)
		{
			return rocket::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return rocket::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return rocket::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return rocket::moduled_decode(input, output);
		}
	},
	{
		"Kosinski+",
		".kosp", ".kospm",
		[](auto &input, auto &output)
		{
			return kosplus::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosplus::decode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosplus::moduled_encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosplus::moduled_decode(input, output);
		}
	},
};

void Worker::processFile(const Format* const format, const bool decompress, const QString input_file_path, const QString output_file_path, const bool moduled)
{
	const auto &Attempt = [&]()
	{
		try
		{
			std::ifstream input_stream(Utilities::PathFromQString(input_file_path), std::ios::binary);

			if (!input_stream.is_open())
				return false;

			std::fstream output_stream(Utilities::PathFromQString(output_file_path), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

			if (!output_stream.is_open())
				return false;

			if (moduled)
			{
				if (decompress)
					return format->decompress_moduled(input_stream, output_stream);
				else
					return format->compress_moduled(input_stream, output_stream);
			}
			else
			{
				if (decompress)
					return format->decompress(input_stream, output_stream);
				else
					return format->compress(input_stream, output_stream);
			}

			return false;
		}
		catch (...)
		{
			return false;
		}
	};

	emit processingComplete(Attempt(), decompress);
}

const Format* FindFormatFromName(const QString &name)
{
	for (const auto &format : formats)
		if (format.name == name)
			return &format;

	return nullptr;
};

const Format* FindFormatFromExtension(const QString &extension, bool &moduled)
{
	const QString &extension_lowercase = extension.toLower();

	for (const auto &format : formats)
	{
		if (format.extension_normal == extension_lowercase)
		{
			moduled = false;
			return &format;
		}
		else if (format.extension_moduled == extension_lowercase)
		{
			moduled = true;
			return &format;
		}
	}

	return nullptr;
}

}
