/* ============================================================
* QupZilla - WebKit based browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "commandlineoptions.h"
#include "browserwindow.h"
#include "mainapplication.h"

#include <QCoreApplication>
#include <QFileInfo>
#define tr(s) QObject::tr(s)

CommandLineOptions::CommandLineOptions(int &argc)
    : m_argc(argc)
{
    parseActions();
}

CommandLineOptions::ActionPairList CommandLineOptions::getActions()
{
    return m_actions;
}

void CommandLineOptions::showHelp()
{
    using namespace std;

    const QString orw = tr("or");
    const QString help = " " + tr("Usage:") + " qupzilla " + "[options] URL" +"\n" +
                       "\n" +
                       " " + tr("QupZilla options:") + "\n" +
                       "    -h " + orw + " --help                        " + tr("print this message") + "\n" +
                       "    -a " + orw + " --authors                     " + tr("print QupZilla authors") + "\n" +
                       "    -v " + orw + " --version                     " + tr("print QupZilla version") + "\n" +
                       "\n" +
                       "    -p=" + tr("PROFILE") + " " + orw + " --profile=" + tr("PROFILE") + "     " + tr("start with specified profile") + "\n" +
                       "    -ne " + orw + " --no-extensions              " + tr("start without extensions") + "\n" +
                       "    -po " + orw + " --portable                   " + tr("start in portable mode") + "\n" +
                       "\n" +
                       " " + tr("Options to control running QupZilla:") + "\n" +
                       "    -nt " + orw + " --new-tab                    " + tr("open new tab") + "\n" +
                       "    -nw " + orw + " --new-window                 " + tr("open new window") + "\n" +
                       "    -pb " + orw + " --private-browsing           " + tr("start private browsing") + "\n" +
                       "    -dm " + orw + " --download-manager           " + tr("show download manager") + "\n" +
                       "    -fs " + orw + " --fullscreen                 " + tr("toggle fullscreen") + "\n" +
                       "    -nr " + orw + " --no-remote                  " + tr("open new browser instance") + "\n" +
                       "    -ct=" + tr("URL") + " " + orw + " --current-tab=" + tr("URL") + "        " + tr("open URL in current tab") + "\n" +
                       "    -ow=" + tr("URL") + " " + orw + " --open-window=" + tr("URL") + "        " + tr("open URL in new window") + "\n" +
                       "\n" +
                       " " + tr("QupZilla is a new, fast and secure web browser") + "\n" +
                       " " + tr("based on WebKit core (http://webkit.org) and") + "\n" +
                       " " + tr("written in Qt Framework (http://qt-project.org/)") + "\n\n" +
                       " " + tr("For more information please visit wiki at") + "\n" +
                       " https://github.com/QupZilla/qupzilla/wiki\n";

    cout << help.toUtf8().data() << " > " << Qz::WWWADDRESS.toUtf8().data() << endl;
}

void CommandLineOptions::parseActions()
{
    using namespace std;

    const QStringList arguments = QCoreApplication::arguments();
    if (arguments.isEmpty()) {
        return;
    }

    // Skip first argument (it should be program itself)
    for (int i = 1; i < arguments.count(); ++i) {
        QString arg = arguments.at(i);

        if (arg == QLatin1String("-h") || arg == QLatin1String("--help")) {
            showHelp();
            ActionPair pair;
            pair.action = Qz::CL_ExitAction;
            m_actions.append(pair);
            break;
        }

        if (arg == QLatin1String("-a") || arg == QLatin1String("--authors")) {
            cout << tr("QupZilla authors: ").toUtf8().data() << endl;
            cout << "  David Rosca <nowrep@gmail.com>" << endl;
            ActionPair pair;
            pair.action = Qz::CL_ExitAction;
            m_actions.append(pair);
            break;
        }

        if (arg == QLatin1String("-v") || arg == QLatin1String("--version")) {
            cout << "QupZilla v" << Qz::VERSION.toUtf8().data()
#ifdef GIT_REVISION
                 << " rev " << GIT_REVISION << " "
#endif
                 << "(build " << Qz::BUILDTIME.toUtf8().data() << ")"
                 << endl;
            ActionPair pair;
            pair.action = Qz::CL_ExitAction;
            m_actions.append(pair);
            break;
        }

        if (arg.startsWith(QLatin1String("-p=")) || arg.startsWith(QLatin1String("--profile="))) {
            int index = arg.indexOf(QLatin1Char('='));
            if (index != -1) {
                const QString profileName = arg.mid(index + 1);
                cout << tr("QupZilla: Starting with profile").toUtf8().data() << " '" << profileName.toUtf8().data() << "'" << endl;

                ActionPair pair;
                pair.action = Qz::CL_StartWithProfile;
                pair.text = profileName;
                m_actions.append(pair);
            }
        }

        if (arg.startsWith(QLatin1String("-ne")) || arg.startsWith(QLatin1String("--no-extensions"))) {
            ActionPair pair;
            pair.action = Qz::CL_StartWithoutAddons;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-nt")) || arg.startsWith(QLatin1String("--new-tab"))) {
            ActionPair pair;
            pair.action = Qz::CL_NewTab;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-nw")) || arg.startsWith(QLatin1String("--new-window"))) {
            ActionPair pair;
            pair.action = Qz::CL_NewWindow;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-dm")) || arg.startsWith(QLatin1String("--download-manager"))) {
            ActionPair pair;
            pair.action = Qz::CL_ShowDownloadManager;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-pb")) || arg.startsWith(QLatin1String("--private-browsing"))) {
            ActionPair pair;
            pair.action = Qz::CL_StartPrivateBrowsing;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-nr")) || arg.startsWith(QLatin1String("--no-remote"))) {
            ActionPair pair;
            pair.action = Qz::CL_StartNewInstance;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-po")) || arg.startsWith(QLatin1String("--portable"))) {
            ActionPair pair;
            pair.action = Qz::CL_StartPortable;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-fs")) || arg.startsWith(QLatin1String("--fullscreen"))) {
            ActionPair pair;
            pair.action = Qz::CL_ToggleFullScreen;
            m_actions.append(pair);
        }

        if (arg.startsWith(QLatin1String("-ct")) || arg.startsWith(QLatin1String("--current-tab"))) {
            int index = arg.indexOf(QLatin1Char('='));
            if (index != -1) {
                ActionPair pair;
                pair.action = Qz::CL_OpenUrlInCurrentTab;
                pair.text = arg.mid(index + 1);
                m_actions.append(pair);
            }
        }

        if (arg.startsWith(QLatin1String("-ow")) || arg.startsWith(QLatin1String("--open-window"))) {
            int index = arg.indexOf(QLatin1Char('='));
            if (index != -1) {
                ActionPair pair;
                pair.action = Qz::CL_OpenUrlInNewWindow;
                pair.text = arg.mid(index + 1);
                m_actions.append(pair);
            }
        }
    }

    QString url = arguments.last();
    QFileInfo fileInfo(url);

    if (fileInfo.exists()) {
        url = fileInfo.absoluteFilePath();
    }

    if (m_argc > 1 && !url.isEmpty() && !url.startsWith(QLatin1Char('-')) &&
            (url.contains(QLatin1Char('.')) || url.contains(QLatin1Char('/'))
             || url.contains(QLatin1Char('\\')))) {
        ActionPair pair;
        pair.action = Qz::CL_OpenUrl;
        pair.text = url;
        m_actions.append(pair);
    }
}
