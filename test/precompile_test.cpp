/*
   Copyright 2022 The Silkpre Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <catch2/catch.hpp>

#include <silkpre/precompile.h>

#include "hex.hpp"

enum {
    EVMC_BYZANTIUM = 4,
    EVMC_BERLIN = 8,
};

TEST_CASE("Ecrecover") {
    std::basic_string<uint8_t> in{
        from_hex("18c547e4f7b0f325ad1e56f57e26c745b09a3e503d86e00e5255ff7f715d3d1c0000000000000000000000000000"
                 "00000000000000000000000000000000001c73b1693892219d736caba55bdb67216e485557ea6b6af75f37096c9a"
                 "a6a5a75feeb940b1d03b21e36b0e47e79769f095fe2ab855bd91e3a38756b7d75a9c4549")};
    SilkpreOutput out{silkpre_ecrec_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "000000000000000000000000a94f5374fce5edbc8e2a8697c15331677e6ebf0b");
    std::free(out.data);

    // Unrecoverable key
    in = from_hex(
        "a8b53bdf3306a35a7103ab5504a0c9b492295564b6202b1942a84ef3001072810000000000000000000000000000"
        "00000000000000000000000000000000001b30783565316530336635336365313862373732636362303039336666"
        "37316633663533663563373562373464636233316138356161386238383932623465386211223344556677889910"
        "11121314151617181920212223242526272829303132");
    out = silkpre_ecrec_run(in.data(), in.length());
    CHECK((out.data != nullptr && out.size == 0));
    std::free(out.data);
}

TEST_CASE("SHA256") {
    std::basic_string<uint8_t> in{
        from_hex("38d18acb67d25c8bb9942764b62f18e17054f66a817bd4295423adf9ed98873e0000000000000000000000000000"
                 "00000000000000000000000000000000001b38d18acb67d25c8bb9942764b62f18e17054f66a817bd4295423adf9"
                 "ed98873e789d1dd423d25f0772d2748d60f7e4b81bb14d086eba8e8e8efb6dcff8a4ae02")};
    SilkpreOutput out{silkpre_sha256_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "811c7003375852fabd0d362e40e68607a12bdabae61a7d068fe5fdd1dbbf2a5d");
    std::free(out.data);
}

TEST_CASE("RIPEMD160") {
    std::basic_string<uint8_t> in{
        from_hex("38d18acb67d25c8bb9942764b62f18e17054f66a817bd4295423adf9ed98873e0000000000000000000000000000"
                 "00000000000000000000000000000000001b38d18acb67d25c8bb9942764b62f18e17054f66a817bd4295423adf9"
                 "ed98873e789d1dd423d25f0772d2748d60f7e4b81bb14d086eba8e8e8efb6dcff8a4ae02")};
    SilkpreOutput out{silkpre_rip160_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000009215b8d9882ff46f0dfde6684d78e831467f65e6");
    std::free(out.data);
}

TEST_CASE("EXPMOD") {
    std::basic_string<uint8_t> in{
        from_hex("0000000000000000000000000000000000000000000000000000000000000001"
                 "0000000000000000000000000000000000000000000000000000000000000020"
                 "0000000000000000000000000000000000000000000000000000000000000020"
                 "03"
                 "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e"
                 "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f")};
    CHECK(silkpre_expmod_gas(in.data(), in.length(), EVMC_BYZANTIUM) == 13056);

    SilkpreOutput out{silkpre_expmod_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000000000000000000000000000000000000000000001");
    std::free(out.data);

    in = from_hex(
        "0000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000020"
        "0000000000000000000000000000000000000000000000000000000000000020"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");

    out = silkpre_expmod_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000000000000000000000000000000000000000000000");
    std::free(out.data);

    in = from_hex(
        "0000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000020"
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd");
    CHECK(silkpre_expmod_gas(in.data(), in.length(), EVMC_BYZANTIUM) == UINT64_MAX);
    CHECK(silkpre_expmod_gas(in.data(), in.length(), EVMC_BERLIN) == UINT64_MAX);

    in = from_hex(
        "0000000000000000000000000000000000000000000000000000000000000100"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000100"
        "94fff7dfe2f9c757463dab3aaa4103e9b820bed33aaa0f2b6c0ec056d338288dcd7c568aeb0a1c7bfdde436f4c69"
        "f242f79661df1d8c5b65836a41070f0b562002c67c5e6037b1e4d9e7c9e4e5faf6c9d3b46ed618b75dbf01c8f519"
        "ebd5afde96cf446a1cbd6fa58077592d22bdb661c16ebd9a207571f331d8e45eb0e3f58731eda925429d4e10d823"
        "fed0a6819ce94f68791bc90222b2f767e884858b5d054ac6fbfb0ec6dbdc88371bed2a85e13c2fd3f85963b7e8d0"
        "06373f9a7dd295ce1e87fdb28e3a9e1a3851169e24042bb401b872a0bdd55e8b36a01efed0d65fc3adf94dbf5eb3"
        "7365afa8add999aa5fcb772439f607c6127c32c7fe920efd7b74"
        "010001"
        "aa05b012cda6a5d91d80dc970a252e4b70aff168381da61bd7c655db438afe1322cc387442a8a801f974dbf4ffb1"
        "10e5b68c03202ca47470bda7cff40c50c2762a0e45222a4df1e6c6d69a1dccafd1535a1bb82d6c17dd2ac04b8d02"
        "6092d4189ab630d1348baac2ff5612faf07961f48482571f59e922c744dab8b9c7acf6295fcc72566626c6423776"
        "1c9d571616e1cbeef439413f348f9c6e89226a971b393fc8d45472951d68897eaf264acdbb5cd54b6c4ea520b45c"
        "3abbbd78fa27dd113921d3facbcc1d6040243c9761867c69a1dc13d9f71898121ff696561458d9d9f87536d6a84f"
        "b602c91f9b07e561fa2f54eb0f9f1984f3cbe728ec142cbed52f");
    CHECK(silkpre_expmod_gas(in.data(), in.length(), EVMC_BYZANTIUM) == 30310);
    CHECK(silkpre_expmod_gas(in.data(), in.length(), EVMC_BERLIN) == 5461);
}

TEST_CASE("BN_ADD") {
    std::basic_string<uint8_t> in{
        from_hex("00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000"
                 "00000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000"
                 "000000010000000000000000000000000000000000000000000000000000000000000002")};
    SilkpreOutput out{silkpre_bn_add_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd315ed738c0e0a7c92e7845f96b2"
          "ae9c0a68a6a449e3538fc7ff3ebf7a5a18a2c4");
    std::free(out.data);
}

TEST_CASE("BN_MUL") {
    std::basic_string<uint8_t> in{
        from_hex("1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee"
                 "9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000000000"
                 "00000000000000000000000009")};
    SilkpreOutput out{silkpre_bn_mul_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "1dbad7d39dbc56379f78fac1bca147dc8e66de1b9d183c7b167351bfe0aeab742cd757d51289cd8dbd0acf9e67"
          "3ad67d0f0a89f912af47ed1be53664f5692575");
    std::free(out.data);
}

TEST_CASE("SNARKV") {
    // empty input
    std::basic_string<uint8_t> in{};
    SilkpreOutput out{silkpre_snarkv_run(in.data(), in.length())};
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000000000000000000000000000000000000000000001");
    std::free(out.data);

    // input size is not a multiple of 192
    in = from_hex("ab");
    out = silkpre_snarkv_run(in.data(), in.length());
    CHECK(!out.data);

    in = from_hex(
        "0f25929bcb43d5a57391564615c9e70a992b10eafa4db109709649cf48c50dd216da2f5cb6be7a0aa72c440c53c9"
        "bbdfec6c36c7d515536431b3a865468acbba2e89718ad33c8bed92e210e81d1853435399a271913a6520736a4729"
        "cf0d51eb01a9e2ffa2e92599b68e44de5bcf354fa2642bd4f26b259daa6f7ce3ed57aeb314a9a87b789a58af499b"
        "314e13c3d65bede56c07ea2d418d6874857b70763713178fb49a2d6cd347dc58973ff49613a20757d0fcc22079f9"
        "abd10c3baee245901b9e027bd5cfc2cb5db82d4dc9677ac795ec500ecd47deee3b5da006d6d049b811d7511c7815"
        "8de484232fc68daf8a45cf217d1c2fae693ff5871e8752d73b21198e9393920d483a7260bfb731fb5d25f1aa4933"
        "35a9e71297e485b7aef312c21800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed0906"
        "89d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b12c85ea5db8c6deb4aab71808dcb408f"
        "e3d1e7690c43d37b4ce6cc0166fa7daa");
    out = silkpre_snarkv_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000000000000000000000000000000000000000000001");
    std::free(out.data);

    in = from_hex(
        "00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000"
        "000000000000000000000000000000000002198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7"
        "aef312c21800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed090689d0585ff075ec9e"
        "99ad690c3395bc4b313370b38ef355acdadcd122975b12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b"
        "4ce6cc0166fa7daa");
    out = silkpre_snarkv_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) == "0000000000000000000000000000000000000000000000000000000000000000");
    std::free(out.data);
}

// https://eips.ethereum.org/EIPS/eip-152#test-cases
TEST_CASE("BLAKE2") {
    std::basic_string<uint8_t> in{
        from_hex("00000c48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c3e"
                 "2b8c68059b6bbd41fbabd9831f79217e1319cde05b61626300000000000000000000000000000000000000000000"
                 "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                 "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                 "00000000000000000000000300000000000000000000000000000001")};
    SilkpreOutput out{silkpre_blake2_f_run(in.data(), in.length())};
    CHECK(!out.data);

    in = from_hex(
        "000000000c48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f"
        "6c3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b6162630000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000300000000000000000000000000000001");
    out = silkpre_blake2_f_run(in.data(), in.length());
    CHECK(!out.data);

    in = from_hex(
        "0000000c48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c"
        "3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b616263000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000300000000000000000000000000000002");
    out = silkpre_blake2_f_run(in.data(), in.length());
    CHECK(!out.data);

    in = from_hex(
        "0000000048c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c"
        "3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b616263000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000300000000000000000000000000000001");
    out = silkpre_blake2_f_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "08c9bcf367e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d282e6ad7f520e511f6c3e2b8c"
          "68059b9442be0454267ce079217e1319cde05b");
    std::free(out.data);

    in = from_hex(
        "0000000c48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c"
        "3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b616263000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000300000000000000000000000000000001");
    out = silkpre_blake2_f_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de45"
          "33cc9518d38aa8dbf1925ab92386edd4009923");
    std::free(out.data);

    in = from_hex(
        "0000000c48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c"
        "3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b616263000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000300000000000000000000000000000000");
    out = silkpre_blake2_f_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "75ab69d3190a562c51aef8d88f1c2775876944407270c42c9844252c26d2875298743e7f6d5ea2f2d3e8d22603"
          "9cd31b4e426ac4f2d3d666a610c2116fde4735");
    std::free(out.data);

    in = from_hex(
        "0000000148c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c"
        "3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b616263000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000300000000000000000000000000000001");
    out = silkpre_blake2_f_run(in.data(), in.length());
    REQUIRE(out.data);
    CHECK(to_hex(out.data, out.size) ==
          "b63a380cb2897d521994a85234ee2c181b5f844d2c624c002677e9703449d2fba551b3a8333bcdf5f2f7e08993"
          "d53923de3d64fcc68c034e717b9293fed7a421");
    std::free(out.data);
}
