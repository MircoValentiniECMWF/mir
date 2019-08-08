/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_method_knn_pick_NClosestLSM_h
#define mir_method_knn_pick_NClosestLSM_h

#include "mir/method/knn/pick/NClosest.h"
#include "mir/method/knn/pick/PickWithLSM.h"


namespace mir {
namespace method {
namespace knn {
namespace pick {


struct NClosestLSM : PickWithLSM {
    NClosestLSM(const param::MIRParametrisation&, const lsm::LandSeaMasks&);
    void pick(const search::PointSearch&, size_t ip, const Point3&, neighbours_t&) const;
    size_t n() const { return 1; }
    bool sameAs(const Pick&) const;
private:
    void print(std::ostream&) const;
    void hash(eckit::MD5&) const;
    NClosest nClosest_;
};


}  // namespace pick
}  // namespace knn
}  // namespace method
}  // namespace mir


#endif

