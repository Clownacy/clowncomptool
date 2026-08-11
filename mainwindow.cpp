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

#include "mdcomp/kosinski.hh"

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
	const QString output_filename = QFileDialog::getSaveFileName(this);

	if (output_filename.isEmpty())
		return;

	// TODO: Run this on another thread to avoid freezing the UI.
	const auto &Attempt = [&]()
	{
		try
		{
			const auto &PathFromQString = [](const QString &string)
			{
				const auto &utf8_string = string.toUtf8();
				return std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(utf8_string.data()), utf8_string.size()));
			};

			std::ifstream input_stream(PathFromQString(ui->lineEdit_Input->text()), std::ios::binary);
			std::fstream output_stream(PathFromQString(output_filename), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

			const QString &format = ui->comboBox_Format->currentText();
			const bool moduled = ui->checkBox_Moduled->isChecked();
			const auto module_size = ui->spinBox_ModuleSize->value();

			if (format == "Kosinski")
			{
				if (moduled)
				{
					if (decompress)
						return kosinski::moduled_decode(input_stream, output_stream, module_size);
					else
						return kosinski::moduled_encode(input_stream, output_stream, module_size);
				}
				else
				{
					if (decompress)
						return kosinski::decode(input_stream, output_stream);
					else
						return kosinski::encode(input_stream, output_stream);
				}
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
