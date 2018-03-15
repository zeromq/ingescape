#!/bin/sh
#call first : ./build-wrapper-macosx-x86 --out-dir bw-output make clean all
sonar-scanner \
    -Dsonar.projectKey=mastic \
    -Dsonar.sources=. \
    -Dsonar.cfamily.build-wrapper-output=bw-output \
    -Dsonar.host.url=https://sonarcloud.io \
    -Dsonar.organization=ingenuityio-bitbucket \
    -Dsonar.login=fc9d839b16bd684b6055cb242b21e991f403a687
