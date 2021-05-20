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

  `-n`, `--numeric`	Treat nodes/attributes value as number.



## Complie:

At project directory, run command:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

If you want to compile debug version, please replace `cmake ..` with `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.

`make install` will put `xmlabit` at `/usr/local/bin` by default.



## Examples:

You can run commands below at `build` directory:

### Example 1: sort node by sub node value

`./xmlabit -t /bookstore@book#title books.xml -o output.xml`

Here is a test xml file `books.xml`, contains bookstore and book informations:

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
If we want to sort books by book's title, the argument bebind `-t` is `/bookstore@book#title`, `/` means the root of XML, `/bookstore@book` means the name of the nodes to be sorted is `book`, the XPath is `/bookstore/book`, the value of sub node or attribute which named `title` will be used to sort.

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

Here we have another `book2.xml`, which also contains books' information, but the structure of XML is different.

Input test xml file `books2.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <books>
    <book lang="eng" price="29.99">Harry Potter3</book>
    <book lang="us" price="10">Harry Potter2</book>
    <book lang="fr" price="2">Harry Potter</book>
    <book lang="eng" price="5">readme</book>
    <book lang="zh-CN" price="0.1">readme2</book>
    <book lang="eng" price="92">Learning XML</book>
    <book lang="eng" price="0.01">readme3</book>
  </books>
</bookstore>
```


#### 2.1 sort the `book`s descending by title

`./xmlabit -t /bookstore/books@book -d books2.xml`

Output xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="us" price="10">Harry Potter2</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="92">Learning XML</book>
		<book lang="eng" price="5">readme</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="eng" price="0.01">readme3</book>
	</books>
</bookstore>
```

#### 2.2 sort all `book`s by attribute `lang`

`./xmlabit -t /bookstore/books@book#lang books2.xml`

Output test xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="5">readme</book>
		<book lang="eng" price="92">Learning XML</book>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="us" price="10">Harry Potter2</book>
		<book lang="zh-CN" price="0.1">readme2</book>
	</books>
</bookstore>
```

#### Example 2.3: sort all `book`s by attribute `price`

`./xmlabit -t /bookstore/books@book#price books2.xml`

Output xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="us" price="10">Harry Potter2</book>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="5">readme</book>
		<book lang="eng" price="92">Learning XML</book>
	</books>
</bookstore>
```

Because the value of `price` actually is a string, the xml sorted by alphabet.

#### Example 2.4: sort all `book`s by attribute `price` as numeric

`./xmlabit -t /bookstore/books@book#price books2.xml -n`

Output xml is:

```xml
<bookstore>
	<books>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="eng" price="5">readme</book>
		<book lang="us" price="10">Harry Potter2</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="92">Learning XML</book>
	</books>
</bookstore>
```


