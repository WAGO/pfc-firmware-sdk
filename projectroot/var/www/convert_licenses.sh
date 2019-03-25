#!/bin/sh

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

set -e

find licenses -type f | while read F; do
  TMP="$F.$$"
  format=""
  # Copy orig file to TMP while replacing CRLF by \n
  tr -d '\r' < "$F" > "$TMP"
  case "$(file -bi "$F")" in
    *=us-ascii|*=utf-8)
      ;;
    *=iso-8859-1)
      format="iso-8859-1"
      ;;
    *=unknown-8bit)
      # This are UTF8 files with ASCII copyright 0xA9, which is invalid UTF8
      sed -i 's/ \xa9/ ©/' "$TMP"
      ;;
    *)
      # Fail on unexpected file formats
      echo "unexpected format: $F"
      file "$F"
      exit 1
      ;;
  esac
  if test -z "$format"; then
    # No import format specified
    mv "$TMP" "$F"
  else
    # convert from input format
    iconv -f "$format" -t utf8 < "$TMP" > "$F"
    rm -f "$TMP"
  fi
  # Check that it now is "us-ascii" or "utf-8", otherwise bail out
  case "$(file -bi "$F")" in
    *=us-ascii|*=utf-8) ;;
    *)
      echo "Failed to convert $F"
      file "$F"
      exit 1
      ;;
  esac
done
