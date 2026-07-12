#include "al.h"
#include "Archive/alArchive.h"

#include "fastlz.h"
#include "zlib.h"
#include "unzip.h"
#include "zip.h"

bool alArchive_CompressData_zlib(alCompressDataInfo* inf)
{
	uLong bufferSize = compressBound((uLong)inf->m_uncompressedDataSize);
	void* buffer = alMemory::Malloc(bufferSize);

	uLong compressedSize = 0;
	int ret = compress(
		(Bytef*)buffer,
		&compressedSize,
		(const Bytef*)inf->m_uncompressedData,
		inf->m_uncompressedDataSize
		);

	if (ret != Z_OK)
	{
		alMemory::Free(buffer);
		return false;
	}

	inf->m_compressedDataSize = compressedSize;
	inf->m_compressedData = alMemory::Realloc(buffer, inf->m_compressedDataSize);
	return true;
}

bool alArchive_DecompressData_zlib(alCompressDataInfo* inf)
{
	inf->m_uncompressedData = alMemory::Malloc(inf->m_uncompressedDataSize);

	uLong uncompressedSize = 0;
	int ret = uncompress(
		(Bytef*)inf->m_uncompressedData,
		&uncompressedSize,
		(Bytef*)inf->m_compressedData,
		inf->m_compressedDataSize
	);

	if (ret != Z_OK)
	{
		alMemory::Free(inf->m_uncompressedData);
		inf->m_uncompressedData = 0;
		return false;
	}
	return true;
}

bool alArchive_CompressData_fastlz(alCompressDataInfo* inf)
{
	int level = 1;
	if(inf->m_compressionOption == 2)
		level = 2;

	uint32_t bufferSize = inf->m_uncompressedDataSize + 
		(((inf->m_uncompressedDataSize + 100)/100) * 5);
	void* buffer = alMemory::Malloc(bufferSize);

	int ret = fastlz_compress_level(
		level, 
		inf->m_uncompressedData, 
		inf->m_uncompressedDataSize, 
		buffer);

	if ((uint32_t)ret > inf->m_uncompressedDataSize)
	{
		alMemory::Free(buffer);
		return false;
	}

	inf->m_compressedDataSize = ret;
	inf->m_compressedData = alMemory::Realloc(buffer, inf->m_compressedDataSize);

	return true;
}

bool alArchive_DecompressData_fastlz(alCompressDataInfo* inf)
{
	inf->m_uncompressedData = alMemory::Malloc(inf->m_uncompressedDataSize);

	int ret = fastlz_decompress(
		inf->m_compressedData,
		inf->m_compressedDataSize,
		inf->m_uncompressedData,
		inf->m_uncompressedDataSize
		);

	if (!ret)
	{
		alMemory::Free(inf->m_uncompressedData);
		inf->m_uncompressedData = 0;
		return false;
	}

	return true;
}

bool alArchive::CompressData(alCompressDataInfo* inf)
{
	AL_ASSERT_ST(inf);
	AL_ASSERT_ST(!inf->m_compressedData);
	AL_ASSERT_ST(inf->m_uncompressedData);
	AL_ASSERT_ST(inf->m_uncompressedDataSize);
	if (inf)
	{
		switch (inf->m_compression_type)
		{
		default:
		case alCompressionType::fastlz:
			return alArchive_CompressData_fastlz(inf);
		case alCompressionType::zlib:
			return alArchive_CompressData_zlib(inf);
		}
	}
	return false;
}

bool alArchive::DecompressData(alCompressDataInfo* inf)
{
	AL_ASSERT_ST(inf);
	AL_ASSERT_ST(inf->m_compressedData);
	AL_ASSERT_ST(inf->m_compressedDataSize);
	AL_ASSERT_ST(inf->m_uncompressedDataSize);

	if (inf)
	{
		switch (inf->m_compression_type)
		{
		default:
		case alCompressionType::fastlz:
			return alArchive_DecompressData_fastlz(inf);
		case alCompressionType::zlib:
			return alArchive_DecompressData_zlib(inf);
		}
	}
	return false;
}

alArchiveFile* alArchive::LoadZip(const char* fn)
{
	alArchiveFile* newArchFile = 0;

	char filename_inzip[0xffff];

	unzFile zipfile = unzOpen(fn);
	if (zipfile)
	{
		unz_global_info gi;
		if (unzGetGlobalInfo(zipfile, &gi) == UNZ_OK)
		{
			if (unzGoToFirstFile(zipfile) == UNZ_OK)
			{
				unz_file_info ufi;
				for (uLong i = 0; i < gi.number_entry; ++i)
				{
					if (unzGetCurrentFileInfo(
						zipfile,
						&ufi,
						filename_inzip,
						0xfff,
						0, 0, 0, 0) == UNZ_OK)
					{
						if (ufi.uncompressed_size)
						{
							if (!newArchFile)
							{
								newArchFile = alCreate<alArchiveFile>();
								newArchFile->m_zipFile = zipfile;
							}


							unz_file_pos filePos;
							unzGetFilePos(zipfile, &filePos);
							
							newArchFile->_addEntry(
								ufi.uncompressed_size,
								ufi.compressed_size,
								filePos.pos_in_zip_directory,
								filePos.num_of_file,
								filename_inzip);

						//	printf("%s\n", filename_inzip);

						}
						unzGoToNextFile(zipfile);
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	return newArchFile;
}

void alArchive::SaveZip(
	alArchiveFileZipFileData* entries,
	uint32_t entriesNum,
	const char* fn)
{
	AL_ASSERT_ST(entries);
	AL_ASSERT_ST(entriesNum);
	AL_ASSERT_ST(fn);

	zipFile zip = zipOpen64(fn, APPEND_STATUS_CREATE);
	if (zip != NULL)
	{
		for (uint32_t i = 0; i < entriesNum; ++i)
		{
			zip_fileinfo zi;
			zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
				zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
			zi.dosDate = 0;
			zi.internal_fa = 0;
			zi.external_fa = 0;
		//	filetime(filenameinzip, &zi.tmz_date, &zi.dosDate);

			zipOpenNewFileInZip(
				zip,
				entries[i].m_fileName,
				&zi,
				0,
				0,
				0,
				0,
				0,
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION);

			
			zipWriteInFileInZip(zip, entries[i].m_data, entries[i].m_dataSize);
			zipCloseFileInZip(zip);
		}
		zipClose(zip,0);
	}
}


// =============================================
//                                 alArchiveFile
// =============================================
void alArchiveFile::_addEntry(
	uint32_t us, 
	uint32_t cs, 
	uint32_t pos, 
	uint32_t num,
	const char* name)
{
	alArchiveFileEntry e;
	e.m_compressedSize = cs;
	e.m_uncompressedSize = us;
	e.m_positionInZip = pos;
	e.m_numberInZip = num;
	
	alStringA stra = name;
	stra.ToLower();
	stra.FlipSlash();

	m_entries[stra.c_str()] = e;
	auto it = m_entries.find(stra.c_str());
	it->second.m_fileName = it->first.c_str();

	++m_fileNum;
}

alArchiveFile::alArchiveFile()
{
}

alArchiveFile::~alArchiveFile()
{
	if (m_zipFile)
	{
		unzClose(m_zipFile);
	}
}

alFileBuffer* alArchiveFile::GetFile(alArchiveFileEntry* fe)
{
	AL_ASSERT_ST(fe);
	alFileBuffer* fb = 0;
	if (fe)
	{
		unz64_file_pos_s _pos;
		_pos.pos_in_zip_directory = fe->m_positionInZip;
		_pos.num_of_file = fe->m_numberInZip;
		if (unzGoToFilePos64(m_zipFile, &_pos) == UNZ_OK)
		{
			unzOpenCurrentFile(m_zipFile);

			
			fb = alCreate<alFileBuffer>(fe->m_uncompressedSize);
			unzReadCurrentFile(m_zipFile, fb->Data(), fb->Size());
		}
	}
	return fb;
}

alFileBuffer* alArchiveFile::GetFile(const char* fn, alArchiveFileEntry* out)
{
	AL_ASSERT_ST(fn);

	alFileBuffer* fb = 0;
	if (fn)
	{
		alStringA stra(fn);
		stra.ToLower();

		auto it = m_entries.find(stra.c_str());
		if (it != m_entries.end())
		{
			if (out)
				*out = it->second;

			fb = GetFile(&it->second);
		}
	}
	return fb;
}

