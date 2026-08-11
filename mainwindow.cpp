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

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>
#include <fstream>

#include <QFileDialog>
#include <QMessageBox>

#include "mdcomp/comper.hh"
#include "mdcomp/comperx.hh"
#include "mdcomp/enigma.hh"
#include "mdcomp/kosinski.hh"
#include "mdcomp/kosplus.hh"
#include "mdcomp/nemesis.hh"
#include "mdcomp/rocket.hh"
#include "mdcomp/saxman.hh"

struct Format
{
	const char *name, *extension_normal, *extension_moduled;
	std::function<bool(std::istream&, std::iostream&)> compress, decompress;
	std::function<bool(std::istream&, std::iostream&, std::size_t)> compress_moduled, decompress_moduled;
};

static const Format formats[] = {
	{
		"Kosinski",
		"kos", "kosm",
		[](auto &input, auto &output)
		{
			return kosinski::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosinski::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return kosinski::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return kosinski::moduled_decode(input, output, module_size);
		}
	},
	{
		"Enigma",
		"eni", "enim",
		[](auto &input, auto &output)
		{
			return enigma::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return enigma::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return enigma::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return enigma::moduled_decode(input, output, module_size);
		}
	},
	{
		"Nemesis",
		"nem", "nemm",
		[](auto &input, auto &output)
		{
			return nemesis::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return nemesis::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return nemesis::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return nemesis::moduled_decode(input, output, module_size);
		}
	},
	{
		"Saxman",
		"sax", "saxm",
		[](auto &input, auto &output)
		{
			return saxman::encode(input, output, true); // TODO: Do something about the header!
		},
		[](auto &input, auto &output)
		{
			return saxman::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return saxman::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return saxman::moduled_decode(input, output, module_size);
		}
	},
	{
		"Comper",
		"comp", "compm",
		[](auto &input, auto &output)
		{
			return comper::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comper::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return comper::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return comper::moduled_decode(input, output, module_size);
		}
	},
	{
		"Comper-X",
		"compx", "compxm",
		[](auto &input, auto &output)
		{
			return comperx::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return comperx::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return comperx::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return comperx::moduled_decode(input, output, module_size);
		}
	},
	{
		"Rocket",
		"rock", "rockm",
		[](auto &input, auto &output)
		{
			return rocket::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return rocket::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return rocket::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return rocket::moduled_decode(input, output, module_size);
		}
	},
	{
		"Kosinski+",
		"kosp", "kospm",
		[](auto &input, auto &output)
		{
			return kosplus::encode(input, output);
		},
		[](auto &input, auto &output)
		{
			return kosplus::decode(input, output);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return kosplus::moduled_encode(input, output, module_size);
		},
		[](auto &input, auto &output, auto module_size)
		{
			return kosplus::moduled_decode(input, output, module_size);
		}
	},
};

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Disable parts of the interface by default
	ui->spinBox_ModuleSize->setEnabled(false);
	ui->pushButton_Compress->setEnabled(false);
	ui->pushButton_Decompress->setEnabled(false);

	// Wire-up signals and slots.
	connect(ui->pushButton_InputBrowse, &QPushButton::clicked, this,
		[this]()
		{
			const QString input_filename = QFileDialog::getOpenFileName(this);

			if (!input_filename.isEmpty())
				ui->lineEdit_Input->setText(input_filename);
		}
	);

	connect(ui->checkBox_Moduled, &QCheckBox::stateChanged, this,
		[this](int state)
		{
			ui->spinBox_ModuleSize->setEnabled(state == Qt::CheckState::Checked);
		}
	);

	connect(ui->lineEdit_Input, &QLineEdit::textChanged, this,
		[this](const QString &string)
		{
			const bool enabled = !string.isEmpty();
			ui->pushButton_Compress->setEnabled(enabled);
			ui->pushButton_Decompress->setEnabled(enabled);
		}
	);

	connect(ui->pushButton_Compress, &QPushButton::clicked, this,
		[this]()
		{
			ProcessFile(false);
		}
	);

	connect(ui->pushButton_Decompress, &QPushButton::clicked, this,
		[this]()
		{
			ProcessFile(true);
		}
	);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::ProcessFile(const bool decompress)
{
	const auto &FindFormat = [&]() -> const Format*
	{
		for (const auto &format : formats)
			if (format.name == ui->comboBox_Format->currentText())
				return &format;

		// TODO: An assert?
		return nullptr;
	};

	const auto &format = FindFormat();

	if (format == nullptr)
		return;

	const auto &PathFromQString = [](const QString &string)
	{
		const auto &utf8_string = string.toUtf8();
		return std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(utf8_string.data()), utf8_string.size()));
	};

	const auto &QStringFromPath = [](const std::filesystem::path &path)
	{
		const auto &utf8_string = path.u8string();
		return QString::fromUtf8(reinterpret_cast<const char*>(utf8_string.data()), utf8_string.size());
	};

	const bool moduled = ui->checkBox_Moduled->isChecked();
	const auto extension = decompress ? "unc" : moduled ? format->extension_moduled : format->extension_normal;

	const QString output_filename_hint = QStringFromPath(PathFromQString(ui->lineEdit_Input->text()).replace_extension(extension));
	const QString output_filename = QFileDialog::getSaveFileName(this, {}, output_filename_hint);

	if (output_filename.isEmpty())
		return;

	// TODO: Run this on another thread to avoid freezing the UI.
	const auto &Attempt = [&]()
	{
		try
		{
			std::ifstream input_stream(PathFromQString(ui->lineEdit_Input->text()), std::ios::binary);
			std::fstream output_stream(PathFromQString(output_filename), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

			const auto module_size = ui->spinBox_ModuleSize->value();

			if (moduled)
			{
				if (decompress)
					return format->decompress_moduled(input_stream, output_stream, module_size);
				else
					return format->compress_moduled(input_stream, output_stream, module_size);
			}
			else
			{
				if (decompress)
					return format->decompress(input_stream, output_stream);
				else
					return format->compress(input_stream, output_stream);
			}
		}
		catch (...)
		{
		}

		return false;
	};

	if (Attempt())
		QMessageBox::information(this, windowTitle(), decompress ? "File decompressed successfully." : "File compressed successfully.");
	else
		QMessageBox::warning(this, windowTitle(), decompress ? "Could not decompress file." : "Could not compress file.");
}
