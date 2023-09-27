#include "inibakery.h"
#include <iplugingame.h>
#include <localsavegames.h>
#include <bsainvalidation.h>
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
  m_MOInfo->onAboutToRun([this](const auto& binary) { return prepareIni(binary); });

  return true;
}

QString IniBakery::name() const
{
  return "INI Bakery";
}

QString IniBakery::localizedName() const
{
  return tr("INI Bakery");
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
  return VersionInfo(0, 3, 0, VersionInfo::RELEASE_FINAL);
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
  const IPluginGame *game = m_MOInfo->managedGame();

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

  const IPluginGame *game = m_MOInfo->managedGame();

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
