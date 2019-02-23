#!/bin/sh
cp -R ../../../../agents/learning/firstFullAgent .
find . -name ".DS_Store" -type f -delete
rm -f *.zip
zip -r -X firstFullAgent.zip firstFullAgent
rm -Rf firstFullAgent

#zip -r -X firstAgent.zip firstAgent
#zip -r -X firstAgent_Qt.zip firstAgent_Qt
#zip -r -X firstAgent_VS.zip firstAgent_VS
#zip -r -X firstFullAgent_Qt.zip firstFullAgent_Qt
#zip -r -X firstFullAgent_VS.zip firstFullAgent_VS
