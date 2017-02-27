#!/bin/bash

# Copyright 2017 Steven Hessing
# 
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
# 
#        http://www.apache.org/licenses/LICENSE-2.0
# 
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

openssl req \
    -x509 \
    -newkey rsa:4096 \
    -keyout noddosapiclient.pem \
    -out noddosapiclient.pem \
    -days 3650 \
    -nodes \
    -subj "/C=US/ST=noddosclientcert/L=Somewhere/O=Noddos/CN=client@noddos.io"  

fingerprint=`openssl x509 -noout -in noddosapiclient.pem -fingerprint 2>/dev/null | \
     sed 's|SHA1 Fingerprint=||' | tr -d ':'`

echo "Certificate fingerprint " $fingerprint
