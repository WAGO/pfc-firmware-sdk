//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file xmlhlp.cpp
/// 
/// \brief XML parsing/processing helpers.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "xmlhlp.hpp"

#include <libxml/xpathInternals.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>


namespace wago
{

static xmlChar * xmlChar_cast(char const * szValue)
{
  return reinterpret_cast<xmlChar*>(const_cast<char *>(szValue));
}

xmldoc::xmldoc(xmldoc&& other)
    : doc(other.doc)
{
    other.doc = NULL;
}

xmldoc::~xmldoc()
{
    if (NULL != doc)
        xmlFreeDoc(doc);
}

xmldoc& xmldoc::operator=(xmldoc&& rhs)
{
    if (NULL != doc)
        xmlFreeDoc(doc);
    doc = rhs.doc;
    rhs.doc = NULL;
    return *this;
}


xmlnode::xmlnode(xmlnode&& other)
    : node(other.node)
{
    other.node = NULL;
}

xmlnode::~xmlnode()
{
    // Free the node only if it is not assigned to a parent.
    if (NULL != node && NULL == node->parent)
        xmlFreeNode(node);
}

xmlnode& xmlnode::operator=(xmlnode&& rhs)
{
    if (NULL != node && NULL == node->parent)
        xmlFreeNode(node);
    
    node = rhs.node;
    rhs.node = NULL;

    return *this;
}


xmlattr::xmlattr(xmlattr&& other)
    : attr(other.attr)
{
    other.attr = NULL;
}

xmlattr::~xmlattr()
{
    // Free the attribute only if it is not assigned to a parent.
    if (NULL != attr && NULL == attr->parent)
        xmlFreeProp(attr);
}

xmlattr& xmlattr::operator=(xmlattr&& rhs)
{
    if (NULL != attr && NULL == attr->parent)
        xmlFreeProp(attr);
    
    attr = rhs.attr;
    rhs.attr = NULL;

    return *this;
}


xmlctx::xmlctx(xmlctx&& other)
    : ctx(other.ctx)
{
    other.ctx = NULL;
}

xmlctx::~xmlctx()
{
    if (NULL != ctx)
        xmlXPathFreeContext(ctx);
}

xmlctx& xmlctx::operator=(xmlctx&& rhs)
{
    if (NULL != ctx)
        xmlXPathFreeContext(ctx);
    ctx = rhs.ctx;
    rhs.ctx = NULL;
    return *this;
}


xmlpathobj::xmlpathobj(xmlpathobj&& other)
    : obj(other.obj)
{
    other.obj = NULL;
}

xmlpathobj::~xmlpathobj()
{
    if (NULL != obj)
        xmlXPathFreeObject(obj);
}

xmlpathobj& xmlpathobj::operator=(xmlpathobj&& rhs)
{
    if (NULL != obj)
        xmlXPathFreeObject(obj);

    obj = rhs.obj;
    rhs.obj = NULL;

    return *this;
}


xmldoc create_empty_doc()
{
    xmldoc doc(xmlNewDoc(xmlChar_cast("1.0")));

    if (doc.is_empty())
        throw std::runtime_error("Failed to create an empty xml document.");

    return doc;
}

xmldoc read_file(const std::string& fname)
{
    xmldoc doc(xmlReadFile(fname.c_str(), NULL, XML_PARSE_NOERROR));

    if (doc.is_empty())
        throw std::runtime_error("Failed to open/parse a given xml file.");

    return doc;
}

xmldoc parse_string(const std::string& xmldata)
{
    // See also xmlReadMemory.
    xmldoc doc(xmlParseDoc((const xmlChar*)xmldata.c_str()));

    if (doc.is_empty())
        throw std::runtime_error("Failed to parse a given xml string.");

    return doc;
}

void store_file(const std::string& fname, const xmldoc& doc)
{
    if (doc.is_empty())
        throw std::logic_error("Can't store null xml document.");

    // Save to temporary file first. If write succeeded, rename file and synchronize directory
    const int bytes = xmlSaveFormatFileEnc((fname + ".tmp").c_str(), doc.get(), "utf-8", 1);

    if (bytes < 0)
    {
        throw std::runtime_error("Failed to store xml document.");
    }
    else
    {
        sync();
        rename((fname + ".tmp").c_str(), fname.c_str());
        sync();
    }
}

std::string store_string(const xmldoc& doc)
{
    if (doc.is_empty())
        throw std::logic_error("Can't store null xml document.");
    
    xmlChar* doc_txt = NULL;
    int doc_len = 0;

    xmlDocDumpFormatMemoryEnc(doc.get(), &doc_txt, &doc_len, "utf-8", 1);

    if (NULL == doc_txt || doc_len <= 0)
        throw std::runtime_error("Could not transform xml doc into a string form.");

    const std::string sdoc((char*)doc_txt);

    xmlFree(doc_txt);

    return sdoc;
}

static void remove_text_nodes_recursion(xmlNode* node)
{
    if (NULL == node)
        return;

    xmlNode* child = node->children;

    while (NULL != child)
    {
        xmlNode* const curr = child;

        child = child->next;

        if (3 != curr->type)
            remove_text_nodes_recursion(curr);
        else if (NULL != curr->parent)
        {
            xmlUnlinkNode(curr);
            xmlFreeNode(curr);
        }
    }
}

void remove_text_nodes(const xmldoc& doc)
{
    if (doc.is_empty())
        return;

    xmlNode* const root = xmlDocGetRootElement(doc.get());

    remove_text_nodes_recursion(root);
}

xmlnode set_root_element(xmldoc& doc, xmlnode& root)
{
    if (NULL == doc.get())
        throw std::logic_error("Can't set root node to a null document.");

    return xmlnode(xmlDocSetRootElement(doc.get(), root.get()));
}

xmlnode create_node(const std::string& name)
{
    xmlnode node(xmlNewNode(NULL, xmlChar_cast(name.c_str())));

    if (NULL == node.get())
        throw std::runtime_error("Couldn't create a new node.");

    return node;
}

xmlnode append_node(xmlnode& parent, const std::string& name)
{
    if (NULL == parent.get())
        throw std::logic_error("Can't append node to a null parent.");

    xmlnode child(xmlNewChild(parent.get(), NULL, xmlChar_cast(name.c_str()), NULL));

    if (NULL == child.get())
        throw std::logic_error("Child node creation failed.");

    return child;
}

void remove_node(xmlnode node)
{
    if (NULL != node.get())
        xmlUnlinkNode(node.get());
}

xmlattr append_attribute(xmlnode& node, const std::string& name, const std::string& value)
{
    if (NULL == node.get())
        throw std::logic_error("Can't append attribute to a null node.");

    xmlattr attr(xmlNewProp(node.get(), xmlChar_cast(name.c_str()), xmlChar_cast(value.c_str())));

    if (NULL == attr.get())
        throw std::logic_error("Attribute creation failed.");

    return attr;
}

xmlattr update_attribute(xmlnode& node, const std::string& name, const std::string& value)
{
    if (NULL == node.get())
        throw std::logic_error("Can't set/update attribute from a null node.");

    xmlattr attr(xmlSetProp(node.get(), xmlChar_cast(name.c_str()), xmlChar_cast(value.c_str())));

    if (NULL == attr.get())
        throw std::logic_error("Attribute set/update failed.");

    return attr;
}

void remove_attribute(xmlnode& node, const std::string& name)
{
    if (NULL == node.get())
        throw std::logic_error("Can't remove attribute from a null node.");

    xmlattr attr(xmlHasProp(node.get(), xmlChar_cast(name.c_str())));

    if (NULL == attr.get())
        return;

    const int rval = xmlRemoveProp(attr.release());

    if (0 != rval)
        throw std::logic_error("Could not remove attribute.");
}


xmlctx create_xpath_ctx(const xmldoc& doc,
                        const std::string& prefix,
                        const std::string& ns_uri)
{
    if (doc.is_empty())
        throw std::logic_error("Can't create xpath context for an empty document.");

    xmlctx ctx = xmlctx(xmlXPathNewContext(doc.get()));

    if (ctx.is_empty())
        throw std::runtime_error("Could not create xpath context.");

    add_namespace(ctx, prefix, ns_uri);

    return ctx;
}

void add_namespace(const xmlctx& ctx,
                   const std::string& prefix,
                   const std::string& ns_uri)
{
    if (ctx.is_empty())
        throw std::logic_error("Can't add namespace to an empty context.");

    if (0 < prefix.size() && 0 < ns_uri.size())
    {
        const int rval = xmlXPathRegisterNs(ctx.get(),
                                            xmlChar_cast(prefix.c_str()),
                                            xmlChar_cast(ns_uri.c_str()));

        if (0 != rval)
            throw std::runtime_error("Can't add namespace.");
    }
}

xmlpathobj eval_xpath(const xmlctx& ctx, const std::string& xpath)
{
    return xmlpathobj(xmlXPathEvalExpression(xmlChar_cast(xpath.c_str()), ctx.get()));
}

xmlnode get_node(const xmlpathobj& xpo, const bool nonempty)
{
    const int count = get_node_count(xpo);

    if (1 != count)
    {
        if (0 == count && !nonempty)
            return xmlnode();
        else
            throw std::runtime_error("No node/to many nodes under a requested xpath.");
    }

    return xmlnode(xpo.get()->nodesetval->nodeTab[0]);
}

xmlnode get_node(const xmlctx& ctx, const std::string& xpath, const bool nonempty)
{
    return get_node(eval_xpath(ctx, xpath), nonempty);
}

int get_node_count(const xmlpathobj& xpo)
{
    xmlXPathObject* const obj(xpo.get());

    if (NULL != obj && XPATH_NODESET == obj->type && NULL != obj->nodesetval)
        return obj->nodesetval->nodeNr;
    else
        return 0;
}

int get_node_count(const xmlctx& ctx, const std::string& xpath)
{
    return get_node_count(eval_xpath(ctx, xpath));
}

std::string get_string(const xmlpathobj& xpo, const bool force)
{
    xmlXPathObject* const obj(xpo.get());

    if (force && (NULL == obj || XPATH_STRING != obj->type ||
        NULL == obj->stringval || 0 == ::strlen((char*)obj->stringval)))
        throw std::runtime_error("Queried xpath doesn't point to a string.");

    return (NULL != obj && XPATH_STRING == obj->type &&
            NULL != obj->stringval && 0 != strlen((char*)obj->stringval)) ?
                (char*)obj->stringval : "";
}

std::string get_string(const xmlctx& ctx,
                       const std::string& xpath,
                       const bool force)
{
    return get_string(eval_xpath(ctx, xpath), force);
}

// TODO: Is it OK to return vector directly? Move vs. copy!?
void get_attribute_value_list(const xmlctx& ctx,
                              const std::string& parent,
                              const std::string& attribute,
                              std::vector<std::string>& string_list)
{
    int count = 0;
    xmlpathobj const xpo = eval_xpath(ctx, parent + "//@" + attribute);

    if(!xpo.is_empty())
    {
        count = get_node_count(xpo);
    }

    if (0 < count)
    {
        xmlXPathObject* const obj(xpo.get());
        for (int i = 0; i < count; i++)
        {
            if ( XML_ATTRIBUTE_NODE == obj->nodesetval->nodeTab[i]->type )
            {
                string_list.push_back(std::string( (char *) obj->nodesetval->nodeTab[i]->last->content));
            }
            else
            {
                throw std::runtime_error("Queried xpath xmlElementType is not supported.");
            }
        }
    }
}

} // namespace wago

