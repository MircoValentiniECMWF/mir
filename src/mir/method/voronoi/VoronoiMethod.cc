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


#include "mir/method/voronoi/VoronoiMethod.h"

#include <algorithm>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/utils/MD5.h"

#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/search/PointSearch.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Trace.h"


namespace mir {
namespace method {
namespace voronoi {


namespace {


struct Biplet : std::pair<size_t, size_t> {
    using pair::pair;
    operator WeightMatrix::Triplet() const { return {first, second, 1. /*non-zero*/}; }
    bool operator<(const Biplet& other) const {
        return first < other.first || (first == other.first && second < other.second);
    }
};


}  // namespace


VoronoiMethod::VoronoiMethod(const param::MIRParametrisation& param) : MethodWeighted(param), pick_(1) {}


bool VoronoiMethod::sameAs(const Method& other) const {
    auto o = dynamic_cast<const VoronoiMethod*>(&other);
    return (o != nullptr) && name() == o->name() && MethodWeighted::sameAs(*o);
}


void VoronoiMethod::assemble(util::MIRStatistics&, WeightMatrix& W, const repres::Representation& in,
                             const repres::Representation& out) const {
    auto& log = Log::debug();
    log << "VoronoiMethod::assemble (input: " << in << ", output: " << out << ")" << std::endl;


    std::unique_ptr<search::PointSearch> tree;
    {
        trace::ResourceUsage usage("assemble: create output k-d tree", log);
        tree.reset(new search::PointSearch(parametrisation_, out));
    }


    auto Nin  = in.numberOfPoints();
    auto Nout = out.numberOfPoints();

    std::vector<bool> assigned;
    assigned.assign(Nout, false);

    std::set<Biplet> biplets;


    {
        trace::ProgressTimer progress("assemble: input-based assign", Nin, {"point"}, log);

        std::vector<search::PointSearch::PointValueType> closest;
        for (const std::unique_ptr<repres::Iterator> it(in.iterator()); it->next();) {
            if (++progress) {
                log << *tree << std::endl;
            }

            pick_.pick(*tree, it->point3D(), closest);
            for (auto& c : closest) {
                auto i = c.payload();
                biplets.emplace(i, it->index());
                assigned[i] = true;
            }
        }
    }


    auto Nassigned = size_t(std::count(assigned.cbegin(), assigned.cend(), true));
    if (Nassigned < Nout) {
        Log::debug() << "assemble: input-based assignment: " << Nassigned << " of "
                     << Log::Pretty(Nout, {"output point"}) << " (completing with output-based assignment)"
                     << std::endl;

        {
            trace::ResourceUsage usage("assemble: create input k-d tree", log);
            tree.reset(new search::PointSearch(parametrisation_, in));
        }

        {
            trace::ProgressTimer progress("assemble: output-based assign", Nout - Nassigned, {"point"}, log);

            std::vector<search::PointSearch::PointValueType> closest;
            for (const std::unique_ptr<repres::Iterator> it(out.iterator()); it->next();) {
                auto i = it->index();
                if (assigned[i]) {
                    continue;
                }

                if (++progress) {
                    log << *tree << std::endl;
                }

                pick_.pick(*tree, it->point3D(), closest);
                for (auto& c : closest) {
                    auto j = c.payload();
                    biplets.emplace(i, j);  // won't insert biplet if existing
                }
            }
        }
    }

    {
        trace::Timer time("assemble: fill sparse matrix", log);

        // TODO: triplets, really? why not writing to the matrix directly?

        // fill sparse matrix (disable expensive validation, as the matrix isn't very sparse)
        ASSERT_NONEMPTY_INTERPOLATION("VoronoiMethod", !biplets.empty());
        W.setFromTriplets({biplets.begin(), biplets.end()});
        W.validateMatrixEntryBounds(false);
        W.validateMatrixRowSum(false);
    }
}


void VoronoiMethod::hash(eckit::MD5& md5) const {
    MethodWeighted::hash(md5);
    std::ostringstream str;
    print(str);
    md5.add(str.str());
}


void VoronoiMethod::print(std::ostream& out) const {
    out << "VoronoiMethod[name=" << name() << ",";
    MethodWeighted::print(out);
    out << "]";
}


const char* VoronoiMethod::name() const {
    return "voronoi";
}


}  // namespace voronoi
}  // namespace method
}  // namespace mir
