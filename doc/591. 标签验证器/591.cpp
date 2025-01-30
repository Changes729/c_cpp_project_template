#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

#define XML_START_CH '<'
#define XML_TAG_END_CH '/'
#define XML_CDATA_BEGIN_CH '!'
#define XML_END_CH '>'
#define XML_CDATA_TAG_BEGIN "![CDATA["
#define XML_CDATA_TAG_END "]]>"
#define XML_TAG_MAX_LENGTH 10

enum xml_solution_state_t {
  XML_STATE_CHECK_CONTENT,
  XML_STATE_CHECK_XML_BEGIN,
  XML_STATE_CHECK_XML_DATA,
  XML_STATE_CHECK_XML_CDATA_BEGIN,
  XML_STATE_CHECK_XML_CDATA,
  XML_STATE_CHECK_XML_CDATA_END,
  XML_STATE_CHECK_XML_END,
  XML_STATE_CHECK_XML_RETURN,
};

class Solution {
public:
  bool isValid(string code) {
    int index = 0;
    bool res = true;
    res = check_content(code, index, false);
    return res && index == code.length();
  }

  /**
   * @brief
   *
   * @param code
   * @param index
   * @param res
   * @return index of check end;
   */
  int check_content(string &code, int &index, bool in_div) {
    bool res = true;
    xml_solution_state_t state = XML_STATE_CHECK_CONTENT;
    char buffer[XML_TAG_MAX_LENGTH]{};
    int tag_length = 0;

    for (; index < code.length() && res == true &&
           state != XML_STATE_CHECK_XML_RETURN;
         index++) {
      char c = code[index];
      switch (state) {
      case XML_STATE_CHECK_CONTENT: {
        if (c == XML_START_CH) {
          state = XML_STATE_CHECK_XML_BEGIN;
          tag_length = 0;
        } else {
          res = in_div;
        }
      } break;
      case XML_STATE_CHECK_XML_BEGIN: {
        if (c == XML_CDATA_BEGIN_CH) {
          state = XML_STATE_CHECK_XML_CDATA_BEGIN;
          res = in_div && (code.compare(index, sizeof(XML_CDATA_TAG_BEGIN) - 1,
                                        XML_CDATA_TAG_BEGIN) == 0);
          index = index + sizeof(XML_CDATA_TAG_BEGIN) - 2;
          state = XML_STATE_CHECK_XML_CDATA;
        } else if (c == XML_TAG_END_CH) {
          if (tag_length == 0) {
            state = XML_STATE_CHECK_XML_RETURN;
            index = index - 1; // TODO:
          } else {
            state = XML_STATE_CHECK_XML_END;
          }
        } else if (isupper(c) && tag_length < XML_TAG_MAX_LENGTH - 1) {
          state = XML_STATE_CHECK_XML_DATA;
          buffer[tag_length] = c;
          tag_length = tag_length + 1;
          buffer[tag_length] = '\0';
        } else {
          res = false;
        }
      } break;
      case XML_STATE_CHECK_XML_DATA: {
        if (c == XML_END_CH) {
          state = XML_STATE_CHECK_XML_END;
          res = check_content(code, index, true);
        } else if (isupper(c) && tag_length < XML_TAG_MAX_LENGTH - 1) {
          buffer[tag_length] = c;
          tag_length = tag_length + 1;
          buffer[tag_length] = '\0';
        } else {
          res = false;
        }
      } break;
      case XML_STATE_CHECK_XML_CDATA: {
        if (code.compare(index, sizeof(XML_CDATA_TAG_END) - 1,
                         XML_CDATA_TAG_END) == 0) {
          state = XML_STATE_CHECK_CONTENT;
          index = index + sizeof(XML_CDATA_TAG_END) - 2;
        }
      } break;
      case XML_STATE_CHECK_XML_END: {
        if (tag_length == 0) {
          res = false;
        } else {
          state = in_div ? XML_STATE_CHECK_CONTENT : XML_STATE_CHECK_XML_RETURN;
          res = (code.compare(index, tag_length, buffer) == 0);
          index = index + tag_length;
          res = res && (code[index] == XML_END_CH);
        }
      } break;

      default:
        break;
      }
    }

    return res && (state == XML_STATE_CHECK_XML_RETURN ||
                   state == XML_STATE_CHECK_CONTENT);
  }
};
