#include "import/AmfLoader.h"

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

TEST_CASE("AmfLoader:loads a tetrahedron from element text content", "[amf-loader][tier-e]") {
    auto mesh = loadAmfMesh(fixture("import/tetra.amf"));
    REQUIRE(mesh.error.empty());
    REQUIRE(mesh.positions.size() == 4);
    REQUIRE(mesh.indices.size() == 12);
    REQUIRE(mesh.positions[1].x == Approx(1.0));
    REQUIRE(mesh.positions[3].z == Approx(1.0));
}

TEST_CASE("AmfLoader:an out-of-range triangle index is skipped, not used as-is",
          "[amf-loader][tier-e]") {
    // Same shape as ThreeMfLoader's equivalent test: 3 vertices, 2
    // triangles, the second referencing vertex index 999999.
    auto mesh = loadAmfMesh(fixture("import/oob_triangle.amf"));
    REQUIRE(mesh.error.empty());
    REQUIRE(mesh.positions.size() == 3);
    REQUIRE(mesh.indices.size() == 3);
    for (uint32_t idx : mesh.indices)
        REQUIRE(idx < mesh.positions.size());
}

TEST_CASE("AmfLoader:empty mesh reports a diagnostic, not a crash", "[amf-loader][tier-e]") {
    auto mesh = loadAmfMesh(fixture("import/empty.amf"));
    REQUIRE_FALSE(mesh.error.empty());
    REQUIRE(mesh.positions.empty());
}

TEST_CASE("AmfLoader:missing file reports an error, not a crash", "[amf-loader][tier-e]") {
    auto mesh = loadAmfMesh(fixture("import/does_not_exist.amf"));
    REQUIRE_FALSE(mesh.error.empty());
}
