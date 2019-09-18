#!/usr/bin/env bats

function setup {
    TESTCASE_TMPDIR="$(mktemp -d "${BATS_TMPDIR}/${BATS_TEST_NAME}.XXXXXX")"
}

function teardown {
    rm -rf "${TESTCASE_TMPDIR}"
}

@test "environment: JFROG_APIKEY is set" {
    [[ -n "${JFROG_APIKEY}" ]]
}

@test "environment: curl is installed" {
    run command -v curl

    [[ "${status}" == "0" ]]
}

@test "environment: gzip is installed" {
    run command -v gzip

    [[ "${status}" == "0" ]]
}

@test "artifactory.sh: illegal command line parameter => return error" {
    run ./artifactory.sh \
        unknown_parameter

    [[ "$status" != "0" ]]
    [[ "${output}" =~ "Unknown parameter: unknown_parameter" ]]
}


#@test "fetch_md5sum: valid URL, no output file => print md5sum to stdout" {
#
#    run ./artifactory.sh \
#        fetch_md5sum \
#        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz"
#
#    echo "${output}"
#
#    [[ "$status" == "0" ]]
#
#    #[[ "${lines[0]}" == $'6f06c42553730ab2f91c3c91ee9e00d2\r' ]]
#    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]
#}

@test "fetch_md5sum: valid url => checksum file is created" {

    local file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_md5sum \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]
    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]

}

@test "fetch_md5sum: invalid url => return error" {

    run ./artifactory.sh \
        fetch_md5sum \
        "invalid_url"

    echo "${output}"

    [[ "$status" != "0" ]]

}

@test "fetch_md5sum: invalid url => no checksum file is created" {

    local file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_md5sum \
        "illegal_url" \
        "${file}"

    echo "${output}"

    [[ "$status" != "0" ]]

   run cat "${file}"

   echo "${output}"

   [[ "$status" != "0" ]]
   [[ "${output}" =~ 'No such file or directory' ]]
}

@test "fetch_source: valid url => download and store a valid file" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_source \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    [[ -f "${file}" ]]

    run gzip -t "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

}

@test "fetch_source: invalid url => do not create anything" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_source \
        "invalid_url" \
        "${file}"

    echo "${output}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
}

@test "fetch: not enough parameters (0) => abort" {

    run ./artifactory.sh \
        fetch

    [[ "$status" != "0" ]]
}


@test "fetch: not enough parameters (1) => abort" {

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz"

    [[ "$status" != "0" ]]
}

@test "fetch: not enough parameters (2) => abort" {

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \

    [[ "$status" != "0" ]]
}

@test "fetch: source file and md5 exist => do nothing, return true" {

    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${file}"
    touch "${md5sum}"

    local file_metadata=

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${file} and ${md5sum} exist\..*") -ne 0 ]]
}

# Required for the BSP workflow to not publish proprietary sources by accident
@test "fetch: source file path does not exist => do nothing, return error" {

    local file="${TESTCASE_TMPDIR}/wago_intern/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${md5sum}" ]]
}


@test "fetch: only source file exists => remove existing file and download everything" {

    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${file}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${md5sum} missing, overwriting ${file}\..*") -ne 0 ]]

    run gzip -t "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${md5sum}"
    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]
}

@test "fetch: only md5 file exists => remove existing file and download everything" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${md5sum}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${file} missing, overwriting ${md5sum}\..*") -ne 0 ]]
 
    run gzip -t "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${md5sum}"
    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]
}

@test "fetch: valid url => download file and checksum" {

    local outputfile="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1-93177_64.tgz" \
        "${outputfile}" \
        "${md5file}"

    echo "fetch: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${outputfile}" ]]

    run gzip -t "${outputfile}"

    echo "gzip: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${md5file}" ]]

    run cat "${md5file}"

    echo "cat: ${output}"

    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]
}

@test "fetch: valid snapshot url => download file and checksum" {

    local outputfile="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory-test/playground-generic-dev-local/wago/rlb-stack/rlb-stack-Source/rlb-stack-Source-1\[INTEGRATION\].tgz" \
        "${outputfile}" \
        "${md5file}"

    echo "fetch: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${outputfile}" ]]

    run gzip -t "${outputfile}"

    echo "gzip: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${md5file}" ]]
}


@test "fetch: invalid url => create nothing, return error" {

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${checksum}" ]]
    
    [[ -n "${output}" ]]
}

@test "fetch: JFROG_APIKEY empty => return error and output message" {

    export JFROG_APIKEY=

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    echo "${output}"

    [[ "${output}" =~ 'Please make sure the environment variable JFROG_APIKEY is set' ]]
}

@test "fetch: JFROG_APIKEY not set => return error and output message" {

    unset JFROG_APIKEY

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    echo "${output}"

    [[ "${output}" =~ 'Please make sure the environment variable JFROG_APIKEY is set' ]]
}

@test "fetch: empty url => create nothing, return error" {

    run ./artifactory.sh \
        fetch \
        "" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${checksum}" ]]
    
    [[ -n "${output}" ]]
}
