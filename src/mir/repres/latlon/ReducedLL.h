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


#pragma once

#include "mir/repres/Gridded.h"


namespace mir {
namespace repres {
namespace latlon {


class ReducedLL : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ReducedLL(const param::MIRParametrisation&);

    // -- Destructor

    ~ReducedLL() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    atlas::Grid atlasGrid() const override;

    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::vector<long> pl_;

    // -- Methods
    // None

    // -- Overridden methods

    void fill(grib_info&) const override;
    void fill(api::MIRJob&) const override;
    void fill(util::MeshGeneratorParameters&) const override;

    void validate(const MIRValuesVector&) const override;

    Iterator* iterator() const override;

    void makeName(std::ostream&) const override;
    bool sameAs(const Representation&) const override;

    size_t numberOfPoints() const override;
    bool getLongestElementDiagonal(double&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace latlon
}  // namespace repres
}  // namespace mir
