// Copyright 2018 ASL author
// Licensed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ASLZIP_H
#define ASLZIP_H

#include "export.h"

#include <asl/String.h>
#include <asl/Map.h>
#include <asl/Date.h>
#include <asl/File.h>

struct mz_zip_archive_;

namespace asl
{
class ASLZIP_API ZipFile;

/**
Represents an entry in a Zip archive (a compressed file); and has an interface similar to class File.
*/
class ASLZIP_API ZipItem
{
	friend class ZipFile;
public:
	ZipItem() { _size = 0; _index = -1; }
	ZipItem(ZipFile* owner, const String& path, int index, int size, double t);
	/**
	Returns this file's name (full path in archive)
	*/
	const String& name() const { return _path; }
	/**
	Returns this file's size
	*/
	int size() const { return _size; }
	/**
	Returns the file's modification date/time
	*/
	const Date& lastModified() const { return _date; }
	/**
	Returns true if this file exists in the archive
	*/
	bool exists() const { return _index >= 0; }

	/**
	Returns true if this entry is an empty directory
	*/
	bool isDirectory() const;
	/**
	Returns this file's text content
	*/
	String text();
	/**
	Returns this file's binary content
	*/
	Array<byte> content();
	/**
	Extracts this file to the given directory
	*/
	bool extract(const String& destdir);

private:
	ZipFile* _owner;
	int _index;
	String _path;
	int _size;
	Date _date;
};

/**
Represents a Zip archive
*/
class ASLZIP_API ZipFile
{
	friend class ZipItem;
public:
	/**
	Constructs a ZipFile object for the given local zip file path; if it exists its directory is loaded; otherwise it can
	still be used for adding files to a new zip file.
	*/
	ZipFile(const String& path);
	~ZipFile();
	/**
	Returns the list of files in this archive
	*/
	const Array<ZipItem>& items() { return _items; }
	/**
	Returns a file entry by path name
	*/
	ZipItem& operator[](const String& name);
	/**
	Extract all files with directory structure to the given destination directory; it will be created if needed, but its parent
	must exist.
	*/
	bool unpack(const String& destdir);
	/**
	Add the contents of a directory recursively to this archive; optionally adding the directory name as root
	*/
	bool pack(const String& srcdir, bool addroot = false);

	/**
	Adds a file to the archive with the given text content
	*/
	bool add(const String& path, const String& text);
	/**
	Adds a file to the archive with the given binary content
	*/
	bool add(const String& path, const Array<byte>& data);
	/**
	Adds a file to the archive containing the given local file; if path ends with a '/', it is treated as a
	directory and the file's original name is used; otherwise, path is used as the internal zipped name
	*/
	bool add(const String& path, const File& file);

	/**
	Sets compression level (0: no compression, fastest .. 9: best, slowest)
	*/
	ZipFile& setLevel(int n);

	void endWrite();

private:
	bool initWrite();
	bool packdir(const String& dir, const String& srcdir);
	mz_zip_archive_* _zip;
	String _path;
	Array<ZipItem> _items;
	Dic<ZipItem> _namedItems;
	Array<byte> _buffer;
	ZipItem _empty;
	bool _writing;
	int _levelFlags;
	int _level;
};

}

#endif
