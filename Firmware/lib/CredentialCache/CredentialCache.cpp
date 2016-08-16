#include "CredentialCache.h"
#include "FS.h"
void CredentialCache::begin() { SPIFFS.begin(); }

bool CredentialCache::isFormatted() { return SPIFFS.exists(FORMAT_MAGIC_FILE); }

void CredentialCache::format() {
  SPIFFS.format();
  File magic = SPIFFS.open(FORMAT_MAGIC_FILE, "w+");
  magic.print(FORMAT_MAGIC_FILE);
  magic.close();
}

String CredentialCache::getFileName(String ssid) {
  String filePath = String(CREDENTIAL_FOLDER);
  filePath += FS_FILE_SEPERATOR;
  filePath += ssid;
  filePath.replace(" ", "");
  return filePath;
}

bool CredentialCache::hasPassphrase(const String &ssid) {
  String filePath = getFileName(ssid);
  return SPIFFS.exists(filePath);
}

String CredentialCache::getPassphrase(const String &ssid) {
  String filePath = getFileName(ssid);
  File passFile = SPIFFS.open(filePath, "r");

  String passPhrase = passFile.readStringUntil(FILE_TERMINATOR);

  passFile.close();

  return passPhrase;
}

void CredentialCache::savePassphrase(const String &ssid,
                                     const String &passphrase) {
  String filePath = getFileName(ssid);

  File passFile = SPIFFS.open(filePath, "w+");

  passFile.println(passphrase);

  passFile.close();
}
