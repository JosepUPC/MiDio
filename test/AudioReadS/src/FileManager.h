#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <FS.h>
#include <SPIFFS.h>
#include <base64.h>

class FileManager {
public:
    FileManager();
    ~FileManager();

    bool writeFile(const char* path, const uint8_t* data, size_t length);
    String readFile(const char* path);
    String encodeBase64(const uint8_t* data, size_t length);

private:
    const char* TAG = "FileHandler";
};

#endif // FILEMANAGER_H