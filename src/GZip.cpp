#include <asl/GZip.h>
#include <asl/Path.h>
#include <asl/File.h>
#include "miniz.h"

namespace asl {

static bool error(const String& msg)
{
	printf("Zip: %s\n", *msg);
	return false;
}

int mz_deflate(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len, int level)
{
	int status;
	mz_stream stream;
	memset(&stream, 0, sizeof(stream));

	if ((source_len | *pDest_len) > 0xFFFFFFFFU)
		return MZ_PARAM_ERROR;

	stream.next_in = pSource;
	stream.avail_in = (mz_uint32)source_len;
	stream.next_out = pDest;
	stream.avail_out = (mz_uint32)*pDest_len;

	status = mz_deflateInit2(&stream, level, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
	if (status != MZ_OK)
		return status;

	status = mz_deflate(&stream, MZ_FINISH);
	if (status != MZ_STREAM_END)
	{
		mz_deflateEnd(&stream);
		return (status == MZ_OK) ? MZ_BUF_ERROR : status;
	}

	*pDest_len = stream.total_out;
	return mz_deflateEnd(&stream);
}

int mz_inflate(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len)
{
	mz_stream stream;
	int status;
	memset(&stream, 0, sizeof(stream));

	if ((source_len | *pDest_len) > 0xFFFFFFFFU)
		return MZ_PARAM_ERROR;

	stream.next_in = pSource;
	stream.avail_in = (mz_uint32)source_len;
	stream.next_out = pDest;
	stream.avail_out = (mz_uint32)*pDest_len;

	status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
	if (status != MZ_OK)
		return status;

	status = mz_inflate(&stream, MZ_FINISH);
	if (status != MZ_STREAM_END)
	{
		mz_inflateEnd(&stream);
		return ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
	}
	*pDest_len = stream.total_out;

	return mz_inflateEnd(&stream);
}

void gzip(const String& path)
{
	auto data = File(path).content();
	Array<byte> cdata(data.length());
	mz_ulong size = data.length();
	double t1 = now();
	mz_deflate(cdata, &size, data, data.length(), -1);
	double t2 = now();
	cdata.resize(size);
	printf("%i -> %i %.1f %% (%f s)\n", data.length(), size, 100.0*size / data.length(), t2 - t1);

	File file(path + ".gz", File::WRITE);
	file << '\x1f' << '\x8b' << '\x08' << '\x08' << (unsigned)File(path).lastModified().time() << '\x02' << '\0';
	file << Path(path).name() << '\0';
	file << cdata;
	unsigned crc = mz_crc32(MZ_CRC32_INIT, data, data.length());
	file << crc << (unsigned)data.length();
}

void gunzip(const String& path)
{
	File file(path, File::READ);
	file.seek(4);
	Date date = (double)file.read<unsigned>();
	file.seek(2, File::HERE);
	String name;
	while (char c = file.read<char>())
		name << c;
	unsigned len = unsigned(file.size() - file.position() - 8);
	printf("%s %s %i\n", *name, *date.toString(), len);
	Array<byte> cdata(len);
	file.read(cdata.ptr(), len);
	mz_ulong size = cdata.length() * 8;
	Array<byte> data(size);
	double t1 = now();
	int ret = mz_inflate(data, &size, cdata, cdata.length());
	double t2 = now();
	printf("%i\n", ret);
	data.resize(size);
	printf("%i -> %i (%f s)\n", cdata.length(), size, t2 - t1);
	File(path.replace(".gz", "")).put(data);
}

}
