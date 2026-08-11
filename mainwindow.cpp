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

#include <QFileDialog>
#include <QMessageBox>

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
			QString input_filename = QFileDialog::getOpenFileName(this, "Select Input File");

			ui->lineEdit_Input->setText(input_filename);
		}
	);

	connect(ui->pushButton_OutputBrowse, &QPushButton::clicked, this,
		[this]()
		{
			QString output_filename = QFileDialog::getSaveFileName(this, "Select Output File");

			ui->lineEdit_Output->setText(output_filename);
		}
	);

	connect(ui->checkBox_Moduled, &QCheckBox::stateChanged, this,
		[this](int state)
		{
			ui->spinBox_ModuleSize->setEnabled(state == Qt::CheckState::Checked);
		}
	);

	const auto &UpdateButtons = [this]()
	{
		const bool enabled = !ui->lineEdit_Input->text().isEmpty() && !ui->lineEdit_Output->text().isEmpty();
		ui->pushButton_Compress->setEnabled(enabled);
		ui->pushButton_Decompress->setEnabled(enabled);
	};

	connect(ui->lineEdit_Input, &QLineEdit::textChanged, this, UpdateButtons);
	connect(ui->lineEdit_Output, &QLineEdit::textChanged, this, UpdateButtons);
}

MainWindow::~MainWindow()
{
	delete ui;
}
