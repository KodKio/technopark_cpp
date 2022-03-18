#!/bin/bash

FILE=/opt/infer-linux64-v$VERSION/bin/infer
if test -f "$FILE"; then
    echo "$FILE exists."
else
  VERSION=1.0.0; \
  curl -sSL "https://github.com/facebook/infer/releases/download/v$VERSION/infer-linux64-v$VERSION.tar.xz" \
  | sudo tar -C /opt -xJ && \
  sudo ln -s "/opt/infer-linux64-v$VERSION/bin/infer" /usr/local/bin/infer
fi