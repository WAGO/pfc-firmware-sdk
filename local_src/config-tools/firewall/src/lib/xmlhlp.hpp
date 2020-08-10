//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file xmlhlp.hpp
/// 
/// \brief XML parsing/processing helpers.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_FIREWALL_XMLHLP_HPP_
#define WAGO_FIREWALL_XMLHLP_HPP_

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>


//
//  The module contains mostly thin RAII wrappers around C xml parser.
//  In addition to that there are few helper functions added for most commonly
//  performed xml actions.
//
//  Please note that all classes here have no copy constructors/operators
//  available. This is done so because they contain data which simpy can't be
//  copied in a meaningful/safe way.
//
//  Please note also that both xmlnode and xmlattr classes have different
//  destruction behaviour:
//  - if a node or an attribute has a parent, the data is not deleted - it can
//    be safely left attached to the parent,
//  - otherwise the internal data gets deleted.
//  In order to delete a node even if it is connected to a parent please use
//  'remove_node' function.
//
//  BEFORE YOU START
//
//  All RAII classes here contain no functionality by themselves.
//  It is recomended to leave them that way in order to keep the module clean
//  and directly compatible with the underlying C xml parser - user should be
//  able to call C functions with the classes (using get()) without any
//  uncertainity about possible uncompatibilities.
//  All additional functionality should be added as a set of separated
//  functions operating on the classes.
//


namespace wago {
namespace firewall {


// RAII class.
class xmldoc
{
public:
    explicit xmldoc(xmlDoc* const _doc = NULL)
            : doc(_doc) {}
    xmldoc(const xmldoc& other) = delete;
    xmldoc(xmldoc&& other);
    ~xmldoc();

    xmldoc& operator=(const xmldoc& rhs) = delete;
    xmldoc& operator=(xmldoc&& rhs);

    xmlDoc* get(void) const
            { return doc; }
    xmlDoc* release(void)
            { xmlDoc* const _doc(doc); doc = NULL; return _doc; }

    bool is_empty(void) const
            { return NULL == doc; }

private:
    xmlDoc* doc;
};


// RAII class.
class xmlnode
{
public:
    explicit xmlnode(xmlNode* const _node = NULL)
            : node(_node) {}
    xmlnode(const xmlnode& other) = delete;
    xmlnode(xmlnode&& other);
    ~xmlnode();

    xmlnode& operator=(const xmlnode& rhs) = delete;
    xmlnode& operator=(xmlnode&& rhs);

    xmlNode* get(void) const
            { return node; }
    xmlNode* release(void)
            { xmlNode* const _node(node); node = NULL; return _node; }

    bool is_empty(void) const
            { return NULL == node; }

private:
    xmlNode* node;
};


// RAII class.
class xmlattr
{
public:
    explicit xmlattr(xmlAttr* const _attr = NULL)
            : attr(_attr) {}
    xmlattr(const xmlattr& other) = delete;
    xmlattr(xmlattr&& other);
    ~xmlattr();

    xmlattr& operator=(const xmlattr& rhs) = delete;
    xmlattr& operator=(xmlattr&& rhs);

    xmlAttr* get(void) const
            { return attr; }
    xmlAttr* release(void)
            { xmlAttr* const _attr(attr); attr = NULL; return _attr; }

    bool is_empty(void) const
            { return NULL == attr; }

private:
    xmlAttr* attr;
};


// RAII class.
class xmlctx
{
public:
    explicit xmlctx(xmlXPathContext* const _ctx = NULL)
            : ctx(_ctx) {}
    xmlctx(const xmlctx& other) = delete;
    xmlctx(xmlctx&& other);
    ~xmlctx();

    xmlctx& operator=(const xmlctx& rhs) = delete;
    xmlctx& operator=(xmlctx&& rhs);

    xmlXPathContext* get() const
            { return ctx; }
    xmlXPathContext* release()
            { xmlXPathContext* const _ctx(ctx); ctx = NULL; return _ctx; }

    bool is_empty(void) const
            { return NULL == ctx; }

private:
    xmlXPathContext* ctx;
};


// RAII class.
class xmlpathobj
{
public:
    explicit xmlpathobj(xmlXPathObject* const _obj = NULL)
            : obj(_obj) {}
    xmlpathobj(const xmlpathobj& other) = delete;
    xmlpathobj(xmlpathobj&& other);
    ~xmlpathobj();

    xmlpathobj& operator=(const xmlpathobj& rhs) = delete;
    xmlpathobj& operator=(xmlpathobj&& rhs);

    xmlXPathObject* get(void) const
            { return obj; }
    xmlXPathObject* release(void)
            { xmlXPathObject* const _obj(obj); obj = NULL; return _obj; }

    bool is_empty(void) const
            { return NULL == obj; }

private:
    xmlXPathObject* obj;
};


//------------------------------------------------------------------------------
/// Creates an empty xml document.
/// Please note that empty doesn't mean 'is_empty' - the object is created and
/// can be operated on.
/// \return newly created xml document
//------------------------------------------------------------------------------
xmldoc create_empty_doc();

//------------------------------------------------------------------------------
/// Parses xml file.
/// \param fname path to a file which should be parsed
/// \return newly created xml document
//------------------------------------------------------------------------------
xmldoc read_file(const std::string& fname);

//------------------------------------------------------------------------------
/// Parses string file.
/// \param xmldata string to be parsed as xml data
/// \return newly created xml document
//------------------------------------------------------------------------------
xmldoc parse_string(const std::string& xmldata);

//------------------------------------------------------------------------------
/// Stores xml document to a file.
/// \param path to a file which should be written to
/// \param doc xml document to be stored
//------------------------------------------------------------------------------
void store_file(const std::string& fname, const xmldoc& doc);

//------------------------------------------------------------------------------
/// Serializes xml document to a string.
/// \param doc xml document to be serialized
/// \return xml document in serialized form
//------------------------------------------------------------------------------
std::string store_string(const xmldoc& doc);

//------------------------------------------------------------------------------
/// Removes all text nodes from an xml document.
/// \param doc xml document to be operated on
/// \note the function contains internal recursion
//------------------------------------------------------------------------------
void remove_text_nodes(const xmldoc& doc);

//------------------------------------------------------------------------------
/// Sets a (new) root node for a given document.
/// \param doc xml document to be operated on
/// \param root new root element
/// \return previous root node
//------------------------------------------------------------------------------
xmlnode set_root_element(xmldoc& doc, xmlnode& root);

//------------------------------------------------------------------------------
/// Creates a new xml node.
/// \param name tag name of the new element
/// \return newly created node
//------------------------------------------------------------------------------
xmlnode create_node(const std::string& name);

//------------------------------------------------------------------------------
/// Creates a new xml node and appends it to a given parent node.
/// \param parent parent node to which the new node should be appended as a child
/// \param name tag name of the new element
/// \return newly created node (can be safely ignored - its already appended)
//------------------------------------------------------------------------------
xmlnode append_node(xmlnode& parent, const std::string& name);


//------------------------------------------------------------------------------
/// Completely deletes/destroys a given node.
/// Please note that in order to call the function successfully the node object
/// must be passed using std::move function: remove_node(std::move(anode));
/// The function unlinks the node from its parent and then deletes it including
/// its all children.
/// \param node to be deleted
//------------------------------------------------------------------------------
void remove_node(xmlnode node);

//------------------------------------------------------------------------------
/// Appends new attribute to a node.
/// \param node to be operated on
/// \param name a name of the new attribute
/// \param value a value of the new attribute
/// \return newly created attribute (can be safely ignored - its already appended)
//------------------------------------------------------------------------------
xmlattr append_attribute(xmlnode& node, const std::string& name, const std::string& value);

//------------------------------------------------------------------------------
/// Updates an existing attribute.
/// \param node to be operated on
/// \param name a name of the attribute to be updated
/// \param value a new value of the attribute
/// \return updated attribute (can be safely ignored)
//------------------------------------------------------------------------------
xmlattr update_attribute(xmlnode& node, const std::string& name, const std::string& value);

//------------------------------------------------------------------------------
/// Removes an existing attribute.
/// \param node to be operated on
/// \param name a name of the attribute to be removed
//------------------------------------------------------------------------------
void remove_attribute(xmlnode& node, const std::string& name);


//------------------------------------------------------------------------------
/// Creates an xpath context.
/// This function allows to create a context and attach to it a required
/// namespace. More namespaces can be added later if required.
/// Please see description of add_namespace function.
/// \param doc xml document for which the context should be created
/// \param prefix xml namespace prefix
/// \param ns_uri xml namespace
/// \return newly created context
//------------------------------------------------------------------------------
xmlctx create_xpath_ctx(const xmldoc& doc,
                        const std::string& prefix = "",
                        const std::string& ns_uri = "");

//------------------------------------------------------------------------------
/// Adds a namespace to a context.
/// This function allows to create a context and attach to it a required
/// namespace. More namespaces can be added later if required.
/// \note Please remember that xpath doesn't work with default namespaces.
///       If a namespace is to be attached to a context its prefix can't be
///       empty.
/// \param ctx xml context to which the namespace should be attached
/// \param prefix xml namespace prefix
/// \param ns_uri xml namespace
//------------------------------------------------------------------------------
void add_namespace(const xmlctx& ctx,
                   const std::string& prefix,
                   const std::string& ns_uri);

//------------------------------------------------------------------------------
/// Evaluates xpath expression.
/// \param ctx xml context to be used
/// \param xpath xpath to be evaluated
/// \return object of evaluated xpath expression
//------------------------------------------------------------------------------
xmlpathobj eval_xpath(const xmlctx& ctx, const std::string& xpath);

//------------------------------------------------------------------------------
/// Returns node pointed to by evaluated xpath object.
/// \param xpo evaluated xpath object
/// \param nonempty true allows to return an empty node object if no match was
///                 found, false requires such a node to exist and an exception
///                 is thrown if it doesn't
/// \return node pointed to by the evaluated xpath object
//------------------------------------------------------------------------------
xmlnode get_node(const xmlpathobj& xpo,
                 const bool nonempty = true);

//------------------------------------------------------------------------------
/// Returns node pointed to by xpath expression.
/// \param ctx xml context to be used
/// \param xpath xpath expression which should point to a node
/// \param nonempty true allows to return an empty node object if no match was
///                 found, false requires such a node to exist and an exception
///                 is thrown if it doesn't
/// \return node pointed to by the xpath expression
//------------------------------------------------------------------------------
xmlnode get_node(const xmlctx& ctx,
                 const std::string& xpath,
                 const bool nonempty = true);

//------------------------------------------------------------------------------
/// Returns node count.
/// \param xpo evaluated xpath object
/// \return count of nodes pointed to by the evaluated xpath object
//------------------------------------------------------------------------------
int get_node_count(const xmlpathobj& xpo);

//------------------------------------------------------------------------------
/// Returns node count.
/// \param ctx xml context to be used
/// \param xpath xpath expression which should point to a node(s)
/// \return count of nodes pointed to by the xpath expression
//------------------------------------------------------------------------------
int get_node_count(const xmlctx& ctx, const std::string& xpath);

//------------------------------------------------------------------------------
/// Returns a string pointed to by an evealuated xpath object.
/// \param xpo evaluated xpath object
/// \param force true means that the string should be there, it is required,
///              false allows an empty string to be returned.
/// \return requested string
//------------------------------------------------------------------------------
std::string get_string(const xmlpathobj& xpo,
                       const bool force = true);

//------------------------------------------------------------------------------
/// Returns a string pointed to by an xpath expression.
/// \param ctx xml context to be used
/// \param xpath xpath expression
/// \param force true means that the string should be there, it is required,
///              false allows an empty string to be returned.
/// \return requested string
//------------------------------------------------------------------------------
std::string get_string(const xmlctx& ctx,
                       const std::string& xpath,
                       const bool force = true);


//------------------------------------------------------------------------------
///
/// Returns list of attribute values.
///
/// \param ctx
///   xml context to be used.
/// \param parent
///   ancestor xpath expression. All nodes below this ancestor will be searched for given attribute.
/// \param attribute
///   attribute name to search for.
/// \return string_list
///   String list of attribute values.
///
//------------------------------------------------------------------------------
std::vector<std::string> get_attribute_value_list(const xmlctx& ctx,
                                                  const std::string& ancestor,
                                                  const std::string& attribute);

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_XMLHLP_HPP_

