# aslzip

A Zip file compress/uncompress extension library for ASL. It is based on the *miniz* library.
It might be merged into the ASL library in the future.


You can read a specific contained file as a string or as a binary blob. Or you can
extract it to a given directory (`asl` namespace omitted here):

```cpp
ZipFile zip("archive.zip");

String text = zip["some/file.txt"].text();

zip["some/data.csv"].extract("./");  // will produce "./data.csv"
```

Adding files to a new zip archive can also use files, strings or byte arrays as input:

```cpp
ZipFile zip("archive.zip");

zip.add("package.json", Json::encode(info));

zip.add("images/", File("some/logo.png"));  // the zip entry will be "images/logo.png"
```

And you can pack or unpack directories recursively with all their content:

```cpp
ZipFile("archive.zip").unpack("data/desdir");

ZipFile("archive2.zip").pack("some/dir", true);
```

The second case will pack the contents of directory "some/dir" into "archive2.zip". The
optional `true` argument makes the directory name "dir" be placed at the root of the zip file.

You can also enumerate the contents of a zip archive:

```cpp
for(auto& item : zip.items())
{
	printf("%s %i %s\n", *item.name(), item.size(), *item.lastModified().toString());
	
	// item.text() extracts the text content
	// item.content() extracts the binary content
}
```
