/***************************************************************************
                          btinstallmgr.cpp  -  description
                             -------------------
    begin                : Mit Sep 24 2003
    copyright            : (C) 2003 by The BibleTime team
    email                : info@bibletime.info
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "btinstallmgr.h"

#include <kapplication.h>

#include <filemgr.h>
#include <swconfig.h>
#include <swbuf.h>
#include <map>
#include <utility>

using namespace sword;


QStringList BTInstallMgr::Tool::sourceList( sword::InstallMgr* mgr ) {
  qWarning("BTInstallMgr::Tool::sourceList( sword::InstallMgr* mgr )");
  Q_ASSERT(mgr);
  Q_ASSERT(mgr->installConf);
  
  QStringList names;
	for (InstallSourceMap::iterator it = mgr->sources.begin(); it != mgr->sources.end(); it++) {    
    names << QString::fromLocal8Bit(it->second->caption);
  }  
  return names;
}

sword::InstallSource* BTInstallMgr::Tool::source( sword::InstallMgr* mgr, const QString& name ) {
  Q_ASSERT(mgr);

  InstallSourceMap::iterator source = mgr->sources.find(name.latin1());
	if (source != mgr->sources.end()) {
  	return source->second;
  }
  return 0;   
}

void BTInstallMgr::Tool::addSource( sword::InstallSource* is ) {
 	Q_ASSERT(is);

  SWConfig config(Tool::configFilename().latin1());
	config["Sources"].insert( std::make_pair("FTPSource", is->getConfEnt()) );
  config.Save();
}

void BTInstallMgr::Tool::initConfig() {
	InstallSource is("FTP");
	is.caption = "Crosswire";
	is.source = "ftp.crosswire.org";
	is.directory = "/pub/sword/raw";
  Tool::addSource(&is);

	is.caption = "Crosswire Beta";
	is.source = "ftp.crosswire.org";
	is.directory = "/pub/sword/betaraw";
  Tool::addSource(&is);  

  SWConfig config(Tool::configFilename().latin1());
  config["General"]["PassiveFTP"] = "true";
  config.Save();
}

const QString BTInstallMgr::Tool::configPath() {
	const char *envhomedir  = getenv("HOME");
	QString confPath = QString::fromLatin1((envhomedir) ? envhomedir : ".");
	confPath += QString::fromLatin1("/.sword/InstallMgr");
//	qWarning(confPath.latin1());

  return confPath;
}

const QString BTInstallMgr::Tool::configFilename() {
  return (configPath() + "/InstallMgr.conf").latin1();
}

void BTInstallMgr::Tool::removeSource( sword::InstallMgr* mgr, sword::InstallSource* is) {
  Q_ASSERT(mgr);
  Q_ASSERT(is);

}

BTInstallMgr::BTInstallMgr() : InstallMgr(Tool::configPath().latin1()) {
}

BTInstallMgr::~BTInstallMgr(){
}

void BTInstallMgr::statusUpdate(double dltotal, double dlnow) {
  int totalPercent = (int)((float)(dlnow + m_completedBytes+1) / (float)(m_totalBytes) * 100);
  const int filePercent  = (int)((float)(dlnow + 1) / (float)(dltotal) * 100);
  int totalBytes;
  int fileBytes;
  
  
  emit completed(totalPercent, filePercent);
  KApplication::kApplication()->processEvents();
}

void BTInstallMgr::preDownloadStatus(long totalBytes, long completedBytes, const char *message) {
  emit downloadStarted( "unkown filename" );
  m_completedBytes = completedBytes;
  m_totalBytes = totalBytes;
  KApplication::kApplication()->processEvents();
}
