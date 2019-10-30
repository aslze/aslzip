// Copyright 2018 ASL author
// Licensed under the MIT License (http://opensource.org/licenses/MIT)

#include <asl/ZipFile.h>
#include <asl/Path.h>
#include <asl/Directory.h>
#include "miniz.h"

struct mz_zip_archive_ : public mz_zip_archive {};

namespace asl {

static bool error(const String& msg)
{
	printf("Zip: %s\n", *msg);
	return false;
}

static String sanitize(const String& path)
{
	String p = path;
	p.replaceme('\\', '/');
	int i = 0;
	while(p[i] == '/')
		i++;
	return p.substring(i);
}
	
ZipItem::ZipItem(ZipFile* owner, const String& path, int index, int size, double t)
	:_owner(owner), _index(index), _path(path), _size(size), _date(Date(t))
{
}

bool ZipItem::isDirectory() const
{
	return _path.endsWith('/');
}

String ZipItem::text()
{
	String s(_size, _size);
	if (_index < 0) {
		s = "";
		return s;
	}
	mz_zip_reader_extract_to_mem_no_alloc(_owner->_zip, _index, (char*)s, _size, 0, _owner->_buffer.ptr(), _owner->_buffer.length());
	s[_size] = '\0';
	return s;
}

Array<byte> ZipItem::content()
{
	Array<byte> data(_index < 0? 0 : _size);
	if (_index < 0)
		return data;
	mz_zip_reader_extract_to_mem_no_alloc(_owner->_zip, _index, data.ptr(), _size, 0, _owner->_buffer.ptr(), _owner->_buffer.length());
	return data;
}

bool ZipItem::extract(const String& dest)
{
	if (_index < 0)
		return false;
	File file(dest + "/" + _path.split("/").last(), File::WRITE);
	bool ok = mz_zip_reader_extract_to_cfile(_owner->_zip, _index, file.stdio(), 0) != 0;
	file.close();
	file.setLastModified(_date);
	return ok;
}

ZipFile::~ZipFile()
{
	if (_writing)
	{
		endWrite();
	}
	else
		mz_zip_reader_end(_zip);
	delete (mz_zip_archive*)_zip;
}

void ZipFile::endWrite()
{
	if (_writing)
	{
		if (!mz_zip_writer_finalize_archive(_zip))
		{
			error("Cannot finalize archive");
		}
		mz_zip_writer_end(_zip);
		_file.close();
	}
}

ZipFile::ZipFile(const String& path)
{
	_writing = false;
	_path = path;
	_level = MZ_DEFAULT_LEVEL;
	_levelFlags = _level;
	_zip = (mz_zip_archive_*) new mz_zip_archive;
	_buffer.resize(0x10000);
	mz_zip_zero_struct(_zip);
	_file.open(path, File::READ);
	bool status = mz_zip_reader_init_cfile(_zip, _file.stdio(), 0, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) != 0;
	if (!status)
	{
		return;
	}
	int num = (int)mz_zip_reader_get_num_files(_zip);
	for (int i = 0; i < num; i++)
	{
		mz_zip_archive_file_stat stat;
		if (!mz_zip_reader_file_stat(_zip, i, &stat))
		{
			printf("mz_zip_reader_file_stat() failed!\n"); // skip ??
			mz_zip_reader_end(_zip);
			return;
		}

		ZipItem item(this, stat.m_filename, i, (int)stat.m_uncomp_size, (double)stat.m_time);
		_items << item;
		_namedItems[item.name()] = item;
	}
}

ZipFile& ZipFile::setLevel(int n)
{
	_level = clamp(n, 0, 9);
	_levelFlags = _level;
	return *this;
}

ZipItem& ZipFile::operator[](const String& name)
{
	return _namedItems.has(name) ? _namedItems[name] : _empty;
}

bool ZipFile::unpack(const String& dest)
{
	if (!Directory(dest).exists())
	{
		if (!Directory::create(dest))
			return false;
	}
	foreach2(String& name, ZipItem& item, _namedItems)
	{
		Array<String> parts = name.split('/');
		String dir;
		if (parts.length() > 1)
		{
			for (int j = 0; j < parts.length() - 1; j++)
			{
				if (parts[j] == ".." || !parts[j])
					continue;
				dir += "/" + parts[j];
				Directory::create(dest + dir);
			}
		}
		if (!item.isDirectory())
			item.extract(dest + "/" + Path(name).directory().string());
	}
	return true;
}

bool ZipFile::packdir(const String& dir, const String& srcdir)
{
	Array<File> items = Directory(srcdir).items();
	foreach(File& item, items)
	{
		if (item.name() == '.' || item.name() == "..")
			continue;
		if (item.isDirectory()) {
			if (!packdir(dir + item.name() + '/', item.path()))
				return false;
		}
		else {
			if (!add(dir + item.name(), item))
				return false;
		}
	}
	return true;
}

bool ZipFile::pack(const String& src, bool addroot)
{
	return packdir(addroot? Path(src).name() + '/' : String(""), src);
}

bool ZipFile::add(const String& name, const String& text)
{
	if (!initWrite())
		return false;
	String path = sanitize(name);
	if (!mz_zip_writer_add_mem(_zip, name, *text, text.length(), _levelFlags))
	{
		return error("Cannot add file");
	}
	return true;
}

bool ZipFile::add(const String& name, const Array<byte>& data)
{
	if (!initWrite())
		return false;
	String path = sanitize(name);
	if (!mz_zip_writer_add_mem(_zip, path, data.ptr(), data.length(), _levelFlags))
	{
		return error("Cannot add file");
	}
	return true;
}

bool ZipFile::add(const String& name, const File& file)
{
	if (!initWrite())
		return false;
	String path = sanitize(name);
	path = (!path || path.endsWith('/')) ? path + file.name() : path;
	ULong size = file.size();
	time_t t = (time_t)file.lastModified().time();
	File file2(file.path(), File::READ);
	if (!mz_zip_writer_add_cfile(_zip, path, file2.stdio(), size, &t, "", 0, _levelFlags, 0, 0, 0, 0))
	{
		return error(String(0, "Cannot add file '%s' from '%s'", *path, *file.path()));
	}
	return true;
}

bool ZipFile::initWrite()
{
	if (_writing)
		return true;
	
	if (!_items)
	{
		_file.open(_path, File::WRITE);
		if (!mz_zip_writer_init_cfile(_zip, _file.stdio(), _levelFlags))
		{
			return error("Cannot create file");
		}
	}
	else
	{
		ULong offset = ((mz_zip_archive*)_zip)->m_central_directory_file_ofs;
		_file.close();
		_file.open(_path, File::RW);

		if (!mz_zip_writer_init_from_reader_v2(_zip, _path, _levelFlags))
		{
			mz_zip_reader_end(_zip);
			return error("Cannot create writer from reader");
		}
	}

	_writing = true;
	return true;
}

}
