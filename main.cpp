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

#include <QApplication>

#if defined(_WIN32) && defined(QT_STATIC)
	#include <QtPlugin>

	Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
	Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	// Qt6 does this by default.
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

	// Set some properties which will be useful for saving settings with QSettings.
	QApplication::setApplicationName("ClownCompTool");
	QApplication::setApplicationVersion("1.2.2");
	QApplication::setOrganizationDomain("clownacy.com");
	QApplication::setOrganizationName("clownacy");

	// Set global application icon, so that every window and dialogue will show it.
	QApplication::setWindowIcon(QIcon(":/Icon/assets/logo.svg"));

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
