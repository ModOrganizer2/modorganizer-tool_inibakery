#include "inibakery.h"
#include <iplugingame.h>
#include <localsavegames.h>
#include <bsainvalidation.h>
#include <utility.h>
#include "registry.h"
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
  return "INI Bakery";
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
  return VersionInfo(0, 2, 0, VersionInfo::RELEASE_FINAL);
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

  IProfile *profile = m_MOInfo->profile();

  QString basePath
      = profile->localSettingsEnabled()
            ? profile->absolutePath()
            : m_MOInfo->managedGame()->documentsDirectory().absolutePath();

  if (!iniFileNames().isEmpty()) {

    QString profileIni = basePath + "/" + iniFileNames()[0];

    WCHAR setting[512];
    if (!GetPrivateProfileStringW(L"Launcher", L"bEnableFileSelection", L"0", setting, 512, profileIni.toStdWString().c_str())
      || wcstol(setting, nullptr, 10) != 1) {
      MOBase::WriteRegistryValue(L"Launcher", L"bEnableFileSelection", L"1", profileIni.toStdWString().c_str());
    }
  }

  LocalSavegames *savegames = game->feature<LocalSavegames>();
  if (savegames != nullptr) {
    savegames->prepareProfile(m_MOInfo->profile());
  }

  BSAInvalidation *invalidation = game->feature<BSAInvalidation>();
  if (invalidation != nullptr) {
    invalidation->prepareProfile(m_MOInfo->profile());
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
      result.push_back({m_MOInfo->profilePath() + "/" + QFileInfo(iniFile).fileName(),
                        game->documentsDirectory().absoluteFilePath(iniFile),
                        false, false});
    }
  }

  return result;
}
