#!/bin/bash

export PYTHONPATH=../../../platform-IO-IPC-P-Codesys/sysroot-host/lib/python2.6/lib-dynload/ 
TMP_FILE=./tmp_out
V=0

separator='***********************************************************************'

function echo_spaces(){
  local cnt=0
  while [[ $cnt -lt ${1} ]]; do
    echo -n ' '
    cnt=$((cnt+1))
  done
}

function echo_title(){
  local title_length=$(expr length "$1")
  local line_length=$(expr length "${separator}")

  local spaces_nr=$((${line_length}-${title_length}-2))
  local spaces_left=$((${spaces_nr}/2))
  local spaces_right=$((${spaces_nr}-${spaces_left}))

  echo -n '*'
  echo_spaces $spaces_left
  echo -n $1
  echo_spaces $spaces_right
  echo '*'

}

function run_test(){
  echo "${separator}"
  echo_title "Running test ${1}"
  echo "${separator}"
  ./make-xml-config --configfile test_ptxconfig_${1} --menufile test_menu_${1}.in &> ${TMP_FILE}
  changes=$(diff expected_${1} ${TMP_FILE})
  if [[ $? == 2 ]]; then
    echo "Unexpected error";
    exit 42;
  fi

  echo
  if [[ ${changes} == '' ]]; then
    echo -e "\033[36mPASSED\033[0m"
    echo
    if [[ ${V} == 1 ]]; then echo "Returned:"; cat ${TMP_FILE}; fi
  else
    echo -e "\033[31mFAILED\033[0m"
    echo "Expected:"
    cat expected_${1}
    echo
    echo "Returned:"
    cat ${TMP_FILE}
  fi
  echo
}

###############################################################################

#
# Prepare test-bed: get make-xml-config and python-c-glue.py.
#                   use local xsd_defs.py => it shall have no interconnections
#                   with the other files. We freeze its version because it 
#                   has to match to the .in files.
#

ln -s ../make-xml-config
ln -s ../python_c_glue.py

# Test 1:
#
# general test with a small menu structure
#
run_test simple_general

# Test 2:
#
# general test with a slightly bigger menu structure
#
run_test simple_general2

# Test 3:
#
# error condition: illegal head attribute
#
# __XML: illegal foo = x bar = y
# 
run_test illegal_first_attr

# Test 4:
#
# attributeless element
#
# __XML: attributeless_element [ignored stuff]
# 
run_test empty_elem

# Test 5:
#
# error condition: assignment without rvalue
#
# __XML: element foo=
# 
run_test assign_error

# Test 6:
#
# element without an assignment
#
# __XML: element foo
# 
# Is silently ignored; this error should be caught
# when validating the XML file against the XSD schema
# (if marked as required).
run_test no_assign_elem

# Test 7:
#
# error condition: .baz without a previous foo.bar
#
# __XML: element .bar
run_test orphaned_attr

# Test 8:
#
# error condition: illegal bar in foo.bar
#
# __XML: element foo.illegal_bar
run_test illegal_secondary_attr

# Test 9:
#
# error condition: illegal bar in foo.bar.baz
#
# __XML: element foo.illegal_bar.baz
run_test illegal_middle_attr

#TODO: error calling export

# remove the links
rm -f ./make-xml-config
rm -f ./python_c_glue.py

rm -f ${TMP_FILE}
