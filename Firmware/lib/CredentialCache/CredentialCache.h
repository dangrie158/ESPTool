#ifndef _CREDENTIALCACHE_H_
#define _CREDENTIALCACHE_H_

#include "Arduino.h"

#define CREDENTIAL_FOLDER "/cc"
#define FORMAT_MAGIC_FILE "/cc/.magic"
#define FS_FILE_SEPERATOR "/"
#define FILE_TERMINATOR '\r'

class CredentialCache {
private:
  static String getFileName(String ssid);

public:
  static void begin();
  static bool isFormatted();
  static void format();
  static bool hasPassphrase(const String &ssid);
  static String getPassphrase(const String &ssid);
  static void savePassphrase(const String &ssid, const String &passphrase);
};

#endif //_CREDENTIALCACHE_H_
