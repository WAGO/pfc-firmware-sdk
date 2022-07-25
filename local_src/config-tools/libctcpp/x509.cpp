//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include "x509.hpp"
#include <openssl/err.h>
#include <openssl/pem.h>
#include <iostream>


namespace wago {


x509::~x509()
{
    if (nullptr != cert)
        X509_free(cert);
}


std::string rdname::get_oi_name(void) const noexcept
{
    const char* const sn = OBJ_nid2sn(oi);

    return std::string(nullptr != sn ? sn : "");
}

std::string rdname::get_oi_name_long(void) const noexcept
{
    const char* const ln = OBJ_nid2ln(oi);

    return std::string(nullptr != ln ? ln : "");
}


void dump_x509_errors(void)
{
    // unsigned long ERR_get_error(void);

    unsigned long err;
    const char* file = nullptr;
    int line = 0;
    const char *data = nullptr;
    int flags = 0;

    while (0 != (err = ERR_get_error_line_data(&file, &line, &data, &flags)))
    {
        std::cout << "x509 error: " << err << ", line: " << line << ", file: " << file
                  << ", flags: " << flags << ", data: " << data << std::endl;
    }
}


static x509 parse_file(const std::string& fname, const int type = 0)
{
    FILE* const fp = fopen(fname.c_str(), "rb");

    if (nullptr == fp)
        return x509();

    X509* c = nullptr;

    if (0 == type)
        c = d2i_X509_fp(fp, nullptr);
    else
        c = PEM_read_X509(fp, nullptr, nullptr, (void*)"");

    fclose(fp);

    return x509(c);
}


x509 parse_x509_file(const std::string& fname)
{
    x509 c = parse_file(fname, 1);

    if (c.is_empty())
        c = parse_file(fname, 0);

    return c;
}


std::string get_notBefore(const x509& cert)
{
    ASN1_TIME* const tm = X509_get_notBefore(cert());
    return (char*)tm->data;
}


std::string get_notAfter(const x509& cert)
{
    ASN1_TIME* const tm = X509_get_notAfter(cert());
    return (char*)tm->data;
}


static std::vector<rdname> get_isssub_name(X509_NAME* const nm)
{
    std::vector<rdname> name;

    if (nullptr == nm)
        return name;

    for (int i = 0; i < X509_NAME_entry_count(nm); ++i)
    {
        X509_NAME_ENTRY* const ne = X509_NAME_get_entry(nm, i);
        ASN1_STRING* const st = X509_NAME_ENTRY_get_data(ne);
        ASN1_OBJECT* const ob = X509_NAME_ENTRY_get_object(ne);
        const int nid = OBJ_obj2nid(ob);

        name.push_back(rdname(nid, (char*)st->data));
    }

    return name;
}


std::vector<rdname> get_issuer_name(const x509& cert)
{
    X509_NAME* const nm = X509_get_issuer_name(cert());

    return get_isssub_name(nm);
}


std::vector<rdname> get_subject_name(const x509& cert)
{
    X509_NAME* const nm = X509_get_subject_name(cert());

    return get_isssub_name(nm);
}


} // namespace wago

