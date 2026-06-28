/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief OpenSSL utility functions for TLS certificates
 */

#ifndef SRC_SHIP_TLS_CERTIFICATE_OPENSSL_UTIL_H_
#define SRC_SHIP_TLS_CERTIFICATE_OPENSSL_UTIL_H_

#include <openssl/x509.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Retrieves the Subject Key Identifier (SKI) as a string from the given X509 certificate.
 *
 * This function extracts the Subject Key Identifier (SKI) from the provided X509 certificate
 * and returns it as a null-terminated string. The SKI is a unique identifier for the certificate's
 * public key, often used in certificate chains to identify related certificates.
 *
 * @param cert A pointer to the X509 certificate from which the Subject Key Identifier will be extracted.
 *             This must be a valid, non-null pointer to an X509 structure.
 *
 * @return A pointer to a null-terminated string containing the Subject Key Identifier.
 *         If the SKI cannot be retrieved, the function may return a null pointer.
 *         The caller is responsible for deallocating the string with StringDelete().
 */
const char* OpenSslGetSubjectKeyIdString(const X509* cert);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_TLS_CERTIFICATE_OPENSSL_UTIL_H_
