#pragma once
#include "RTFileFormat.h"

#ifndef CLANLIB_1
	#include "Math/rtRect.h"
	#include "MathUtils.h"
	bool LoadFromFile(CL_Vec2f &num, FILE *fp);
	bool LoadFromFile(CL_Vec3f &num, FILE *fp);
	bool LoadFromFile(CL_Rectf &num, FILE *fp);
#endif

byte * LoadFileIntoMemory(std::string fileName, unsigned int *p_ui_size, bool bUseSavePath = false); //automatically decompresses if needed
byte * LoadFileIntoMemoryBasic(std::string fileName, unsigned int *length, bool bUseSavePath = false, bool bAddBasePath = true); //won't try to automatically decompress

bool FileExists(const std::string &fName);

std::string SeparateStringSTL(std::string input, int index, char delimiter);
bool SeparateString (const char str[], int num, char delimiter, char *return1);
void StringReplace(const std::string& what, const std::string& with, std::string& in);
bool CompressFile(std::string fName);
bool IsAPackedFile(byte *pFile);
bool IsARTFile(byte *pFile); //not full proof, but helps with catching errors
rtpack_header BuildRTPackHeader(int size, int compressedSize);

byte * zlibDeflateToMemory(byte *pInput, int sizeBytes, int *pSizeCompressedOut);
byte * zLibInflateToMemory(byte *pInput, unsigned int compressedSize, unsigned int decompressedSize);

//some helpers with file handling
bool LoadFromFile(std::string &str, FILE *fp);
bool LoadFromFile(float &num, FILE *fp);
bool LoadFromFile(int32 &num, FILE *fp);
bool LoadFromFile(uint32 &num, FILE *fp);
bool LoadFromFile(bool &num, FILE *fp);
bool SaveToFile(float num, FILE *fp);
bool SaveToFile(int num, FILE *fp);
bool SaveToFile(uint32 num, FILE *fp);
bool SaveToFile(const std::string &str, FILE *fp);

bool IsPowerOf2(int n);
byte * DecompressRTPackToMemory(byte *pMem, unsigned int *pDecompressedSize=NULL);
int GetFileSize(const std::string &fName);
std::string AddIPADToFileName(std::string file); //appends _ipad to a file name if we are indeed running on an ipad (or large screen)
std::string ReplaceWithDeviceNameInFileName(const std::string &fName); //replace "iphone" with "ipad" in filename, if on ipad
std::string ReplaceWithLargeInFileName(const std::string &fName); //replace "iphone" with "ipad" in filename, if on ipad
std::string ReplaceWithLargeInFileNameAndOSSpecific(const std::string &fName); //like above, but also changes to "win" if Windows is detected
std::string ReplaceMP3( const std::string &fName); //changes mp3 to ogg in a filename if not on iOS
void AppendStringToFile(const std::string filename, const std::string text);
std::string StripColorCodes(const std::string text);
bool StringFromStartMatches(const std::string &line, const std::string textToMatch); //like an strnstr for strings
