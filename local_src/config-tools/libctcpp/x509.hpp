//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#ifndef WAGO_X509_HPP
#define WAGO_X509_HPP


#include <openssl/x509.h>
#include <string>
#include <vector>


namespace wago {


// RAII
class x509
{
public:
    inline x509() noexcept
        : cert(nullptr) {}
    explicit inline x509(X509* const pure) noexcept
        : cert(pure) {}
    x509(const x509&) = delete;
    inline x509(x509&& other) noexcept
        : cert(other.cert) { other.cert = nullptr; }
    ~x509();

    x509& operator=(const x509&) = delete;
    x509& operator=(x509&& rhs) noexcept
        { cert = rhs.cert; rhs.cert = nullptr; return *this; }

    inline X509* release(void) noexcept
        { X509* const c = cert; cert = nullptr; return c; }

    inline X509* operator()(void) const noexcept
        { return cert; }

    inline bool is_empty(void) const noexcept
        { return nullptr == cert; }

private:
    X509* cert;
};


class rdname
{
public:
    inline rdname()
        : oi(), name() {}
    inline rdname(const int _oi, const std::string& _name)
        : oi(_oi), name(_name) {}

    inline int get_oi(void) const noexcept
        { return oi; }
    inline std::string get_name(void) const noexcept
        { return name; }

    std::string get_oi_name(void) const noexcept;
    std::string get_oi_name_long(void) const noexcept;

private:
    int oi;
    std::string name;
};


void dump_x509_errors(void);

x509 parse_x509_file(const std::string& fname);

std::string get_notBefore(const x509& cert);
std::string get_notAfter(const x509& cert);
std::vector<rdname> get_issuer_name(const x509& cert);
std::vector<rdname> get_subject_name(const x509& cert);


} // namespace wago


#endif // WAGO_X509_HPP
