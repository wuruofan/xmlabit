/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : rf.w
#   Email         : demonsimon#gmail.com
#   File Name     : xmlabit.cpp
#   Last Modified : 2021-05-10 20:38
#   Describe      :
#
# ====================================================*/

#include <getopt.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "pugixml.hpp"
#include "xlogger.h"

#define ENABLE_NUMERIC_COMPAROR 0

using namespace std;
using namespace xlogger;

#ifdef DEBUG
#define LOG_LEVEL (Level::Debug)
#else
#define LOG_LEVEL (Level::Info)
#endif

const char* node_types[] = {"null",  "document", "element", "pcdata",
                            "cdata", "comment",  "pi",      "declaration"};

static const std::string kVersion = "0.0.1";
static const std::string kUsage =
    "Sort xml nodes/attributes by alphabet.\n"
    "\nUsage: xmlabit [options] -t <xml_nodes/attributes_name> -o "
    "<output_xml_file> <input_xml_file>\n"
    "\nOptions:\n"
    "  -v, --version\t\tPrint version information.\n"
    "  -h, --help\t\tPrint this usage.\n"
    "  -t, --target\t\tXml nodes/attributes name which need to sort, like: "
    "/xpath/of/parent_node#node_name@node_or_attribute_name, for example: "
    "/bookstore/#books@price.\n"
    "  -o, --output\t\tOuput xml file path which sorted by xmlabit, if none "
    "output file argument is provided, then output to the screen.\n"
#if ENABLE_NUMERIC_COMPAROR
    "  -n, --numeric\t\tTreat nodes/attributes value as number.\n"
#endif
    "  -d, --desecend\tSort nodes/attributes value in deseconding order.\n"
    "  -i, --ignore-case\tCase insensitive while sorting not in numeric "
    "mode.\n";

const char* get_value(pugi::xml_node node, const char* attribute_name) {
  if (node.attribute(attribute_name))
    return node.attribute(attribute_name).value();
  else if (node.child(attribute_name))
    return node.child(attribute_name).text().get();
  else
    return nullptr;
}

int comparator(const char* lhs, const char* rhs, bool ascending_order = true,
               bool is_numeric = false) {
  int ascending_flag = ascending_order ? 1 : -1;

  if (is_numeric) {
    long double lhsd = std::stold(std::string(lhs));
    long double rhsd = std::stold(std::string(rhs));
    long double ret = (lhsd - rhsd) * ascending_flag;

    // if lhs < rhs in numeric, return -1.
    if (ret > 0.0)
      return 1;
    else if (ret == 0.0)
      return 0;
    else
      return -1;

  } else {
    // XLOG(D) << "[" << __func__ << "] " << lhs << " & " << rhs ;

    // if lhs < rhs in ASCII, then strcmp return negative value.
    return std::strcmp(lhs, rhs) * ascending_flag;
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

int sort_xml_node(pugi::xml_node parent_node, const char* child_name,
                  const char* attribute_name, bool ascending_order = true,
                  bool is_numeric = false) {
  assert(parent_node);
  // assert(parent_node.children(child_name));

  pugi::xml_node prev_node;
  pugi::xml_node pos_node;
  pugi::xml_node tmp_curr_node;

  pugi::xml_node mid_sorted_node;
  int sorted_node_count = 0;

  for (pugi::xml_node curr_node = parent_node.child(child_name); curr_node;
       curr_node = curr_node.next_sibling(child_name), sorted_node_count++) {
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

    int result = comparator(get_value(mid_sorted_node, attribute_name),
                            get_value(curr_node, attribute_name),
                            ascending_order, is_numeric);

    if (result > 0) {
      // mid_sorted_node value > curr_node, need find pos before it.
      pugi::xml_node last_pos_node = mid_sorted_node;
      pos_node = mid_sorted_node.previous_sibling(child_name);
      while (!pos_node.empty() &&
             comparator(get_value(pos_node, attribute_name),
                        get_value(curr_node, attribute_name), ascending_order,
                        is_numeric) > 0) {
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
             comparator(get_value(pos_node, attribute_name),
                        get_value(curr_node, attribute_name), ascending_order,
                        is_numeric) < 0) {
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
  cout << "parent_node empty: " << parent_node.empty()
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

std::string get_raw_string(pugi::xml_node node) {
  std::stringstream ss;
#ifdef DEBUG
  node.print(ss, "  ", pugi::format_raw);
#endif
  return ss.str();
}

std::string output_file_path(std::string input_file_path) {
  std::stringstream ss;
  const char* file_name = std::strrchr(input_file_path.c_str(), '/');
  const std::string postfix("_sorted");

  if (!file_name)
    file_name = input_file_path.c_str();
  else
    file_name += 1;

  const char* file_extension = std::strchr(file_name, '.');

  if (!file_extension)
    ss << input_file_path << postfix;
  else {
    int path_length_without_extension =
        file_extension - input_file_path.c_str();
    ss << input_file_path.substr(0, path_length_without_extension) << postfix
       << input_file_path.substr(
              path_length_without_extension,
              input_file_path.length() - path_length_without_extension);
  }

  return ss.str();
}

int main(int argc, char** argv) {
  // xmlabit -t /root/plcy/safeapp#item@name -n -d -o out_path.xml file_path.xml
  int ret = 0;
#if ENABLE_NUMERIC_COMPAROR
  const char* optstring = "t:o:nidvh";
#else
  const char* optstring = "t:o:idvh";  // t:o:nidvh
#endif
  std::string node_query_string;
  std::string in_file_path;
  std::string out_file_path;
  std::string parent_node_path, target_node_name, target_attribute_name;
  bool using_numeric_comparator = false, comparator_ignore_case = false,
       acsecending_order = true;

  set_level_threshold(LOG_LEVEL);

  // std::vector<std::string> input_files;

  std::regex query_regex("\\S+#\\S+@\\S+");
  // std::smatch query_match;
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
#if ENABLE_NUMERIC_COMPAROR
      {"numeric", no_argument, 0, 'n'},
#endif
      {"desecend", no_argument, 0, 'd'},
      {"ignore-case", no_argument, 0, 'i'},
      {0, 0, 0, 0}
    };

    int option_index = 0;

    /* Argument parameters:
      no_argument: " "
      required_argument: ":"
      optional_argument: "::" */

    choice = getopt_long(argc, argv, optstring, long_options, &option_index);

    if (choice == -1) break;

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
        break;

      case 't':
        node_query_string = optarg;
        if (std::regex_match(node_query_string, query_regex)) {
          XLOG(D) << node_query_string;
          const char* c = std::strchr(node_query_string.c_str(), '#');
          const char* a = std::strchr(c, '@');
          parent_node_path =
              node_query_string.substr(0, c - node_query_string.c_str());
          target_node_name = node_query_string.substr(
              c + 1 - node_query_string.c_str(), a - c - 1);
          target_attribute_name =
              node_query_string.substr(a + 1 - node_query_string.c_str(),
                                       node_query_string.c_str() - a - 1);

          XLOG(D) << "parent node path: " << parent_node_path;
          XLOG(D) << "target node name: " << target_node_name;
          XLOG(D) << "target attribute: " << target_attribute_name;
        } else {
          XLOG(D) << "Illegal arguments: " << node_query_string;
        }

        break;
      case 'o':
        out_file_path = optarg;
        break;
#if ENABLE_NUMERIC_COMPAROR
      case 'n':
        using_numeric_comparator = true;
        break;
#endif
      case 'i':
        comparator_ignore_case = true;
        break;
      case 'd':
        acsecending_order = false;
        XLOG(D) << "choose descending order!";
        break;
      default:
        /* Not sure how to get here... */
        ret = EXIT_FAILURE;
        break;
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

  pugi::xml_document dx;

  pugi::xml_parse_result result = dx.load_file(in_file_path.c_str());

  if (!result) {
    XLOG(E) << "XML file(" << in_file_path
            << ") parsed with error: " << result.description() << " !";

    return -result.status;
  }

  if (!dx.empty()) {
    dx.save(std::cout);

    pugi::xpath_node target_parent_xnode =
        dx.select_node(parent_node_path.c_str());

    std::clock_t c_start = std::clock();

    auto t_start = std::chrono::high_resolution_clock::now();
    sort_xml_node(target_parent_xnode.node(), target_node_name.c_str(),
                  target_attribute_name.c_str(), acsecending_order,
                  using_numeric_comparator);
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
