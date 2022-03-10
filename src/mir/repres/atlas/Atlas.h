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


namespace atlas {
class Grid;
}


namespace mir {
namespace repres {
namespace atlas {


class Atlas : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Atlas(const Atlas&) = delete;

    // -- Destructor

    ~Atlas() = default;

    // -- Convertors
    // None

    // -- Operators

    Atlas& operator=(const Atlas&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    Atlas() = default;

    // -- Members
    // None

    // -- Methods

    virtual const ::atlas::Grid& atlasGridRef() const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    void validate(const MIRValuesVector&) const override;
    size_t numberOfPoints() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace atlas
}  // namespace repres
}  // namespace mir
