/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : rf.w
#   Email         : demonsimon#gmail.com
#   File Name     : xmlabit.cpp
#   Last Modified : 2021-05-21 18:14
#   Describe      :
#
# ====================================================*/

#include <cstdio>
#include <getopt.h>
#include <iterator>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "pugixml.hpp"
#include "xlogger.h"

using namespace std;
using namespace xlogger;

#ifdef DEBUG
#define LOG_LEVEL (Level::Debug)
#else
#define LOG_LEVEL (Level::Info)
#endif

const char *node_types[] = {"null",  "document", "element", "pcdata",
                            "cdata", "comment",  "pi",      "declaration"};

static const std::string kVersion = "0.2.4";
static const std::string kUsage =
    "Sort xml nodes/attributes by alphabet.\n"
    "\nUsage: xmlabit [options] -t <xml_nodes/attributes_name> -o "
    "<output_xml_file> <input_xml_file>\n"
    "\nOptions:\n"
    "  -v, --version\t\tPrint version information.\n"
    "  -h, --help\t\tPrint this usage.\n"
    "  -t, --target\t\tXml nodes/attributes name which need to sort, like: "
    "`/xpath/of/parent_node@node#attribute`, for example: "
    "/bookstore@books#price. Extraly, `#attribute` can be omitted.\n"
    "  -o, --output\t\tOuput xml file path which sorted by xmlabit, if none "
    "output file argument is provided, then output to the screen.\n"
    "  -n, --numeric\t\tTreat nodes/attributes value as number.\n"
    "  -d, --desecend\tSort nodes/attributes value in deseconding order.\n"
    "  -i, --ignore-case\tCase insensitive while sorting not in numeric "
    "mode.\n"
    "  -s, --sort-attr\t\tSort nodes' attributes by alphabet.\n";

const char *get_attribute_value(pugi::xml_node node,
                                const char *attribute_name) {
  if (node.attribute(attribute_name))
    return node.attribute(attribute_name).value();
  else if (node.child(attribute_name))
    return node.child(attribute_name).text().get();
  else {
    XLOG(D) << "get_value text: " << node.text().get() << " of " << node.name();
    return node.text().get();
  }
}

int comparator(const char *lhs, const char *rhs, bool ascending_order = true,
               bool is_numeric = false, bool ignore_case = false) {
  int ascending_flag = ascending_order ? 1 : -1;
  long double lhsd = 0.0, rhsd = 0.0;

  if (is_numeric) {
    try {
      lhsd = std::stold(std::string(lhs));
      rhsd = std::stold(std::string(rhs));
    } catch (const std::invalid_argument &e) {
      throw std::runtime_error("target value not numeric!");
    }

    long double ret = (lhsd - rhsd) * ascending_flag;

    // if lhs < rhs in numeric, return -1.
    if (ret > 0.0)
      return 1;
    else if (ret == 0.0)
      return 0;
    else
      return -1;

  } else {
    int ret = 0;

    // if lhs < rhs in ASCII, then strcmp return negative value.
    if (ignore_case) {
      std::string lhs_str(lhs);
      std::string rhs_str(rhs);

      transform(lhs_str.begin(), lhs_str.end(), lhs_str.begin(), ::tolower);
      transform(rhs_str.begin(), rhs_str.end(), rhs_str.begin(), ::tolower);

      ret = std::strcmp(lhs_str.c_str(), rhs_str.c_str()) * ascending_flag;
    } else {
      ret = std::strcmp(lhs, rhs) * ascending_flag;
    }

    XLOG(D) << "[" << __func__ << "] " << lhs
            << (ret < 0     ? " < "
                : (ret > 0) ? " > "
                            : "==")
            << rhs << ", ret = " << ret;

    return ret;
  }
}

inline const pugi::xml_node next_node(pugi::xml_node current_node,
                                      bool walking_previous) {
  return walking_previous ? current_node.previous_sibling()
                          : current_node.next_sibling();
}

// std::ostream& operator<<(std::ostream& os, const pugi::xml_node &node) {
// std::ostringstream oss(std::ios_base::ate);
// node.print(oss);
// // std::cout << "aaaaaaaaaaa: " << oss.str()<< std::endl;
// // os << oss.str();
// return os << oss.str();
// }

// inline std::string&& to_string(const pugi::xml_node &node) {
// std::ostringstream oss(std::ios_base::ate);
// node.print(oss);
// return std::move(oss.str());
// }

/* sort current xml_node's attributes by alphabet */
int sort_node_attributes(pugi::xml_node curr_node) {
  assert(curr_node);

  if (curr_node.attributes().empty()) {
    return 0;
  }

  // if (std::distance(curr_node.attributes_end(), curr_node.attributes_begin())
  // == 1) {
  //   return 0;
  // }
#ifdef DEBUG
  XLOG(D) << "\n>>> before attributes sort: ";
  curr_node.print(std::cout);
  XLOG(D) << "\n";
#endif

  pugi::xml_attribute mid_sorted_attr;

  int sorted_attr_count = 0;

  for (auto curr_attr = curr_node.first_attribute(); curr_attr;
       sorted_attr_count++) {
    if (mid_sorted_attr.empty()) {
      mid_sorted_attr = curr_attr;
      curr_attr = curr_attr.next_attribute();
      continue;
    }

    pugi::xml_attribute pos_attr, next_attr;

    pugi::xml_attribute tmp_attr = curr_attr; // use to remove

    next_attr = curr_attr.next_attribute();
    pos_attr = curr_attr;

    std::string attr_name = curr_attr.name();
    std::string attr_value = curr_attr.value();

    XLOG(D) << "curr_attr name: " << attr_name
            << ", mid_sorted_attr name: " << mid_sorted_attr.name();

    int result = comparator(mid_sorted_attr.name(), curr_attr.name());

    if (result > 0) {
      // mid_sorted_attr name > curr_attr, need find pos before it.
      XLOG(D) << "mid_sorted_attr's name > curr_attr's name: "
              << mid_sorted_attr.name() << ", " << curr_attr.name();
      pugi::xml_attribute last_pos_attr = mid_sorted_attr;
      pos_attr = mid_sorted_attr.previous_attribute();

      while (!pos_attr.empty() &&
             comparator(pos_attr.name(), curr_attr.name()) > 0) {
        last_pos_attr = pos_attr;
        pos_attr = pos_attr.previous_attribute();
      }

      curr_node.remove_attribute(tmp_attr);
      curr_attr = next_attr;

      if (pos_attr.empty()) {
        curr_node.insert_attribute_before(attr_name.c_str(), last_pos_attr) =
            attr_value.c_str();
      } else {
        curr_node.insert_attribute_after(attr_name.c_str(), pos_attr) =
            attr_value.c_str();
      }

    } else {
      // mid_sorted_attr name < curr_attr, need find pos after it.
      XLOG(D) << "mid_sorted_attr's name < curr_attr's name: "
              << mid_sorted_attr.name() << ", " << curr_attr.name();
      pos_attr = mid_sorted_attr.next_attribute();
      XLOG(D) << pos_attr;
      while (pos_attr != curr_attr &&
             comparator(pos_attr.name(), curr_attr.name()) < 0) {
        pos_attr = pos_attr.next_attribute();
      }

      if (pos_attr == curr_attr) {
        // do nothing
      } else {
        curr_node.remove_attribute(tmp_attr);
        curr_node.insert_attribute_before(attr_name.c_str(), pos_attr) =
            attr_value.c_str();
      }

      curr_attr = next_attr;
    }

    if (sorted_attr_count % 2 == 0) {
      if (result > 0) {
        // already insert one node in front mid_sorted_node, not need to move.
      } else {
        mid_sorted_attr = mid_sorted_attr.next_attribute();
      }
    }
  }

#ifdef DEBUG
  XLOG(D) << ">>> after attributes sort: ";
  curr_node.print(std::cout);
  XLOG(D) << "\n";
#endif

  return 0;
}

int sort_xml_node(pugi::xml_node parent_node, const char *child_name,
                  const char *attribute_name, bool ascending_order = true,
                  bool is_numeric = false, bool ignore_case = false,
                  bool sorting_attr = false) {
  assert(parent_node);
  // assert(parent_node.children(child_name));

  pugi::xml_node pos_node;
  pugi::xml_node tmp_curr_node;

  pugi::xml_node mid_sorted_node;
  int sorted_node_count = 0;

  for (pugi::xml_node curr_node = parent_node.child(child_name); curr_node;
       curr_node = curr_node.next_sibling(child_name), sorted_node_count++) {

    if (sorting_attr) {
      sort_node_attributes(curr_node);
    }

    tmp_curr_node = curr_node.previous_sibling(child_name);
    pos_node = curr_node;

#ifdef DEBUG
    XLOG(D) << "---------- curr_node ----------";
    curr_node.print(std::cout);
    XLOG(D) << "---------- mid_node ----------";
    mid_sorted_node.print(std::cout);
#endif

    if (mid_sorted_node.empty()) {
      XLOG(D) << "This is the first node !";
      mid_sorted_node = curr_node;
      continue;
    }

    int result =
        comparator(get_attribute_value(mid_sorted_node, attribute_name),
                   get_attribute_value(curr_node, attribute_name),
                   ascending_order, is_numeric, ignore_case);

    if (result > 0) {
      // mid_sorted_node value > curr_node, need find pos before it.
      pugi::xml_node last_pos_node = mid_sorted_node;
      pos_node = mid_sorted_node.previous_sibling(child_name);
      while (!pos_node.empty() &&
             comparator(get_attribute_value(pos_node, attribute_name),
                        get_attribute_value(curr_node, attribute_name),
                        ascending_order, is_numeric, ignore_case) > 0) {
        last_pos_node = pos_node;
        pos_node = pos_node.previous_sibling(child_name);
      }

#ifdef DEBUG
      XLOG(D) << "---------- pos_node ----------";
      pos_node.print(std::cout);
#endif

      if (pos_node.empty()) {
#ifdef DEBUG
        XLOG(D) << "---------- last_pos_node ----------";
        last_pos_node.print(std::cout);
#endif

        parent_node.insert_move_before(curr_node, last_pos_node);
        curr_node = tmp_curr_node;
      } else {
        parent_node.insert_move_after(curr_node, pos_node);
        curr_node = tmp_curr_node;
      }
    } else {
      // mid_sorted_node value < curr_node, need find pos after it.
      pos_node = mid_sorted_node.next_sibling(child_name);
      while (pos_node != curr_node &&
             comparator(get_attribute_value(pos_node, attribute_name),
                        get_attribute_value(curr_node, attribute_name),
                        ascending_order, is_numeric) < 0) {
        pos_node = pos_node.next_sibling(child_name);
      }

#ifdef DEBUG
      XLOG(D) << "---------- pos_node ----------";
      pos_node.print(std::cout);
#endif

      if (pos_node == curr_node) {
        // do nothing
      } else {
        parent_node.insert_move_after(curr_node, pos_node);
        curr_node = tmp_curr_node;
      }
    }

    if (sorted_node_count % 2 == 0) {
      if (result > 0) {
        // already insert one node in front mid_sorted_node, not need to move.
      } else
        mid_sorted_node = mid_sorted_node.next_sibling(child_name);
    }

#ifdef DEBUG
    XLOG(D) << "===============================";
    parent_node.print(std::cout);
#endif
  }

  return 0;
}

#if 0
pugi::xml_node find_insert_position_node(pugi::xml_node start_node,
                                         const char* attribute_name,
                                         const char* current_value,
                                         bool walking_previous,
                                         bool ascending_order = true) {
  // if walk previous, walk stop while find node comparator result negative.
  int stopping_comparartor_flag = walking_previous ? -1 : 1;
  int comparator_result = 0;

  pugi::xml_node pos_node = start_node;
  pugi::xml_node node = next_node(start_node, walking_previous);

  while (node) {
    comparator_result = comparator(
        current_value, get_value(node, attribute_name), ascending_order);
    if (stopping_comparartor_flag && comparator_result) {
      // find position
      break;
    } else {
      pos_node = node;
      node = next_node(node, walking_previous);
    }
  }

  return pos_node;
}

pugi::xml_node insert_node_relatively(pugi::xml_node& parent_node,
                                      const pugi::xml_node& current_node,
                                      pugi::xml_node& last_node,
                                      const char* attribute_name,
                                      bool ascending_order = true) {
  const char* current_value = nullptr;
  const char* last_value = nullptr;
  int comparator_result = 0;

  pugi::xml_node pos_node;
  pugi::xml_node ret_node;

  assert(parent_node);
  assert(current_node);

  XLOG(D) << "current_node: " << current_node.hash_value();
  XLOG(D) << "last_node: " << last_node.hash_value();
  XLOG(D) << "parent_node empty: " << parent_node.empty()
          << ", last_node empty: " << last_node.empty();
  if (last_node.empty()) {
    ret_node = parent_node.append_copy(current_node);

  } else {
    current_value = get_value(current_node, attribute_name);
    last_value = get_value(last_node, attribute_name);

    assert(current_value);
    assert(last_value);

    comparator_result = comparator(current_value, last_value, ascending_order);
    XLOG(D) << "current_value: " << current_value
            << ", last_value: " << last_value;
    if (comparator_result > 0) {
      pos_node = find_insert_position_node(
          last_node, attribute_name, current_value, true, ascending_order);
      // ret_node = pos_node.prepend_copy(current_node);
      ret_node = parent_node.insert_copy_before(current_node, pos_node);
      XLOG(D) << "insert before pos_node: " << pos_node.hash_value();
    } else {
      pos_node = find_insert_position_node(
          last_node, attribute_name, current_value, false, ascending_order);
      // ret_node = pos_node.append_copy(current_node);
      ret_node = parent_node.insert_copy_after(current_node, pos_node);
      XLOG(D) << "insert after  pos_node: " << pos_node.hash_value();
    }
  }
  XLOG(D) << "ret_node: " << ret_node.hash_value();

#ifdef DEBUG
  for (pugi::xml_node ch : parent_node.children()) {
    XLOG(D) << "parent ch: " << ch.hash_value();
  }
  parent_node.print(std::cout);
#endif
  return ret_node;
}
#endif

inline std::string get_raw_string(pugi::xml_node node) {
  std::stringstream ss;
#ifdef DEBUG
  node.print(ss, "  ", pugi::format_raw);
#endif
  return ss.str();
}

std::string output_file_path(std::string input_file_path) {
  std::stringstream ss;
  const char *file_name = std::strrchr(input_file_path.c_str(), '/');
  const std::string postfix("_sorted");

  if (!file_name)
    file_name = input_file_path.c_str();
  else
    file_name += 1;

  const char *file_extension = std::strchr(file_name, '.');

  if (!file_extension)
    ss << input_file_path << postfix;
  else {
    int path_length_without_extension =
        file_extension - input_file_path.c_str();
    ss << input_file_path.substr(0, path_length_without_extension) << postfix
       << input_file_path.substr(path_length_without_extension,
                                 input_file_path.length() -
                                     path_length_without_extension);
  }

  return ss.str();
}

pugi::xpath_node &&check_node(const pugi::xml_document &dx,
                              const string &parent_node_path,
                              const string &node_name,
                              const string &attr_name) {
  pugi::xpath_node parent_xnode;
  try {
    parent_xnode = dx.select_node(parent_node_path.c_str());

    pugi::xml_node node = parent_xnode.node().child(node_name.c_str());

    if (!node) {
      throw std::runtime_error("found target node failed: " + node_name + "!");
    } else if (!attr_name.empty()) {
      // XLOG(D) << "attribute: " << attr_name;
      // XLOG(D) << "> " << node.child(attr_name.c_str());
      // XLOG(D) << "> " << node.attribute(attr_name.c_str());
      if (!(node.child(attr_name.c_str()) ||
            node.attribute(attr_name.c_str()))) {
        throw std::runtime_error("found target attribute failed: " + attr_name +
                                 "!");
      }
    }
  } catch (const pugi::xpath_exception &e) {
    throw std::runtime_error(
        "select target parent node failed: " + parent_node_path + "!");
  }

  return std::move(parent_xnode);
}

int main(int argc, char **argv) {
  // xmlabit -t /root/plcy/safeapp@item#name -n -d -o out_path.xml file_path.xml
  int ret = 0;
  const char *optstring = "t:o:nidvhs";
  std::string node_query_string;
  std::string in_file_path;
  std::string out_file_path;
  std::string parent_node_path, target_node_name, target_attribute_name;
  bool using_numeric_comparator = false, comparator_ignore_case = false,
       acsecending_order = true;
  bool sorting_attributes = false;

  set_level_threshold(LOG_LEVEL);

  // std::vector<std::string> input_files;

  std::regex query_regex("(\\S+)@(\\S+)#(\\S+)");
  std::regex query_regex_no_attr("(\\S+)@(\\S+)");
  std::smatch matches;

  int choice;
  while (1) {
    static struct option long_options[] = {
        /* Use flags like so:
        {"verbose",	no_argument,	&verbose_flag, 'V'}*/
        /* Argument styles: no_argument, required_argument, optional_argument */
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"target", required_argument, 0, 't'},
        {"output", required_argument, 0, 'o'},
        {"numeric", no_argument, 0, 'n'},
        {"desecend", no_argument, 0, 'd'},
        {"ignore-case", no_argument, 0, 'i'},
        {"sort-attrs", no_argument, 0, 's'},
        {0, 0, 0, 0}};

    int option_index = 0;

    /* Argument parameters:
      no_argument: " "
      required_argument: ":"
      optional_argument: "::" */

    choice = getopt_long(argc, argv, optstring, long_options, &option_index);

    if (choice == -1)
      break;

    switch (choice) {
    case 'v':
      XLOG(I) << "xmlabit version " << kVersion;
      return 0;
      break;

    case 'h':
      XLOG(I) << kUsage;
      break;

    case '?':
      /* getopt_long will have already printed an error */
      XLOG(I) << "Unsupported arguments: " << argv[optind - 1];
      XLOG(I) << kUsage;
      return -EXIT_FAILURE;
      break;

    case 't':
      node_query_string = optarg;
      if (std::regex_match(node_query_string, matches, query_regex)) {
        XLOG(D) << node_query_string
                << " has attribute, matches: " << matches.size();

        assert(matches.size() == 4);

        parent_node_path.assign(matches[1]);
        target_node_name.assign(matches[2]);
        target_attribute_name.assign(matches[3]);

        XLOG(D) << "parent node path: " << parent_node_path;
        XLOG(D) << "target node name: " << target_node_name;
        XLOG(D) << "target attribute: " << target_attribute_name;
      } else if (std::regex_match(node_query_string, matches,
                                  query_regex_no_attr)) {
        XLOG(D) << node_query_string
                << " no attribute, matches: " << matches.size();
        assert(matches.size() == 3);

        parent_node_path.assign(matches[1]);
        target_node_name.assign(matches[2]);

        XLOG(D) << "parent node path: " << parent_node_path;
        XLOG(D) << "target node name: " << target_node_name;
      } else {
        XLOG(E) << "Illegal arguments: " << node_query_string;
      }

      break;
    case 'o':
      out_file_path = optarg;
      break;
    case 'n':
      using_numeric_comparator = true;
      break;
    case 'i':
      comparator_ignore_case = true;
      break;
    case 'd':
      acsecending_order = false;
      XLOG(D) << "choose descending order!";
      break;
    case 's':
      sorting_attributes = true;
      break;
    default:
      /* Not sure how to get here... */
      ret = EXIT_FAILURE;
      return ret;
    }
  }

  /* Deal with non-option arguments here */
  if (optind < argc) {
    if (optind < argc - 1) {
      XLOG(W) << "Error: more than one input file founded! Now only support "
                 "one input xml file!\n";
      XLOG(I) << kUsage;

      return -1;
    }

    in_file_path.assign(argv[optind]);
    XLOG(D) << "input file: " << in_file_path;
    //     while (optind < argc) {
    // input_files.push_back(argv[optind]);
    // cout << argv[optind] ;
    // optind++;
    // }
  }

  if (node_query_string.empty() || in_file_path.empty()) {
    XLOG(E) << "Error: arguments error!\n";
    XLOG(I) << kUsage;
  }

  pugi::xml_document dx;

  pugi::xml_parse_result result = dx.load_file(in_file_path.c_str());

  if (!result) {
    XLOG(E) << "XML file(" << in_file_path
            << ") parsed with error: " << result.description() << "!";

    return -result.status;
  }

  if (!dx.empty()) {
#ifdef DEBUG
    dx.save(std::cout);
#endif

    pugi::xpath_node target_parent_xnode;

    try {
      target_parent_xnode = check_node(dx, parent_node_path, target_node_name,
                                       target_attribute_name);
    } catch (const runtime_error &e) {
      XLOG(E) << "Error: " << e.what();
      return -EXIT_FAILURE;
    }

    std::clock_t c_start = std::clock();

    auto t_start = std::chrono::high_resolution_clock::now();

    try {
      sort_xml_node(target_parent_xnode.node(), target_node_name.c_str(),
                    target_attribute_name.c_str(), acsecending_order,
                    using_numeric_comparator, comparator_ignore_case,
                    sorting_attributes);

#ifdef DEBUG_SORT_ATTR
      for (pugi::xml_node curr_node =
               target_parent_xnode.node().child(target_node_name.c_str());
           curr_node;
           curr_node = curr_node.next_sibling(target_node_name.c_str())) {
        sort_node_attributes(curr_node);
      }
#endif

    } catch (const std::runtime_error &e) {
      XLOG(E) << "Error: " << e.what();

      return -EXIT_FAILURE;
    }

    std::clock_t c_end = std::clock();

    auto t_end = std::chrono::high_resolution_clock::now();

    XLOG(D) << std::fixed << std::setprecision(2)
            << "time cost: " << (c_end - c_start) * 1000.0 / CLOCKS_PER_SEC
            << " ms";
    XLOG(D)
        << std::fixed << std::setprecision(2) << "Wall clock time passed: "
        << std::chrono::duration<double, std::milli>(t_end - t_start).count()
        << " ms\n";

    if (out_file_path.empty()) {
      dx.save(std::cout);
    } else {
      bool ret = dx.save_file(out_file_path.c_str());

      if (!ret) {
        XLOG(E) << "Error: save failed! " << out_file_path;
      }
    }
  }

  return 0;
}
