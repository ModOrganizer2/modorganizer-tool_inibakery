#include "inibakery.h"
#include <iplugingame.h>
#include <localsavegames.h>
#include <utility.h>
#include <QFile>
#include <QCoreApplication>
#include <QStringList>
#include <QtPlugin>
#include <functional>


using namespace MOBase;


bool IniBakery::init(MOBase::IOrganizer *moInfo)
{
  using namespace std::placeholders;
  m_MOInfo = moInfo;
  m_MOInfo->onAboutToRun(std::bind(&IniBakery::prepareIni, this, _1));

  return true;
}

QString IniBakery::name() const
{
  return "Ini Bakery";
}

QString IniBakery::author() const
{
  return "Tannin";
}

QString IniBakery::description() const
{
  return tr("Manages game ini files");
}

MOBase::VersionInfo IniBakery::version() const
{
  return VersionInfo(0, 0, 2, VersionInfo::RELEASE_ALPHA);
}

bool IniBakery::isActive() const
{
  return true;
}

QList<PluginSetting> IniBakery::settings() const
{
  return QList<PluginSetting>();
}

QStringList IniBakery::iniFileNames() const
{
  return m_MOInfo->managedGame()->iniFiles();
}

bool IniBakery::prepareIni(const QString&)
{
  const IPluginGame *game = qApp->property("managed_game").value<IPluginGame*>();

  game = m_MOInfo->managedGame();
  // import global ini files if they don't exist in the profile yet
  for (const QString &iniFile : iniFileNames()) {
    QString profileIni = m_MOInfo->profilePath() + "/" + iniFile;

    if (!QFile::exists(profileIni)) {
      shellCopy(game->documentsDirectory().absoluteFilePath(iniFile),
                profileIni);
    }
  }

  IProfile *profile = m_MOInfo->profile();

  QString basePath
      = profile->localSettingsEnabled()
            ? profile->absolutePath()
            : m_MOInfo->managedGame()->documentsDirectory().absolutePath();

  QString profileIni = basePath + "/" + iniFileNames()[0];

  WritePrivateProfileStringW(L"Launcher", L"bEnableFileSelection", L"1",
                             profileIni.toStdWString().c_str());

  WritePrivateProfileStringW(L"Archive", L"bInvalidateOlderFiles", L"1",
                             profileIni.toStdWString().c_str());
  LocalSavegames *savegames = game->feature<LocalSavegames>();
  if (savegames != nullptr) {
    savegames->prepareProfile(m_MOInfo->profile());
  }

  if (m_MOInfo->managedGame()->name() == "Fallout 4") {
    static std::set<QString> modDirs = { "STRINGS", "TEXTURES", "MUSIC", "SOUND",
                                         "INTERFACE", "MESHES", "PROGRAMS",
                                         "MATERIALS", "LODSETTINGS", "VIS", "MISC",
                                         "SCRIPTS", "SHADERSFX", "VIDEO" };

    QStringList moddedDataDirs;
    for (const QString &dir : m_MOInfo->listDirectories("")) {
      QString dirU = dir.toUpper();
      if (modDirs.find(dirU) != modDirs.end()) {
        moddedDataDirs.append(dirU + "\\");
      }
    }

    WritePrivateProfileStringW(L"Archive", L"sResourceDataDirsFinal",
                               moddedDataDirs.join(",").toStdWString().c_str(),
                               profileIni.toStdWString().c_str());
  }
  return true;
}


MappingType IniBakery::mappings() const
{
  MappingType result;

  IPluginGame *game = qApp->property("managed_game").value<IPluginGame *>();

  IProfile *profile = m_MOInfo->profile();

  if (profile->localSettingsEnabled()) {
    for (const QString &iniFile : iniFileNames()) {
      result.push_back({m_MOInfo->profilePath() + "/" + iniFile,
                        game->documentsDirectory().absoluteFilePath(iniFile),
                        false, false});
    }
  }

  return result;
}
