#ifndef _AL_ARCH_H_
#define _AL_ARCH_H_

#include <map>
#include <string>

enum class alCompressionType
{
	// options:
	// 1 is the fastest compression and generally useful for short data.
	// 2 is slightly slower but it gives better compression ratio.
	fastlz,

	zlib,
};

struct alCompressDataInfo
{
	alCompressionType m_compression_type = alCompressionType::fastlz;

	void* m_uncompressedData = 0;
	uint32_t m_uncompressedDataSize = 0;

	void* m_compressedData = 0;
	uint32_t m_compressedDataSize = 0;

	int32_t m_compressionOption = 0;
};

struct alArchiveFileZipFileData
{
	void* m_data = 0;
	uint32_t m_dataSize = 0;

	char m_fileName[0xff];
};

struct alArchiveFileEntry
{
	uint32_t m_compressedSize = 0;
	uint32_t m_uncompressedSize = 0;
	
	uint32_t m_positionInZip = 0;
	uint32_t m_numberInZip = 0;

	//alStringA m_fileName;
	const char* m_fileName = 0;
};

class alArchiveFile
{
	friend class alArchive;
	
	uint32_t m_fileNum = 0;
	std::map<std::string, alArchiveFileEntry> m_entries;

	void* m_zipFile = 0;

	void _addEntry(uint32_t us, uint32_t cs, uint32_t pos, uint32_t num, const char* name);
public:
	alArchiveFile();
	~alArchiveFile();

	uint32_t GetFileNum() { return m_fileNum; }

	alFileBuffer* GetFile(alArchiveFileEntry*);
	alFileBuffer* GetFile(const char*, alArchiveFileEntry* out = 0);
};

class alArchive
{
public:

	// Memory will be allocated using alMemory
	// Provide m_uncompressedData and m_uncompressedDataSize
	// Compressed data will be stored in m_uncompressedData 
	//  and m_uncompressedDataSize will have size
	static bool CompressData(alCompressDataInfo*);

	// Memory will be allocated using alMemory
	// Provide m_uncompressedDataSize
	// Provide m_compressedData and m_compressedDataSize
	// Uncompressed data will be in m_uncompressedData
	static bool DecompressData(alCompressDataInfo*);
	
	static alArchiveFile* LoadZip(const char*);
	static void SaveZip(alArchiveFileZipFileData* entries, uint32_t entriesNum, const char* fn);
};

#endif
