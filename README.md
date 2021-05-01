# xmlabit

> xmlabit = xml + alphabet + it!

**A tool sort xml node by pugixml, sort xml nodes/attributes by alphabet.**

![GitHub Action](https://github.com/wuruofan/xmlabit/actions/workflows/cmake.yml/badge.svg)

## Usage: 

  `xmlabit [options] -t <xml_nodes/attributes_name> -o <output_xml_file> <input_xml_file>`


## Options:

  `-v`, `--version`		Print version information.
  
  `-h`, `--help`		Print this usage.
  
  `-t`, `--target`		Xml nodes/attributes name which need to sort, like: /xpath/of/parent_node#node_name@node_or_attribute_name, for example: /bookstore/#books@price.
  
  `-o`, `--output`		Ouput xml file path which sorted by xmlabit, if none output file argument is provided, then output to the screen.
  
  `-d`, `--desecend`	Sort nodes/attributes value in deseconding order.
  
  `-i`, `--ignore-case`	Case insensitive while sorting not in numeric mode.

