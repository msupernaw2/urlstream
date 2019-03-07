// -*- C++ -*-
/* 
 * File:   xml.hpp
 * Author: matthewsupernaw
 *
 * Created on August 15, 2015, 9:23 AM
 */

#ifndef XML_HPP
#define	XML_HPP

#include <string>


#define DEBUG false

#define TEXT                1
#define ENTITY              2
#define OPEN_TAG            3
#define CLOSE_TAG           4
#define START_TAG           5
#define ATTRIBUTE_LVALUE    6
#define ATTRIBUTE_EQUAL     9
#define ATTRIBUTE_RVALUE    10
#define QUOTE               7
#define IN_TAG              8
#define SINGLE_TAG          12
#define COMMENT             13
#define DONE                11
#define DOCTYPE             14
#define PRE                 15
#define CDATA               16

class meta_data_element {
public:
    std::string name;
    std::string value;

    std::string get_name() const {
        return name;
    }

    void set_name(std::string name) {
        this->name = name;
    }

    std::string get_value() const {
        return value;
    }

    void set_value(std::string value) {
        this->value = value;
    }

};

class xml_node {
    std::vector<meta_data_element> meta_data;
    std::string data;
    std::vector<xml_node*> children;
    int depth;
    std::string name;
    int parent_id;
public:
    bool is_closed;

    typedef std::vector<meta_data_element >::iterator meta_data_itererator;
    typedef std::vector<xml_node*>::iterator child_itererator;

    xml_node() : is_closed(false) {

    }

    xml_node(const xml_node& other) :
    meta_data(other.meta_data),
    data(other.data),
    children(other.children), 
    depth(other.depth), 
    name(other.name), 
    parent_id(other.parent_id),
    is_closed(other.is_closed) {
    }

    meta_data_itererator meta_data_begin() {
        return meta_data.begin();
    }

    meta_data_itererator meta_data_end() {
        return meta_data.end();
    }

    child_itererator children_begin() {
        return children.begin();
    }

    child_itererator children_end() {
        return children.end();
    }

    void add_node( xml_node* node) {
        this->children.push_back(node);      
    }

    std::vector<xml_node*> get_children() const {
        return children;
    }

    std::string get_data() const {
        return data;
    }

    void set_data(std::string data) {
        this->data = data;
    }

    int get_depth() const {
        return depth;
    }

    void set_depth(int depth) {
        this->depth = depth;
    }

    std::vector<meta_data_element> get_meta_data() const {
        return meta_data;
    }

    void set_meta_data(std::vector<meta_data_element> meta_data) {
        this->meta_data = meta_data;
    }

    std::string get_name() const {
        return name;
    }

    void set_name(std::string name) {
        this->name = name;
    }

    int get_parent_id() const {
        return parent_id;
    }

    void set_parent_id(int parent_id) {
        this->parent_id = parent_id;
    }


};

class xml {
    std::vector<xml_node> nodes;
    xml_node* root;
    int depth;
public:

    typedef std::vector<xml_node>::iterator iterator;
    typedef std::vector<xml_node>::const_iterator const_iterator;

    /**
     * Open and parse a xml file.
     * @param file
     */
    void open(const std::string& file) {

        std::ifstream in;
        in.open(file.c_str());

        std::string line;

        std::stringstream ss;

        while (in.good()) {
            std::getline(in, line);
            ss << line << "\n";
        }

        in.close();
        parse(ss.str());
    }

    /**
     * Parse xml from a string.
     * @param str
     */
    void parse(const std::string& in) {

        std::vector<int> st;
        long counter = 0;
        char buffer[1];
        char quotec = '"';
        bool eol = false;
        int line = 1;
        int col = 0;
        depth = 0;
        int mode = PRE;
        std::string sb = "";
        std::string tagName = "";
        std::string etag = "";
        std::string lvalue = ""; // = null;
        std::string rvalue = ""; // = null;
        xml_node node;
        std::vector<meta_data_element> meta;
        int index = 0;
        char* data = (char*) in.c_str();
        while (index != in.length()) {

            buffer[0] = data[index];
            index++;

            if (DEBUG) {
                std::cout << mode << sb << "|" << std::endl;
            }

            //
            if (buffer[0] == '\n' && eol) {
                eol = false;
                continue;
            } else if (eol) {
                eol = false;
            } else if (buffer[0] == '\n') {
                line++;
                col = 0;
            } else if (buffer[0] == '\r') {
                eol = true;
                buffer[0] = '\n';
                line++;
                col = 0;
            } else {
                col++;
            }

            if (mode == DONE) {
                return;
            } else if (mode == TEXT) {

                if (buffer[0] == '[' && sb == "![CDATA") {
                    mode = CDATA;
                    st.push_back(mode);
                    sb = "";

                } else if (buffer[0] == '<') {
                    st.push_back(mode);
                    mode = START_TAG;
                    this->data(sb);
                    sb = "";


                } else if (buffer[0] == '&') {
                    st.push_back(mode);
                    mode = ENTITY;
                    etag = "";
                } else {

                    if (buffer[0] != '\n' & buffer[0] != '\t') {
                        sb.append(buffer, 1);
                    }
                }
            } else if (mode == CLOSE_TAG) {
                if (buffer[0] == '>') {
                    mode = st.at(0);
                    tagName = sb;
                    sb = "";
                    depth--;
                    if (depth == 0) {
                        mode = DONE;
                    }
                    this->end_element(tagName);
                } else {
                    sb.append(buffer, 1);
                }

                // we are processing CDATA
            } else if (mode == CDATA) {

                if (buffer[0] == ']' && sb == "]>") {

                    mode = START_TAG;

                    sb = sb.substr(0, sb.length() - 2);
                    depth--;
                    this->data(sb);
                    sb = "";
                    //
                } else {
                    sb.append(buffer, 1);
                }
                // we are processing a comment.  We are inside
                // the <!-- .... --> looking for the -->.
            } else if (mode == COMMENT) {
                if (buffer[0] == '>' && ends_with(sb, "--")) {
                    sb = "";
                    mode = PRE; // st.at(0);
                } else {
                    sb.append(buffer, 1);
                }

                // We are outside the root tag element
            } else if (mode == PRE) {
                if (buffer[0] == '<') {
                    this->start_document();
                    mode = TEXT;
                    st.push_back(mode);
                    mode = START_TAG;
                }
                // We are inside one of these <? ... ?>
                // or one of these <!DOCTYPE ... >
            } else if (mode == DOCTYPE) {
                if (buffer[0] == '>') {
                    mode = st.at(0);
                    if (mode == TEXT) {
                        mode = PRE;
                    }
                }

                // we have just seen a < and
                // are wondering what we are looking at
                // <foo>, </foo>, <!-- ... --->, etc.
            } else if (mode == START_TAG) {
                mode = st.at(0);



                if (buffer[0] == '!' && sb == "[CDATA[") {
                    mode = CDATA;
                    st.push_back(mode);
                    sb = "";

                } else if (buffer[0] == '/') {
                    st.push_back(mode);

                    sb = "";
                    mode = CLOSE_TAG;
                } else if (buffer[0] == '?') {
                    mode = DOCTYPE;
                } else if (buffer[0] == '!' && sb == "--") {
                    mode = COMMENT;
                } else {
                    st.push_back(mode);
                    mode = OPEN_TAG;
                    tagName = "";
                    meta = std::vector<meta_data_element>();
                    sb.append(buffer, 1);
                }

                // we are processing an entity, e.g. &lt;, &#187;, etc.
            } else if (mode == ENTITY) {

                if (buffer[0] == ';') {
                    std::string cent = etag;
                    mode = st.back(); //at(0);
                    etag = "";
                    if (cent == "lt") {
                        sb.append("<");
                    } else if (cent == "gt") {
                        sb.append(">");
                    } else if (cent == "amp") {
                        sb.append("&");
                    } else if (cent == "quot") {
                        sb.append("\"");
                    } else if (cent == "apos") {
                        sb.append("'");
                    }// Could parse hex entities if we wanted to
                        //else if(cent.startsWith("#x"))
                        //sb.append((char)Integer.parseInt(cent.substring(2),16));
                    else if (starts_with(cent, "#")) {
                        // sb.append((char) Integer.parseInt(cent.substring(1)));
                    }// Insert custom entity definitions here
                    else {
                        this->malformed_xml_exception("Unknown entity: &" + cent + ";", line, col);
                    }
                } else {
                    etag.append(buffer, 1);
                }

                // we have just seen something like this:
                // <foo a="b"/
                // and are looking for the final >.
            } else if (mode == SINGLE_TAG) {
                if (tagName == "") {
                    tagName = sb;
                }
                if (buffer[0] != '>') {
                    this->malformed_xml_exception("Expected > for tag: <" + tagName + "/>", line, col);
                }
                this->start_element(tagName, meta);
                this->end_element(tagName);
                if (depth == 0) {
                    this->end_document();
                    return;
                }
                sb = "";
                tagName = "";
                mode = st.at(0);

                // we are processing something
                // like this <foo ... >.  It could
                // still be a <!-- ... --> or something.
            } else if (mode == OPEN_TAG) {
                if (buffer[0] == '>') {
                    if (tagName == "") {
                        tagName = sb;
                    }
                    sb = "";
                    depth++;
                    this->start_element(tagName, meta);
                    tagName = "";
                    //meta = new std::vector<MetaData*>();
                    mode = st.at(0);
                } else if (buffer[0] == '/') {
                    mode = SINGLE_TAG;
                } else if (buffer[0] == '-' && sb == "!-") {
                    mode = COMMENT;
                } else if (buffer[0] == '[' && sb == "![CDATA") {
                    mode = CDATA;
                    sb = "";
                } else if (buffer[0] == 'E' && sb == "!DOCTYP") {
                    sb = "";
                    mode = DOCTYPE;
                } else if (buffer[0] == ' ') {
                    tagName = sb;
                    sb = "";
                    mode = IN_TAG;
                } else {
                    sb.append(buffer, 1);
                }

                // We are processing the quoted right-hand side
                // of an element's attribute.
            } else if (mode == QUOTE) {
                if (buffer[0] == quotec) {
                    rvalue = sb;
                    sb = "";
                    meta_data_element metaData;
                    metaData.set_name(lvalue);
                    metaData.set_value(rvalue);
                    meta.push_back(metaData);
                    st.push_back(mode);
                    mode = IN_TAG;
                    // See section the XML spec, section 3.3.3
                    // on normalization processing.
                }/*else if (" \r\n\u0009".indexOf(c) >= 0) {
                sb.append(' ');
            } */ else if (buffer[0] == '&') {
                    st.push_back(mode);
                    mode = ENTITY;
                    etag = "";
                } else {
                    sb.append(buffer, 1);
                }

            } else if (mode == ATTRIBUTE_RVALUE) {
                if (buffer[0] == '"' || buffer[0] == '\'') {
                    quotec = buffer[0];
                    mode = QUOTE;

                } else if (buffer[0] == ' ') {
                    ;
                } else {
                    this->malformed_xml_exception("Error in attribute processing", line, col);
                }

            } else if (mode == ATTRIBUTE_LVALUE) {
                if (buffer[0] == ' ') {
                    lvalue = sb;
                    sb = "";
                    mode = ATTRIBUTE_EQUAL;
                    st.push_back(mode);
                } else if (buffer[0] == '=') {
                    lvalue = sb;
                    sb = "";
                    mode = ATTRIBUTE_RVALUE;
                    st.push_back(mode);
                } else {
                    sb.append(buffer, 1);
                }

            } else if (mode == ATTRIBUTE_EQUAL) {
                if (buffer[0] == '=') {
                    mode = ATTRIBUTE_RVALUE;
                    st.push_back(mode);
                } else if (buffer[0] == ' ') {
                    ;
                } else {
                    this->malformed_xml_exception("Error in attribute processing.", line, col);
                }

            } else if (mode == IN_TAG) {

                if (buffer[0] == '>') {
                    mode = st.at(0);
                    this->start_element(tagName, meta);
                    depth++;
                    tagName = "";
                } else if (buffer[0] == '/') {
                    mode = SINGLE_TAG;
                } else if (buffer[0] == ' ') {
                    ;
                } else {
                    mode = ATTRIBUTE_LVALUE;

                    sb.append(buffer, 1);
                }
            }
            if (mode == DONE) {
                this->end_document();
            }
        }


    }

    xml_node* get_root() {
        return root;
    }

private:

    void start_element(const std::string tag, std::vector<meta_data_element>& meta) {


        xml_node n;
        n.set_name(tag);
        n.set_meta_data(meta);
        n.set_parent_id(this->get_last_open_index());
        this->nodes.push_back(n);
    }

    int get_last_open_index() {

        for (int i = this->nodes.size() - 1; i > -1; i--) {
            if (!nodes[i].is_closed) {
                return i;
            }
        }

        return -1;
    }

    xml_node* get_last_open_node() {

        for (int i = this->nodes.size() - 1; i > -1; i--) {
            if (!nodes[i].is_closed) {
                return &nodes[i];
            }
        }

        return NULL;
    }

    void data(std::string data) {


        // Trim Both leading and trailing spaces
        size_t startpos = data.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
        size_t endpos = data.find_last_not_of(" \t"); // Find the first character position from reverse af

        // if all spaces or empty return an empty string
        if ((std::string::npos == startpos) || (std::string::npos == endpos)) {
            data = "";
        } else
            data = data.substr(startpos, endpos - startpos + 1);


        xml_node* n = get_last_open_node();
        n->set_data(data);

    }

    bool ends_with(std::string const &fullString, std::string const &ending) {
        size_t lastMatchPos = fullString.rfind(ending); // Find the last occurrence of ending
        bool isEnding = lastMatchPos != std::string::npos; // Make sure it's found at least once

        // If the string was found, make sure that any characters that follow it are the ones we're trying to ignore
        for (int i = lastMatchPos + ending.length(); (i < fullString.length()) && isEnding; i++) {
            if ((fullString[i] != '\n') &&
                    (fullString[i] != '\r')) {
                isEnding = false;
            }
        }

        return isEnding;
    }

    bool starts_with(const std::string& fullString, const std::string& start) {

        if (fullString.length() < start.length() || fullString.length() == 0)
            return false;

        for (unsigned int i = 0; i < start.length(); ++i) {
            if (fullString[i] != start[i])
                return false;
        }

        return true;
    }

    void end_element(std::string tag) {

        for (int i = this->nodes.size() - 1; i > -1; i--) {
            xml_node* n = (xml_node*) & nodes[i];
            if (tag == n->get_name() && !n->is_closed) {
                n->is_closed = true;
            }

        }
    }

    void start_document() {


    }

    void end_document() {

        for (int i = 1; i < this->nodes.size(); i++) {
            xml_node* node = (xml_node*) & this->nodes.at(i);
            int parentPosition = this->nodes[i].get_parent_id();
            this->nodes[parentPosition].add_node(&nodes[i]);
        }

        this->root = (xml_node*) & this->nodes[0];

    }

    void malformed_xml_exception(std::string error, int line, int column) {
        std::cerr << error << " near line " << line << ", column " << column << std::endl;
        exit(0);
    }
};

#endif	/* XML_HPP */

