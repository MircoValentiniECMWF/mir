/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/distance/ClimateFilter.h"

#include <cmath>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {
namespace knn {
namespace distance {


ClimateFilter::ClimateFilter(const param::MIRParametrisation& param) {
    double distance = 1.;
    param.get("distance", distance);
    ASSERT(distance > 0.);

    delta_ = 1000.;
    param.get("climate-filter-delta", delta_);
    ASSERT(delta_ > 0.);

    if (delta_ > distance) {
        auto str = [](const std::string& option, double value) {
            return "option '" + option + "' = " + std::to_string(value);
        };

        const std::string msg = "ClimateFilter: " + str("distance", distance) +
                                " should be greater than " + str("climate-filter-delta", delta_);
        eckit::Log::error() << msg << std::endl;
        throw eckit::UserError(msg);
    }

    halfDelta_ = distance / 2.;
}


void ClimateFilter::operator()(
        const Point3& point,
        const neighbours_t& neighbours,
        std::vector<double>& weights ) const {

    const size_t nbPoints = neighbours.size();
    ASSERT(nbPoints);

    weights.clear();
    weights.reserve(nbPoints);

    // calculate neighbour points weights, and their total (for normalisation)
    double sum = 0.;
    for (size_t j = 0; j < nbPoints; ++j) {
        auto r = Point3::distance(point, neighbours[j].point());
        auto h = r < halfDelta_ - delta_
                     ? 1.
                     : halfDelta_ + delta_ < r
                           ? 0.
                           : 0.5 + 0.5 * std::cos(M_PI_2 * (r - halfDelta_ + delta_) / delta_);
        // h = std::max(0., std::min(0.99, h));

        weights.emplace_back(h);
        sum += h;
    }

    // normalise all weights according to the total
    ASSERT(sum > 0.);
    double invSum = 1. / sum;
    std::for_each(weights.begin(), weights.end(), [=](double& w) { w *= invSum; });
}


bool ClimateFilter::sameAs(const DistanceWeighting& other) const {
    auto o = dynamic_cast<const ClimateFilter*>(&other);
    return o && eckit::types::is_approximately_equal(halfDelta_, o->halfDelta_) &&
           eckit::types::is_approximately_equal(delta_, o->delta_);
}


void ClimateFilter::print(std::ostream& out) const {
    out << "ClimateFilter[halfDelta=" << halfDelta_ << ",delta=" << delta_ << "]";
}


void ClimateFilter::hash(eckit::MD5& h) const {
    h.add("climate-filter");
    h.add(halfDelta_);
    h.add(delta_);
}


static DistanceWeightingBuilder<ClimateFilter> __distance("climate-filter");


}  // namespace distance
}  // namespace knn
}  // namespace method
}  // namespace mir

