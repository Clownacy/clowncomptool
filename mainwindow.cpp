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

#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>

#include "compression.h"
#include "utilities.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setAcceptDrops(true);

	// Set-up the worker thread.
	Compression::Worker *worker = new Compression::Worker;
	worker->moveToThread(&worker_thread);
	connect(&worker_thread, &QThread::finished, worker, &QObject::deleteLater);
	connect(this, &MainWindow::processFile, worker, &Compression::Worker::processFile);
	connect(worker, &Compression::Worker::processingComplete, this, &MainWindow::processingComplete);
	worker_thread.start();

	// Disable parts of the interface by default.
	ui->pushButton_Compress->setEnabled(false);
	ui->pushButton_Decompress->setEnabled(false);
	ui->progressBar->setMaximum(1);

	// Wire-up signals and slots.
	connect(ui->pushButton_InputBrowse, &QPushButton::clicked, this,
		[this]()
		{
			const QString input_filename = QFileDialog::getOpenFileName(this);

			if (!input_filename.isEmpty())
				ui->lineEdit_Input->setText(input_filename);
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
			beginProcessingFile(false);
		}
	);

	connect(ui->pushButton_Decompress, &QPushButton::clicked, this,
		[this]()
		{
			beginProcessingFile(true);
		}
	);

	// Load settings.
	QSettings settings;
	ui->lineEdit_Input->setText(settings.value("Path", "").toString());
	ui->comboBox_Format->setCurrentText(settings.value("Format", "Kosinski").toString());
	ui->checkBox_Moduled->setChecked(settings.value("Moduled", false).toBool());
}

MainWindow::~MainWindow()
{
	// Save settings.
	QSettings settings;
	settings.setValue("Path", ui->lineEdit_Input->text());
	settings.setValue("Format", ui->comboBox_Format->currentText());
	settings.setValue("Moduled", ui->checkBox_Moduled->isChecked());

	worker_thread.quit();
	worker_thread.wait();

	delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData* mimeData = event->mimeData();

	if (mimeData->hasUrls() && mimeData->urls().size() == 1)
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (mimeData->hasUrls())
	{
		const auto &urls = mimeData->urls();

		if (urls.size() == 1)
			ui->lineEdit_Input->setText(urls.front().toString(QUrl::PreferLocalFile));
	}
}

void MainWindow::beginProcessingFile(const bool decompress)
{
	const Compression::Format* const format = Compression::FindFormat(ui->comboBox_Format->currentText());

	if (format == nullptr)
		return;

	const bool moduled = ui->checkBox_Moduled->isChecked();
	const auto extension = decompress ? "unc" : moduled ? format->extension_moduled : format->extension_normal;

	const QString output_filename_hint = Utilities::QStringFromPath(Utilities::PathFromQString(ui->lineEdit_Input->text()).replace_extension(extension));
	const QString output_filename = QFileDialog::getSaveFileName(this, {}, output_filename_hint);

	if (output_filename.isEmpty())
		return;

	// Disable the UI to give the user at least some idea that something is happening in the background.
	ui->controls->setEnabled(false);
	ui->progressBar->setMaximum(0);

	emit processFile(
		format,
		decompress,
		ui->lineEdit_Input->text(),
		output_filename,
		moduled
	);
}

void MainWindow::processingComplete(const bool success, const bool decompress)
{
	// Let the user use the interface again.
	ui->controls->setEnabled(true);
	ui->progressBar->setMaximum(1);

	if (success)
		QMessageBox::information(this, windowTitle(), decompress ? "File decompressed successfully." : "File compressed successfully.");
	else
		QMessageBox::warning(this, windowTitle(), decompress ? "Could not decompress file." : "Could not compress file.");
}
