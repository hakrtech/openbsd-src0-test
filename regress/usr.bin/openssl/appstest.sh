#!/bin/sh
#
# appstest.sh - test script for openssl command according to man OPENSSL(1)
#
# input  : none
# output : all files generated by this script go under $ssldir
#

openssl_bin=/usr/bin/openssl

uname_s=`uname -s | grep 'MINGW'`
if [ "$uname_s" = "" ] ; then
    mingw=0
else
    mingw=1
fi

function section_message {
    echo ""
    echo "#---------#---------#---------#---------#---------#---------#---------#--------"
    echo "==="
    echo "=== (Section) $1 `date +'%Y/%m/%d %H:%M:%S'`"
    echo "==="
}

function start_message {
    echo ""
    echo "[TEST] $1"
}

function check_exit_status {
    status=$1
    if [ $status -ne 0 ] ; then
        echo ":-< error occurs, exit status = [ $status ]"
        exit $status
    else
        echo ":-) success. "
    fi
}

#---------#---------#---------#---------#---------#---------#---------#---------

#
# create ssldir, and all files generated by this script goes under this dir.
#
ssldir="appstest_dir"

if [ -d $ssldir ] ; then
    echo "directory [ $ssldir ] exists, this script deletes this directory ..."
    /bin/rm -rf $ssldir
fi

mkdir -p $ssldir

export OPENSSL_CONF=$ssldir/openssl.cnf
touch $OPENSSL_CONF

user1_dir=$ssldir/user1
mkdir -p $user1_dir

key_dir=$ssldir/key
mkdir -p $key_dir

#---------#---------#---------#---------#---------#---------#---------#---------

# === COMMAND USAGE ===
section_message "COMMAND USAGE"

start_message "output usages of all commands."

cmds=`$openssl_bin list-standard-commands`
$openssl_bin -help 2>> $user1_dir/usages.out
for c in $cmds ; do
    $openssl_bin $c -help 2>> $user1_dir/usages.out
done 

start_message "check all list-* commands."

lists=""
lists="$lists list-standard-commands"
lists="$lists list-message-digest-commands list-message-digest-algorithms"
lists="$lists list-cipher-commands list-cipher-algorithms"
lists="$lists list-public-key-algorithms"

listsfile=$user1_dir/lists.out

for l in $lists ; do
    echo "" >> $listsfile
    echo "$l" >> $listsfile
    $openssl_bin $l >> $listsfile
done

start_message "check interactive mode"
$openssl_bin <<__EOF__
help
quit
__EOF__
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- listing operations ---
section_message "listing operations"

start_message "ciphers"
$openssl_bin ciphers -V
check_exit_status $?

start_message "errstr"
$openssl_bin errstr 2606A074
check_exit_status $?
$openssl_bin errstr -stats 2606A074 > $user1_dir/errstr-stats.out
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- random number etc. operations ---
section_message "random number etc. operations"

start_message "passwd"

pass="test-pass-1234"

echo $pass | $openssl_bin passwd -stdin -1
check_exit_status $?

echo $pass | $openssl_bin passwd -stdin -apr1
check_exit_status $?

echo $pass | $openssl_bin passwd -stdin -crypt
check_exit_status $?

start_message "prime"

$openssl_bin prime 1
check_exit_status $?

$openssl_bin prime 2
check_exit_status $?

$openssl_bin prime -bits 64 -checks 3 -generate -hex -safe 5
check_exit_status $?

start_message "rand"

$openssl_bin rand -base64 100
check_exit_status $?

$openssl_bin rand -hex 100
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# === MESSAGE DIGEST COMMANDS ===
section_message "MESSAGE DIGEST COMMANDS"

start_message "dgst - See [MESSAGE DIGEST COMMANDS] section."

text="1234567890abcdefghijklmnopqrstuvwxyz"
dgstdat=$user1_dir/dgst.dat
echo $text > $dgstdat
hmac_key="test-hmac-key"
cmac_key="1234567890abcde1234567890abcde12"

digests=`$openssl_bin list-message-digest-commands`

for d in $digests ; do

    echo -n "$d ... "
    $openssl_bin dgst -$d -out $dgstdat.$d $dgstdat
    check_exit_status $?

    echo -n "$d HMAC ... "
    $openssl_bin dgst -$d -hmac $hmac_key -out $dgstdat.$d.hmac $dgstdat
    check_exit_status $?

    echo -n "$d CMAC ... "
    $openssl_bin dgst -$d -mac cmac -macopt cipher:aes-128-cbc -macopt hexkey:$cmac_key \
        -out $dgstdat.$d.cmac $dgstdat
    check_exit_status $?
done

#---------#---------#---------#---------#---------#---------#---------#---------

# === ENCODING AND CIPHER COMMANDS ===
section_message "ENCODING AND CIPHER COMMANDS"

start_message "enc - See [ENCODING AND CIPHER COMMANDS] section."

text="1234567890abcdefghijklmnopqrstuvwxyz"
encfile=$user1_dir/encfile.dat
echo $text > $encfile
pass="test-pass-1234"

ciphers=`$openssl_bin list-cipher-commands`

for c in $ciphers ; do
    echo -n "$c ... encoding ... "
    $openssl_bin enc -$c -e -base64 -pass pass:$pass -in $encfile -out $encfile-$c.enc
    check_exit_status $?

    echo -n "decoding ... "
    $openssl_bin enc -$c -d -base64 -pass pass:$pass -in $encfile-$c.enc -out $encfile-$c.dec
    check_exit_status $?

    echo -n "cmp ... "
    cmp $encfile $encfile-$c.dec
    check_exit_status $?
done

#---------#---------#---------#---------#---------#---------#---------#---------

# === various KEY operations ===
section_message "various KEY operations"

key_pass=test-key-pass

# DH

start_message "gendh - Obsoleted by dhparam."
gendh2=$key_dir/gendh2.pem
$openssl_bin gendh -2 -out $gendh2
check_exit_status $?

start_message "dh - Obsoleted by dhparam."
$openssl_bin dh -in $gendh2 -check -text -out $gendh2.out
check_exit_status $?

start_message "dhparam - Superseded by genpkey and pkeyparam."
dhparam2=$key_dir/dhparam2.pem
$openssl_bin dhparam -2 -out $dhparam2
check_exit_status $?
$openssl_bin dhparam -in $dhparam2 -check -text -out $dhparam2.out
check_exit_status $?

# DSA

start_message "dsaparam - Superseded by genpkey and pkeyparam."
dsaparam512=$key_dir/dsaparam512.pem
$openssl_bin dsaparam -genkey -out $dsaparam512 512
check_exit_status $?

start_message "dsa"
$openssl_bin dsa -in $dsaparam512 -text -out $dsaparam512.out
check_exit_status $?

start_message "gendsa - Superseded by genpkey and pkey."
gendsa_des3=$key_dir/gendsa_des3.pem
$openssl_bin gendsa -des3 -out $gendsa_des3 -passout pass:$key_pass $dsaparam512
check_exit_status $?

# RSA

start_message "genrsa - Superseded by genpkey."
genrsa_aes256=$key_dir/genrsa_aes256.pem
$openssl_bin genrsa -f4 -aes256 -out $genrsa_aes256 -passout pass:$key_pass 2048
check_exit_status $?

start_message "rsa"
$openssl_bin rsa -in $genrsa_aes256 -passin pass:$key_pass -check -text -out $genrsa_aes256.out
check_exit_status $?

start_message "rsautl - Superseded by pkeyutl."
rsautldat=$key_dir/rsautl.dat
rsautlsig=$key_dir/rsautl.sig
echo "abcdefghijklmnopqrstuvwxyz1234567890" > $rsautldat

$openssl_bin rsautl -sign -in $rsautldat -inkey $genrsa_aes256 -passin pass:$key_pass -out $rsautlsig
check_exit_status $?

$openssl_bin rsautl -verify -in $rsautlsig -inkey $genrsa_aes256 -passin pass:$key_pass
check_exit_status $?

# EC

start_message "ecparam -list-curves"
$openssl_bin ecparam -list_curves
check_exit_status $?

# get all EC curves
ec_curves=`$openssl_bin ecparam -list_curves | grep ':' | cut -d ':' -f 1`

start_message "ecparam and ec"

for curve in $ec_curves ;
do
    ecparam=$key_dir/ecparam_$curve.pem

    echo -n "ec - $curve ... ecparam ... "
    $openssl_bin ecparam -out $ecparam -name $curve -genkey -param_enc explicit \
        -conv_form compressed -C
    check_exit_status $?

    echo -n "ec ... "
    $openssl_bin ec -in $ecparam -text -out $ecparam.out 2> /dev/null
    check_exit_status $?
done

# PKEY

start_message "genpkey"

# DH by GENPKEY

genpkey_dh_param=$key_dir/genpkey_dh_param.pem
$openssl_bin genpkey -genparam -algorithm DH -out $genpkey_dh_param \
    -pkeyopt dh_paramgen_prime_len:1024
check_exit_status $?

genpkey_dh=$key_dir/genpkey_dh.pem
$openssl_bin genpkey -paramfile $genpkey_dh_param -out $genpkey_dh
check_exit_status $?

# DSA by GENPKEY

genpkey_dsa_param=$key_dir/genpkey_dsa_param.pem
$openssl_bin genpkey -genparam -algorithm DSA -out $genpkey_dsa_param \
    -pkeyopt dsa_paramgen_bits:1024
check_exit_status $?

genpkey_dsa=$key_dir/genpkey_dsa.pem
$openssl_bin genpkey -paramfile $genpkey_dsa_param -out $genpkey_dsa
check_exit_status $?

# RSA by GENPKEY

genpkey_rsa=$key_dir/genpkey_rsa.pem
$openssl_bin genpkey -algorithm RSA -out $genpkey_rsa \
    -pkeyopt rsa_keygen_bits:2048 -pkeyopt rsa_keygen_pubexp:3
check_exit_status $?

# EC by GENPKEY

genpkey_ec_param=$key_dir/genpkey_ec_param.pem
$openssl_bin genpkey -genparam -algorithm EC -out $genpkey_ec_param \
    -pkeyopt ec_paramgen_curve:secp384r1
check_exit_status $?

genpkey_ec=$key_dir/genpkey_ec.pem
$openssl_bin genpkey -paramfile $genpkey_ec_param -out $genpkey_ec
check_exit_status $?

start_message "pkeyparam"

$openssl_bin pkeyparam -in $genpkey_dh_param -text -out $genpkey_dh_param.out
check_exit_status $?

$openssl_bin pkeyparam -in $genpkey_dsa_param -text -out $genpkey_dsa_param.out
check_exit_status $?

$openssl_bin pkeyparam -in $genpkey_ec_param -text -out $genpkey_ec_param.out
check_exit_status $?

start_message "pkey"

$openssl_bin pkey -in $genpkey_dh -text -out $genpkey_dh.out
check_exit_status $?

$openssl_bin pkey -in $genpkey_dsa -text -out $genpkey_dsa.out
check_exit_status $?

$openssl_bin pkey -in $genpkey_rsa -text -out $genpkey_rsa.out
check_exit_status $?

$openssl_bin pkey -in $genpkey_ec -text -out $genpkey_ec.out
check_exit_status $?

start_message "pkeyutl"

pkeyutldat=$key_dir/pkeyutl.dat
pkeyutlsig=$key_dir/pkeyutl.sig
echo "abcdefghijklmnopqrstuvwxyz1234567890" > $pkeyutldat

$openssl_bin pkeyutl -sign -in  $pkeyutldat -inkey $genpkey_rsa -out $pkeyutlsig
check_exit_status $?

$openssl_bin pkeyutl -verify -in $pkeyutldat -sigfile  $pkeyutlsig -inkey $genpkey_rsa
check_exit_status $?

$openssl_bin pkeyutl -verifyrecover -in $pkeyutlsig -inkey $genpkey_rsa
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

section_message "setup local CA"

#
# prepare test openssl.cnf
#

ca_dir=$ssldir/testCA
tsa_dir=$ssldir/testTSA
ocsp_dir=$ssldir/testOCSP
server_dir=$ssldir/server

cat << __EOF__ > $ssldir/openssl.cnf
oid_section             = new_oids
[ new_oids ]
tsa_policy1 = 1.2.3.4.1
tsa_policy2 = 1.2.3.4.5.6
tsa_policy3 = 1.2.3.4.5.7
[ ca ]
default_ca    = CA_default
[ CA_default ]
dir           = ./$ca_dir
crl_dir       = \$dir/crl
database      = \$dir/index.txt
new_certs_dir = \$dir/newcerts
serial        = \$dir/serial
crlnumber     = \$dir/crlnumber
default_days  = 1
default_md    = default
policy        = policy_match
[ policy_match ]
countryName             = match
stateOrProvinceName     = match
organizationName        = match
organizationalUnitName  = optional
commonName              = supplied
emailAddress            = optional
[ req ]
distinguished_name      = req_distinguished_name 
[ req_distinguished_name ]
countryName                     = Country Name
countryName_default             = JP
countryName_min                 = 2
countryName_max                 = 2
stateOrProvinceName             = State or Province Name
stateOrProvinceName_default     = Tokyo
organizationName                = Organization Name
organizationName_default        = TEST_DUMMY_COMPANY
commonName                      = Common Name
[ tsa ]
default_tsa   = tsa_config1 
[ tsa_config1 ]
dir           = ./$tsa_dir
serial        = \$dir/serial
crypto_device = builtin
digests       = sha1, sha256, sha384, sha512
default_policy = tsa_policy1
other_policies = tsa_policy2, tsa_policy3
[ tsa_ext ]
keyUsage = critical,nonRepudiation
extendedKeyUsage = critical,timeStamping
[ ocsp_ext ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation,digitalSignature,keyEncipherment
extendedKeyUsage = OCSPSigning
__EOF__

#---------#---------#---------#---------#---------#---------#---------#---------

#
# setup test CA
#

mkdir -p $ca_dir
mkdir -p $tsa_dir
mkdir -p $ocsp_dir
mkdir -p $server_dir

mkdir -p $ca_dir/certs
mkdir -p $ca_dir/private
mkdir -p $ca_dir/crl
mkdir -p $ca_dir/newcerts
chmod 700 $ca_dir/private
echo "01" > $ca_dir/serial
touch $ca_dir/index.txt 
touch $ca_dir/crlnumber
echo "01" > $ca_dir/crlnumber

# 
# setup test TSA 
#
mkdir -p $tsa_dir/private
chmod 700 $tsa_dir/private
echo "01" > $tsa_dir/serial
touch $tsa_dir/index.txt 

# 
# setup test OCSP 
#
mkdir -p $ocsp_dir/private
chmod 700 $ocsp_dir/private

#---------#---------#---------#---------#---------#---------#---------#--------- 

# --- CA initiate (generate CA key and cert) --- 

start_message "req ... generate CA key and self signed cert"

ca_cert=$ca_dir/ca_cert.pem 
ca_key=$ca_dir/private/ca_key.pem ca_pass=test-ca-pass 

if [ $mingw = 0 ] ; then
    subj='/C=JP/ST=Tokyo/O=TEST_DUMMY_COMPANY/CN=testCA.test_dummy.com/'
else
    subj='//C=JP\ST=Tokyo\O=TEST_DUMMY_COMPANY\CN=testTSA.test_dummy.com\'
fi

$openssl_bin req -new -x509 -newkey rsa:2048 -out $ca_cert -keyout $ca_key \
    -days 1 -passout pass:$ca_pass -batch -subj $subj
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- TSA initiate (generate TSA key and cert) ---

start_message "req ... generate TSA key and cert"

# generate CSR for TSA

tsa_csr=$tsa_dir/tsa_csr.pem
tsa_key=$tsa_dir/private/tsa_key.pem
tsa_pass=test-tsa-pass

if [ $mingw = 0 ] ; then
    subj='/C=JP/ST=Tokyo/O=TEST_DUMMY_COMPANY/CN=testTSA.test_dummy.com/'
else
    subj='//C=JP\ST=Tokyo\O=TEST_DUMMY_COMPANY\CN=testTSA.test_dummy.com\'
fi

$openssl_bin req -new -keyout $tsa_key -out $tsa_csr -passout pass:$tsa_pass -subj $subj
check_exit_status $?

start_message "ca ... sign by CA with TSA extensions"

tsa_cert=$tsa_dir/tsa_cert.pem

$openssl_bin ca -batch -cert $ca_cert -keyfile $ca_key -key $ca_pass \
-in $tsa_csr -out $tsa_cert -extensions tsa_ext
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- OCSP initiate (generate OCSP key and cert) ---

start_message "req ... generate OCSP key and cert"

# generate CSR for OCSP 

ocsp_csr=$ocsp_dir/ocsp_csr.pem
ocsp_key=$ocsp_dir/private/ocsp_key.pem

if [ $mingw = 0 ] ; then
    subj='/C=JP/ST=Tokyo/O=TEST_DUMMY_COMPANY/CN=testOCSP.test_dummy.com/'
else
    subj='//C=JP\ST=Tokyo\O=TEST_DUMMY_COMPANY\CN=testOCSP.test_dummy.com\'
fi

$openssl_bin req -new -keyout $ocsp_key -nodes -out $ocsp_csr -subj $subj
check_exit_status $?

start_message "ca ... sign by CA with OCSP extensions"

ocsp_cert=$ocsp_dir/ocsp_cert.pem

$openssl_bin ca -batch -cert $ca_cert -keyfile $ca_key -key $ca_pass \
-in $ocsp_csr -out $ocsp_cert -extensions ocsp_ext
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- server-admin operations (generate server key and csr) ---
section_message "server-admin operations (generate server key and csr)"

start_message "req ... generate server csr#1"

server_key=$server_dir/server_key.pem
server_csr=$server_dir/server_csr.pem
server_pass=test-server-pass

if [ $mingw = 0 ] ; then
    subj='/C=JP/ST=Tokyo/O=TEST_DUMMY_COMPANY/CN=localhost.test_dummy.com/'
else
    subj='//C=JP\ST=Tokyo\O=TEST_DUMMY_COMPANY\CN=localhost.test_dummy.com\'
fi

$openssl_bin req -new -keyout $server_key -out $server_csr -passout pass:$server_pass -subj $subj
check_exit_status $?

start_message "req ... generate server csr#2 (interactive mode)"

revoke_key=$server_dir/revoke_key.pem
revoke_csr=$server_dir/revoke_csr.pem
revoke_pass=test-revoke-pass

$openssl_bin req -new -keyout $revoke_key -out $revoke_csr -passout pass:$revoke_pass <<__EOF__
JP
Tokyo
TEST_DUMMY_COMPANY
revoke.test_dummy.com
__EOF__
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- CA operations (issue cert for server) ---
section_message "CA operations (issue cert for server)"

start_message "ca ... issue cert for server csr#1"

server_cert=$server_dir/server_cert.pem
$openssl_bin ca -batch -cert $ca_cert -keyfile $ca_key -key $ca_pass \
    -in $server_csr -out $server_cert
check_exit_status $?

start_message "x509 ... issue cert for server csr#2"

revoke_cert=$server_dir/revoke_cert.pem
$openssl_bin x509 -req -in $revoke_csr -CA $ca_cert -CAkey $ca_key -passin pass:$ca_pass \
    -CAcreateserial -out $revoke_cert
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- CA operations (revoke cert and generate crl) ---
section_message "CA operations (revoke cert and generate crl)"

start_message "ca ... revoke server cert#2"
crl_file=$ca_dir/crl.pem
$openssl_bin ca -gencrl -out $crl_file -crldays 30 -revoke $revoke_cert \
    -keyfile $ca_key -passin pass:$ca_pass -cert $ca_cert
check_exit_status $?

start_message "crl ... CA generates CRL"
$openssl_bin crl -in $crl_file -fingerprint
check_exit_status $?

crl_p7=$ca_dir/crl.p7
start_message "crl2pkcs7 ... convert CRL to pkcs7"
$openssl_bin crl2pkcs7 -in $crl_file -certfile $ca_cert -out $crl_p7
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- server-admin operations (check csr, verify cert, certhash) ---
section_message "server-admin operations (check csr, verify cert, certhash)"

start_message "asn1parse ... parse server csr#1"
$openssl_bin asn1parse -in $server_csr -i \
    -dlimit 100 -length 1000 -strparse 01 > $server_csr.asn1parse.out
check_exit_status $?

start_message "verify ... server cert#1"
$openssl_bin verify -verbose -CAfile $ca_cert $server_cert
check_exit_status $?

start_message "x509 ... get detail info about server cert#1"
$openssl_bin x509 -in $server_cert -text -C -dates -startdate -enddate \
    -fingerprint -issuer -issuer_hash -issuer_hash_old \
    -subject -subject_hash -subject_hash_old -ocsp_uri -ocspid -modulus \
    -pubkey -serial -email > $server_cert.x509.out
check_exit_status $?

if [ $mingw = 0 ] ; then
    start_message "certhash"
    $openssl_bin certhash -v $server_dir
    check_exit_status $?
fi

# self signed
start_message "x509 ... generate self signed server cert"
server_self_cert=$server_dir/server_self_cert.pem
$openssl_bin x509 -in $server_cert -signkey $server_key -passin pass:$server_pass -out $server_self_cert
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- Netscape SPKAC operations ---
section_message "Netscape SPKAC operations"

# server-admin generates SPKAC

start_message "spkac"
spkacfile=$server_dir/spkac.file

$openssl_bin spkac -key $genpkey_rsa -challenge hello -out $spkacfile
check_exit_status $?

$openssl_bin spkac -in $spkacfile -verify -out $spkacfile.out
check_exit_status $?

spkacreq=$server_dir/spkac.req
cat << __EOF__ > $spkacreq
countryName = JP
stateOrProvinceName = Tokyo
organizationName = TEST_DUMMY_COMPANY
commonName = spkac.test_dummy.com
__EOF__
cat $spkacfile >> $spkacreq

# CA signs SPKAC
start_message "ca ... CA signs SPKAC csr"
spkaccert=$server_dir/spkac.cert
$openssl_bin ca -batch -cert $ca_cert -keyfile $ca_key -key $ca_pass \
    -spkac $spkacreq -out $spkaccert
check_exit_status $?

start_message "x509 ... convert DER format SPKAC cert to PEM"
spkacpem=$server_dir/spkac.pem
$openssl_bin x509 -in $spkaccert -inform DER -out $spkacpem -outform PEM
check_exit_status $?

# server-admin cert verify

start_message "nseq"
$openssl_bin nseq -in $spkacpem -toseq -out $spkacpem.nseq
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- user1 operations (generate user1 key and csr) ---
section_message "user1 operations (generate user1 key and csr)"

# trust
start_message "x509 ... trust testCA cert"
user1_trust=$user1_dir/user1_trust_ca.pem
$openssl_bin x509 -in $ca_cert -addtrust clientAuth -setalias "trusted testCA" -purpose -out $user1_trust
check_exit_status $?

start_message "req ... generate private key and csr for user1"

user1_key=$user1_dir/user1_key.pem
user1_csr=$user1_dir/user1_csr.pem
user1_pass=test-user1-pass

if [ $mingw = 0 ] ; then
    subj='/C=JP/ST=Tokyo/O=TEST_DUMMY_COMPANY/CN=user1.test_dummy.com/'
else
    subj='//C=JP\ST=Tokyo\O=TEST_DUMMY_COMPANY\CN=user1.test_dummy.com\'
fi

$openssl_bin req -new -keyout $user1_key -out $user1_csr -passout pass:$user1_pass -subj $subj
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- CA operations (issue cert for user1) ---
section_message "CA operations (issue cert for user1)"

start_message "ca ... issue cert for user1"

user1_cert=$user1_dir/user1_cert.pem
$openssl_bin ca -batch -cert $ca_cert -keyfile $ca_key -key $ca_pass \
    -in $user1_csr -out $user1_cert
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- TSA operations ---
section_message "TSA operations"

tsa_dat=$user1_dir/tsa.dat
cat << __EOF__ > $tsa_dat
Hello Bob,
Sincerely yours
Alice
__EOF__

# Query
start_message "ts ... create time stamp request"

tsa_tsq=$user1_dir/tsa.tsq

$openssl_bin ts -query -sha1 -data $tsa_dat -no_nonce -out $tsa_tsq
check_exit_status $?

start_message "ts ... print time stamp request"

$openssl_bin ts -query -in $tsa_tsq -text
check_exit_status $?

# Reply
start_message "ts ... create time stamp response for a request"

tsa_tsr=$user1_dir/tsa.tsr

$openssl_bin ts -reply -queryfile $tsa_tsq -inkey $tsa_key -passin pass:$tsa_pass \
    -signer $tsa_cert -chain $ca_cert -out $tsa_tsr
check_exit_status $?

# Verify
start_message "ts ... verify time stamp response"

$openssl_bin ts -verify -queryfile $tsa_tsq -in $tsa_tsr -CAfile $ca_cert -untrusted $tsa_cert
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- S/MIME operations ---
section_message "S/MIME operations"

smime_txt=$user1_dir/smime.txt
smime_msg=$user1_dir/smime.msg
smime_ver=$user1_dir/smime.ver

cat << __EOF__ > $smime_txt
Hello Bob,
Sincerely yours
Alice
__EOF__

# sign
start_message "smime ... sign to message"

$openssl_bin smime -sign -in $smime_txt -text -out $smime_msg \
    -signer $user1_cert -inkey $user1_key -passin pass:$user1_pass
check_exit_status $?

# verify
start_message "smime ... verify message"

$openssl_bin smime -verify -in $smime_msg -signer $user1_cert -CAfile $ca_cert -out $smime_ver
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- OCSP operations ---
section_message "OCSP operations"

# request
start_message "ocsp ... create OCSP request"

ocsp_req=$user1_dir/ocsp_req.der
$openssl_bin ocsp -issuer $ca_cert -cert $server_cert -cert $revoke_cert \
    -CAfile $ca_cert -reqout $ocsp_req
check_exit_status $?

# response
start_message "ocsp ... create OCPS response for a request"

ocsp_res=$user1_dir/ocsp_res.der
$openssl_bin ocsp -index  $ca_dir/index.txt -CA $ca_cert -CAfile $ca_cert \
    -rsigner $ocsp_cert -rkey $ocsp_key -reqin $ocsp_req -respout $ocsp_res -text > $ocsp_res.out 2>&1
check_exit_status $?

# ocsp server
start_message "ocsp ... start OCSP server in background"

ocsp_port=8888

$openssl_bin ocsp -index  $ca_dir/index.txt -CA $ca_cert -CAfile $ca_cert \
    -rsigner $ocsp_cert -rkey $ocsp_key -port '*:'$ocsp_port -nrequest 1 &
check_exit_status $?
ocsp_svr_pid=$!
echo "ocsp server pid = [ $ocsp_svr_pid ]"
sleep 1

# send query to oscp server
start_message "ocsp ... send OCSP request to server"

ocsp_qry=$user1_dir/ocsp_qry.der
$openssl_bin ocsp -issuer $ca_cert -cert $server_cert -cert $revoke_cert \
    -CAfile $ca_cert -url http://localhost:$ocsp_port -resp_text -respout $ocsp_qry > $ocsp_qry.out 2>&1
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- PKCS operations ---
section_message "PKCS operations"

pkcs_pass=test-pkcs-pass

start_message "pkcs7 ... output certs in crl(pkcs7)"
$openssl_bin pkcs7 -in $crl_p7 -print_certs -text -out $crl_p7.out
check_exit_status $?

start_message "pkcs8 ... convert key to pkcs8"
$openssl_bin pkcs8 -in $user1_key -topk8 -out $user1_key.p8 \
    -passin pass:$user1_pass -passout pass:$user1_pass -v1 pbeWithSHA1AndDES-CBC -v2 des3
check_exit_status $?

start_message "pkcs8 ... convert pkcs8 to key in DER format"
$openssl_bin pkcs8 -in $user1_key.p8 -passin pass:$user1_pass -outform DER -out $user1_key.p8.der
check_exit_status $?

start_message "pkcs12 ... create"
$openssl_bin pkcs12 -export -in $server_cert -inkey $server_key -passin pass:$server_pass \
    -certfile $ca_cert -CAfile $ca_cert -caname "server_p12" -passout pass:$pkcs_pass \
    -certpbe AES-256-CBC -keypbe AES-256-CBC -chain -out $server_cert.p12
check_exit_status $?

start_message "pkcs12 ... verify"
$openssl_bin pkcs12 -in $server_cert.p12 -passin pass:$pkcs_pass -info -noout
check_exit_status $?

start_message "pkcs12 ... to PEM"
$openssl_bin pkcs12 -in $server_cert.p12 -passin pass:$pkcs_pass \
    -passout pass:$pkcs_pass -out $server_cert.p12.pem
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- client/server operations ---
section_message "client/server operations"

host="localhost"
port=4433
sess_log=$user1_dir/s_client_sess.log
s_client_out=$user1_dir/s_client.out

start_message "s_server ... start SSL/TLS test server"
$openssl_bin s_server -accept $port -CAfile $ca_cert \
    -cert $server_cert -key $server_key -pass pass:$server_pass \
    -context "appstest.sh" -id_prefix "APPSTEST.SH" \
    -crl_check -no_ssl2 -no_ssl3 -no_tls1 \
    -nextprotoneg "http/1.1,spdy/3" -alpn "http/1.1,spdy/3" \
    -www -quiet &
check_exit_status $?
s_server_pid=$!
echo "s_server pid = [ $s_server_pid ]"
sleep 1

start_message "s_client ... connect to SSL/TLS test server"
$openssl_bin s_client -connect $host:$port -CAfile $ca_cert \
    -showcerts -crl_check -issuer_checks -policy_check -pause -prexit \
    -nextprotoneg "spdy/3,http/1.1" -alpn "spdy/3,http/1.1" \
    -sess_out $sess_log < /dev/null > $s_client_out 2>&1
check_exit_status $?

start_message "s_time ... connect to SSL/TLS test server"
$openssl_bin s_time -connect $host:$port -CAfile $ca_cert -time 2
check_exit_status $?

start_message "sess_id"
$openssl_bin sess_id -in $sess_log -text -out $sess_log.out
check_exit_status $?

sleep 1
kill -TERM $s_server_pid
wait $s_server_pid

#---------#---------#---------#---------#---------#---------#---------#---------

# === PERFORMANCE ===
section_message "PERFORMANCE"

start_message "speed"
$openssl_bin speed sha512 rsa2048 -multi 2 -elapsed
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

# --- VERSION INFORMATION ---
section_message "VERSION INFORMATION"

start_message "version"
$openssl_bin version -a
check_exit_status $?

#---------#---------#---------#---------#---------#---------#---------#---------

section_message "END"

exit 0

