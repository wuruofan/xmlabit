# xmlabit

> xmlabit = xml + alphabet + it!

**A tool sort xml node by pugixml, sort xml nodes/attributes by alphabet.**

![GitHub Action](https://github.com/wuruofan/xmlabit/actions/workflows/cmake.yml/badge.svg)

## Usage: 

  `xmlabit [options] -t <xml_nodes/attributes_name> -o <output_xml_file> <input_xml_file>`



Example: 

## Options:

  `-v`, `--version`		Print version information.

  `-h`, `--help`		Print this usage.

  `-t`, `--target`		Xml nodes/attributes name which need to sort, like: `/xpath/of/parent_node@node#attribute`, for example: /bookstore@books#price. Extraly, `#attribute` can be omitted.

  `-o`, `--output`		Ouput xml file path which sorted by xmlabit, if none output file argument is provided, then output to the screen.

  `-d`, `--desecend`	Sort nodes/attributes value in deseconding order.

  `-i`, `--ignore-case`	Case insensitive while sorting not in numeric mode.



## Complie:

At project directory, run command:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

If you want to compile debug version, please replace first command with `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.

`make install` will put `xmlabit` at `/usr/local/bin` by default.



## Examples:

### Example 1

You can run command below at `build` directory:

`./xmlabit -t /bookstore@book#title books.xml -o output.xml`

Input test xml file `books.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <magzine>
    <title lang="eng">Beauty</title>
    <price>129.29</price>
  </magzine>
  <book>
    <title lang="eng">Harry Potter3</title>
    <price>29.29</price>
  </book>
  <book>
    <title lang="eng">Harry Potter2</title>
    <price>39.99</price>
  </book>
  <book>
    <title lang="eng">Harry Potter</title>
    <price>29.19</price>
  </book>
  <book>
    <title lang="eng">readme</title>
    <price>0.95</price>
  </book>
  <book>
    <title lang="eng">readme2</title>
    <price>-0.955</price>
  </book>
  <book>
    <title lang="eng">Learning XML</title>
    <price>39.95</price>
  </book>
  <book>
    <title lang="eng">readme3</title>
    <price>0.95</price>
  </book>
</bookstore>
```

The output file `output.xml` is:

```xml
<?xml version="1.0"?>
<bookstore>
	<magzine>
		<title lang="eng">Beauty</title>
		<price>129.29</price>
	</magzine>
	<book>
		<title lang="eng">Harry Potter</title>
		<price>29.19</price>
	</book>
	<book>
		<title lang="eng">Harry Potter2</title>
		<price>39.99</price>
	</book>
	<book>
		<title lang="eng">Harry Potter3</title>
		<price>29.29</price>
	</book>
	<book>
		<title lang="eng">Learning XML</title>
		<price>39.95</price>
	</book>
	<book>
		<title lang="eng">readme</title>
		<price>0.95</price>
	</book>
	<book>
		<title lang="eng">readme2</title>
		<price>-0.955</price>
	</book>
	<book>
		<title lang="eng">readme3</title>
		<price>0.95</price>
	</book>
</bookstore>
```

### Example 2

Also you can run:

`./xmlabit -t /bookstore/books@title#lang -d books2.xml`

Input test xml file `books2.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <books>
    <title lang="eng" price="29.99">Harry Potter3</title>
    <title lang="us" price="10">Harry Potter2</title>
    <title lang="fr" price="2">Harry Potter</title>
    <title lang="eng" price="5">readme</title>
    <title lang="zh-CN" price="0.1">readme2</title>
    <title lang="eng" price="92">Learning XML</title>
    <title lang="eng" price="0.01">readme3</title>
  </books>
</bookstore>
```

Output test xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<title lang="eng" price="29.99">Harry Potter3</title>
		<title lang="eng" price="5">readme</title>
		<title lang="eng" price="92">Learning XML</title>
		<title lang="eng" price="0.01">readme3</title>
		<title lang="fr" price="2">Harry Potter</title>
		<title lang="us" price="10">Harry Potter2</title>
		<title lang="zh-CN" price="0.1">readme2</title>
	</books>
</bookstore>
```

### Example 3

Also you can run:

`./xmlabit -t /bookstore/books@title -d books2.xml`

Input test xml file `books2.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <books>
    <title lang="eng" price="29.99">Harry Potter3</title>
    <title lang="us" price="10">Harry Potter2</title>
    <title lang="fr" price="2">Harry Potter</title>
    <title lang="eng" price="5">readme</title>
    <title lang="zh-CN" price="0.1">readme2</title>
    <title lang="eng" price="92">Learning XML</title>
    <title lang="eng" price="0.01">readme3</title>
  </books>
</bookstore>
```

Output test xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<title lang="fr" price="2">Harry Potter</title>
		<title lang="us" price="10">Harry Potter2</title>
		<title lang="eng" price="29.99">Harry Potter3</title>
		<title lang="eng" price="92">Learning XML</title>
		<title lang="eng" price="5">readme</title>
		<title lang="zh-CN" price="0.1">readme2</title>
		<title lang="eng" price="0.01">readme3</title>
	</books>
</bookstore>
```

