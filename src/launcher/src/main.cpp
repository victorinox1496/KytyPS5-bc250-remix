#include "mainDialog.h"
#include "launcherTheme.h"

#include <QApplication>
#include <QArgument>
#include <QDir>
#include <QFileInfo>
#include <QObject>

#ifdef __linux__
#include <dlfcn.h>
#endif

namespace {

// The released bundle ships a copy of Qt (and its platform plugins) next to
// the executable. When this launcher is built against a newer system Qt,
// loading those stale plugins aborts at startup with Qt_6.NN version errors.
// Force the platform plugin path to the plugin directory of the Qt we are
// actually linked against, so the launcher always runs on a coherent Qt.
void ForceMatchingPlatformPlugins() {
#ifdef __linux__
	if (!qEnvironmentVariableIsEmpty("QT_PLUGIN_PATH")) {
		return;
	}
	Dl_info info {};
	if (dladdr(reinterpret_cast<void*>(&QCoreApplication::quit), &info) == 0) {
		return;
	}
	const auto build_plugins =
	    QFileInfo(QString::fromLocal8Bit(info.dli_fname)).absolutePath() + QStringLiteral("/qt6/plugins");
	if (QDir(build_plugins).exists()) {
		qputenv("QT_PLUGIN_PATH", build_plugins.toLocal8Bit());
	}
#else
	(void)0;
#endif
}

} // namespace

int main(int argc, char* argv[]) {
	ForceMatchingPlatformPlugins();
	QApplication a(argc, argv);
	LauncherTheme::Initialize(a);

	MainDialog w;

	QObject::connect(&a, &QApplication::aboutToQuit, &w, &MainDialog::Quit);

	w.emit Start();

	w.show();

	return QApplication::exec();
}
