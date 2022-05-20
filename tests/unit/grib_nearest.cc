/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"

#include "mir/data/MIRField.h"
#include "mir/input/GribFileInput.h"
#include "mir/method/knn/pick/NClosestOrNearest.h"
#include "mir/repres/Representation.h"
#include "mir/search/PointSearch.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Types.h"


namespace mir {
namespace tests {
namespace unit {


struct GribNearest : tools::MIRTool {
    GribNearest(int argc, char** argv) : MIRTool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<bool>("dont-assert", "Don't assert when checking test"));
        options_.push_back(
            new eckit::option::SimpleOption<bool>("dont-check-multiple", "Disable checking for multiple matches"));
    }

    int minimumPositionalArguments() const override { return 2; }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << " input.grib tests.yaml" << std::endl;
    }

    void execute(const eckit::option::CmdArgs& args) override {
        bool dont_assert = false;
        args.get("dont-assert", dont_assert);


        // setup input and search tree
        std::unique_ptr<input::MIRInput> input(new input::GribFileInput(args(0)));
        ASSERT(input && input->next());  // only check first GRIB message

        const auto field = input->field();
        const repres::RepresentationHandle repres(field.representation());
        const search::PointSearch sptree(input->parametrisation(), *repres);

        constexpr double tol  = 1.;
        constexpr double tol2 = tol * tol;

        method::knn::pick::Pick::neighbours_t closest;
        std::unique_ptr<method::knn::pick::Pick> picker(new method::knn::pick::NClosestOrNearest(2, tol));


        // setup tests
        const eckit::PathName path(args(1));
        ASSERT(path.exists());

        eckit::Translator<std::string, size_t> i;
        eckit::Translator<std::string, double> d;

        using atlas::PointLonLat;
        using test_t = std::pair<PointLonLat, size_t>;

        std::vector<test_t> tests;
        for (const auto& test : eckit::ValueMap(eckit::YAMLParser::decodeFile(path))) {
            const auto ll = eckit::StringTools::split(",", test.first);
            ASSERT(ll.size() == 2);

            // notice the order
            const auto lon = d(eckit::StringTools::trim(ll[0]));
            const auto lat = d(eckit::StringTools::trim(ll[1]));

            tests.emplace_back(PointLonLat{lon, lat}, i(test.second));
        }

        auto point_to_ll = [](const Point3& p) -> PointLonLat {
            PointLonLat ll;
            util::Earth::convertCartesianToSpherical(p, ll);
            return ll;
        };


        // test
        for (const auto& test : tests) {
            Point3 p;
            util::Earth::convertSphericalToCartesian(test.first, p);

            picker->pick(sptree, p, closest);

            if (closest.size() >= 2) {
                const auto df2 = Point3::distance2(p, closest.front().point());
                const auto db2 = Point3::distance2(p, closest.back().point());

                if (eckit::types::is_approximately_lesser_or_equal(db2 - df2, tol2)) {
                    // multiple matches found
                    Log::info() << "Test " << test.first << " -> " << test.second << ", " << closest.size()
                                << " results:" << std::endl;

                    bool ok = false;
                    for (const auto& n : closest) {
                        Log::info() << "Result " << point_to_ll(n.point()) << " -> " << n.payload() << std::endl;
                        ok = ok || n.payload() == test.second;
                    }

                    EXPECT(dont_assert || ok);
                    continue;
                }
            }

            ASSERT(!closest.empty());
            const auto& n = closest.front();

            Log::info() << "Test " << test.first << " -> " << test.second << ", result " << point_to_ll(n.point())
                        << " -> " << n.payload() << std::endl;
            EXPECT(dont_assert || n.payload() == test.second);
        }
    }
};


}  // namespace unit
}  // namespace tests
}  // namespace mir


int main(int argc, char** argv) {
    mir::tests::unit::GribNearest tool(argc, argv);
    return tool.start();
}
