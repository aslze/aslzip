#include <asl/ZipFile.h>

using namespace asl;

int main()
{
	ZipFile zip("gearth.kmz");

	foreach(ZipItem& item, zip.items())
	{
		printf("%s %i %s\n", *item.name(), item.size(), *item.lastModified().toString());
	}

	String txt = zip["doc.kml"].text();

	printf("\n%s\n", *txt);

	zip["doc.kml"].extract("./");

	zip.unpack("./kmz");

	//File("dummy.zip").remove();

	{
		ZipFile zip2("dummy.zip");

		zip2.add("datáλ.txt", "New Hello world!");
		zip2.add("datáλ0.txt", "New Hello world 2!");
		zip2.setLevel(0);
		zip2.add("images/", File("card.jpg"));

		zip2.setLevel(5);

		zip2.add("obj.dae", File("model.dae"));

	}
	ZipFile("dummy.zip").unpack("./dummy");

	File("kmz.kmz").remove();
	ZipFile("kmz.kmz").pack("kmz", true);
}

