# xmlabit

> xmlabit = xml + alphabet + it!

**A tool sort xml node by pugixml, sort xml nodes/attributes by alphabet.**

中文说明文档：[XML排序小工具——xmlabit](https://wuruofan.com/2021/05/20/xmlabit-a-xml-node-sort-tool/)。

![GitHub Action](https://github.com/wuruofan/xmlabit/actions/workflows/cmake.yml/badge.svg)


## Usage: 

  `xmlabit [options] -t <xml_nodes/attributes_name> -o <output_xml_file> <input_xml_file>`


## Options:

  `-v`, `--version`		Print version information.

  `-h`, `--help`		Print this usage.

  `-t`, `--target`		Xml nodes/attributes name which need to sort, like: `/xpath/of/parent_node@node#attribute`, for example: /bookstore@books#price. Extraly, `#attribute` can be omitted.

  `-o`, `--output`		Ouput xml file path which sorted by xmlabit, if none output file argument is provided, then output to the screen.

  `-d`, `--desecend`	Sort nodes/attributes value in deseconding order.

  `-i`, `--ignore-case`	Case insensitive while sorting not in numeric mode.

  `-n`, `--numeric`		Treat nodes/attributes value as number.

  `-s`, `--sort-attr`	Sort nodes' attributes by alphabet.

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

Here we have another `books2.xml`, which also contains books' information, but the structure of XML is different.

Input test xml file `books2.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <books>
    <book lang="eng" price="29.99">Harry Potter3</book>
    <book lang="us" price="10">harry potter2</book>
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
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="eng" price="5">readme</book>
		<book lang="us" price="10">harry potter2</book>
		<book lang="eng" price="92">Learning XML</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="fr" price="2">Harry Potter</book>
	</books>
</bookstore>
```

You can see, there is a book named <harry potter2> list before <Learning XML>, that because the ASCII value of lower case letter `h` is bigger than `L`.

#### 2.2 sort the `book`s descending by title with case insensitive

`./xmlabit -t /bookstore/books@book -di books2.xml`

Output xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="eng" price="5">readme</book>
		<book lang="eng" price="92">Learning XML</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="us" price="10">harry potter2</book>
		<book lang="fr" price="2">Harry Potter</book>
	</books>
</bookstore>
```

#### 2.3 sort all `book`s by attribute `lang`

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
		<book lang="us" price="10">harry potter2</book>
		<book lang="zh-CN" price="0.1">readme2</book>
	</books>
</bookstore>
```

#### 2.4: sort all `book`s by attribute `price`

`./xmlabit -t /bookstore/books@book#price books2.xml`

Output xml is:

```xml
<?xml version="1.0"?>
<bookstore>
	<books>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="us" price="10">harry potter2</book>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="5">readme</book>
		<book lang="eng" price="92">Learning XML</book>
	</books>
</bookstore>
```

Because the value of `price` actually is a string, the xml sorted by alphabet.

#### 2.5: sort all `book`s by attribute `price` as numeric

`./xmlabit -t /bookstore/books@book#price books2.xml -n`

Output xml is:

```xml
<bookstore>
	<books>
		<book lang="eng" price="0.01">readme3</book>
		<book lang="zh-CN" price="0.1">readme2</book>
		<book lang="fr" price="2">Harry Potter</book>
		<book lang="eng" price="5">readme</book>
		<book lang="us" price="10">harry potter2</book>
		<book lang="eng" price="29.99">Harry Potter3</book>
		<book lang="eng" price="92">Learning XML</book>
	</books>
</bookstore>
```

### Example 3: sort nodes, also sort nodes' attributes

Here we have another `books3.xml`, very like `books2.xml`, but each node contains more attributes which are disordered too.

Input test xml file `books3.xml`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<bookstore>
  <books>
    <book lang="eng" price="29.99" day="20" month="May" year="2019">Harry Potter3</book>
    <book month="May" day="20" year="2019" price="10" lang="us" >harry potter2</book>
    <book lang="fr" month="May" price="2" day="20" year="2019">Harry Potter</book>
    <book lang="eng" year="2019" month="May" day="20" price="5">readme</book>
    <book month="May" year="2019" day="20" lang="zh-CN" price="0.1">readme2</book>
  </books>
</bookstore>
```

Run:

`xmlabit -t /bookstore/books@book#lang books3.xml -s`

Output xml is:

```xml
<?xml version="1.0"?>
<bookstore>
  <books>
    <book day="20" lang="eng" month="May" price="29.99" year="2019">Harry Potter3</book>
    <book day="20" lang="eng" month="May" price="5" year="2019">readme</book>
    <book day="20" lang="fr" month="May" price="2" year="2019">Harry Potter</book>
    <book day="20" lang="us" month="May" price="10" year="2019">harry potter2</book>
    <book day="20" lang="zh-CN" month="May" price="0.1" year="2019">readme2</book>
  </books>
</bookstore>
```