#include "import/ThreeMfLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

using namespace chisel::io;
using Catch::Approx;

#ifndef CHISELCAD_TEST_FIXTURE_DIR
#error "CHISELCAD_TEST_FIXTURE_DIR must be defined by the build (see CMakeLists.txt)"
#endif

static std::filesystem::path fixture(const std::string& relPath) {
    return std::filesystem::path(CHISELCAD_TEST_FIXTURE_DIR) / relPath;
}

TEST_CASE("ThreeMfLoader:loads a DEFLATE-compressed tetrahedron", "[3mf-loader][tier-e]") {
    auto mesh = loadThreeMfMesh(fixture("import/tetra.3mf"));
    REQUIRE(mesh.error.empty());
    REQUIRE(mesh.positions.size() == 4);
    REQUIRE(mesh.indices.size() == 12); // 4 triangles * 3
    REQUIRE(mesh.positions[1].x == Approx(1.0));
    REQUIRE(mesh.positions[3].z == Approx(1.0));
}

TEST_CASE("ThreeMfLoader:loads a STORED (uncompressed) archive identically",
          "[3mf-loader][tier-e]") {
    auto mesh = loadThreeMfMesh(fixture("import/tetra_stored.3mf"));
    REQUIRE(mesh.error.empty());
    REQUIRE(mesh.positions.size() == 4);
    REQUIRE(mesh.indices.size() == 12);
}

TEST_CASE("ThreeMfLoader:an out-of-range triangle index is skipped, not used as-is",
          "[3mf-loader][tier-e]") {
    // oob_triangle.3mf has 3 vertices and 2 triangles; the second
    // references vertex index 999999, far outside the object's own 0-2
    // vertex range. It must be dropped, not pushed into out.indices (which
    // would let downstream mesh code index past the position buffer).
    auto mesh = loadThreeMfMesh(fixture("import/oob_triangle.3mf"));
    REQUIRE(mesh.error.empty());
    REQUIRE(mesh.positions.size() == 3);
    REQUIRE(mesh.indices.size() == 3); // only the first (valid) triangle survives
    for (uint32_t idx : mesh.indices)
        REQUIRE(idx < mesh.positions.size());
}

TEST_CASE("ThreeMfLoader:not a ZIP archive reports a diagnostic, not a crash",
          "[3mf-loader][tier-e]") {
    auto mesh = loadThreeMfMesh(fixture("import/corrupt.3mf"));
    REQUIRE_FALSE(mesh.error.empty());
    REQUIRE(mesh.positions.empty());
}

TEST_CASE("ThreeMfLoader:a central-directory entry with a huge local-header offset is rejected, "
          "not read out-of-bounds",
          "[3mf-loader][tier-e]") {
    // malicious_offset.3mf's one central-directory entry claims a local
    // file header at offset 0xFFFFFFF0 (~4GB) in an 84-byte file. Computing
    // "offset + 30 > size" in 32-bit space wraps 0xFFFFFFF0 + 30 back down
    // to 14, which is NOT > 84 — so the bounds check would pass and
    // ZipReader would then read the ZIP local-file-header signature at the
    // real (huge) offset, far past the end of the in-memory buffer. Must be
    // rejected as malformed instead of reading out-of-bounds.
    auto mesh = loadThreeMfMesh(fixture("import/malicious_offset.3mf"));
    REQUIRE_FALSE(mesh.error.empty());
    REQUIRE(mesh.positions.empty());
}

TEST_CASE("ThreeMfLoader:missing file reports an error, not a crash", "[3mf-loader][tier-e]") {
    auto mesh = loadThreeMfMesh(fixture("import/does_not_exist.3mf"));
    REQUIRE_FALSE(mesh.error.empty());
}
