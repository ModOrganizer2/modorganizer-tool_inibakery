#include "inibakery.h"
#include <igameinfo.h>
#include <iplugingame.h>
#include <utility.h>
#include <QFile>
#include <QCoreApplication>
#include <QtPlugin>


using namespace MOBase;


bool IniBakery::init(MOBase::IOrganizer *moInfo)
{
  m_MOInfo = moInfo;

  m_MOInfo->onAboutToRun([this] (const QString&) -> bool {
    // bake the ini on start

    QString profileIni = profileIniPath();
    if (!QFile::exists(profileIni)) {
      shellCopy(gameIniPath(), profileIni);
    }

    WritePrivateProfileStringW(L"Archive", L"sResourceDataDirsFinal",
                               L"STRINGS\\, INTERFACE\\, TEXTURES\\, MESHES\\",
                               profileIni.toStdWString().c_str());
    return true;
  });

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
  return VersionInfo(0, 0, 1, VersionInfo::RELEASE_ALPHA);
}

bool IniBakery::isActive() const
{
  return true;
}

QList<PluginSetting> IniBakery::settings() const
{
  return QList<PluginSetting>();
}

QString IniBakery::iniFileName() const
{
  switch (m_MOInfo->gameInfo().type()) {
    case IGameInfo::TYPE_OBLIVION:  return "oblivion.ini";
    case IGameInfo::TYPE_FALLOUT3:  return "fallout.ini";
    case IGameInfo::TYPE_FALLOUT4:  return "fallout4.ini";
    case IGameInfo::TYPE_FALLOUTNV: return "fallout.ini";
    case IGameInfo::TYPE_SKYRIM:    return "skyrim.ini";
    default: throw std::runtime_error("unsupported game");
  }
}

QString IniBakery::profileIniPath() const
{
  return m_MOInfo->profilePath() + "/" + iniFileName();
}

QString IniBakery::gameIniPath() const
{
  IPluginGame *game = qApp->property("managed_game").value<IPluginGame*>();
  return game->documentsDirectory().absoluteFilePath(iniFileName());
}

MappingType IniBakery::mappings() const
{

  return { { profileIniPath(), gameIniPath(), false } };
}

