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
#include "src/ship/tls_certificate/tls_certificate.h"

#include <iostream>

#include "src/common/eebus_malloc.h"

int main(int argc, char** argv) {
  std::cout << "Hello world!\n";
  TlsCertificateObject* tls_cert
      = TlsCertificateLoadX509KeyPair("/workspaces/repo/eebus/tests/evse.crt", "/workspaces/repo/eebus/tests/evse.key");
  std::cout << TLS_CERTIFICATE_GET_SKI(tls_cert) << "\n";
  TLS_CERTIFICATE_DESTRUCT(tls_cert);
  EEBUS_FREE(tls_cert);
  std::cout << "Done!\n";
}
