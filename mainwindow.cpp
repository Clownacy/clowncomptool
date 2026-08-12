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

#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>

#include "mdcomp/comper.hh"
#include "mdcomp/comperx.hh"
#include "mdcomp/enigma.hh"
#include "mdcomp/kosinski.hh"
#include "mdcomp/kosplus.hh"
#include "mdcomp/nemesis.hh"
#include "mdcomp/rocket.hh"
#include "mdcomp/saxman.hh"

static std::filesystem::path PathFromQString(const QString &string)
{
	const auto &utf8_string = string.toUtf8();
	return std::u8string_view(reinterpret_cast<const char8_t*>(utf8_string.data()), utf8_string.size());
};

static QString QStringFromPath(const std::filesystem::path &path)
{
	const auto &utf8_string = path.u8string();
	return QString::fromUtf8(reinterpret_cast<const char*>(utf8_string.data()), utf8_string.size());
};

class Worker : public QObject
{
	Q_OBJECT

public slots:
	void processFile(const Format* const format, const bool decompress, const QString input_file_path, const QString output_file_path, const bool moduled)
	{
		const auto &Attempt = [&]()
		{
			try
			{
				std::ifstream input_stream(PathFromQString(input_file_path), std::ios::binary);

				if (!input_stream.is_open())
					return false;

				std::fstream output_stream(PathFromQString(output_file_path), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

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

signals:
	void processingComplete(bool result, bool decompress);
};

#include "mainwindow.moc"

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
		"eni", "enim",
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
		"nem", "nemm",
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
		"sax", "saxm",
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
		"sax", "saxm",
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
		"comp", "compm",
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
		"compx", "compxm",
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
		"rock", "rockm",
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
		"kosp", "kospm",
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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setAcceptDrops(true);

	// Set-up the worker thread.
	Worker *worker = new Worker;
	worker->moveToThread(&worker_thread);
	connect(&worker_thread, &QThread::finished, worker, &QObject::deleteLater);
	connect(this, &MainWindow::processFile, worker, &Worker::processFile);
	connect(worker, &Worker::processingComplete, this, &MainWindow::processingComplete);
	worker_thread.start();

	// Disable parts of the interface by default.
	ui->pushButton_Compress->setEnabled(false);
	ui->pushButton_Decompress->setEnabled(false);
	ui->progressBar->setMaximum(1);

	// Load settings.
	QSettings settings;
	ui->lineEdit_Input->setText(settings.value("Path", "").toString());
	ui->comboBox_Format->setCurrentText(settings.value("Format", "Kosinski").toString());
	ui->checkBox_Moduled->setChecked(settings.value("Moduled", false).toBool());

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

	const bool moduled = ui->checkBox_Moduled->isChecked();
	const auto extension = decompress ? "unc" : moduled ? format->extension_moduled : format->extension_normal;

	const QString output_filename_hint = QStringFromPath(PathFromQString(ui->lineEdit_Input->text()).replace_extension(extension));
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
