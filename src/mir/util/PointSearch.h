/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   Apr 2015


#ifndef mir_method_PointSearch_h
#define mir_method_PointSearch_h

#include <iostream>
#include <vector>

#include "eckit/container/sptree/SPNodeInfo.h"
#include "eckit/container/sptree/SPValue.h"
#include "eckit/geometry/Point3.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"


namespace mir {

namespace param {
class MIRParametrisation;
};

namespace repres {
class Representation;
}
}


namespace mir {
namespace util {


class PointSearchTree {
public:
    typedef eckit::geometry::Point3             Point;
    typedef size_t                              Payload;
    typedef eckit::SPValue<PointSearchTree>     PointValueType;

public:
    virtual ~PointSearchTree();

    virtual void build(std::vector<PointValueType>&);

    virtual void insert(const PointValueType&);
    virtual void statsPrint(std::ostream&, bool);
    virtual void statsReset();

    virtual PointValueType nearestNeighbour(const Point&);
    virtual std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k);
    virtual std::vector<PointValueType> findInSphere(const Point&, double);

    virtual bool ready() const;
    virtual void commit();
    virtual void print(std::ostream &) const;

    virtual void lock();
    virtual void unlock();

    friend std::ostream &operator<<(std::ostream &s, const PointSearchTree &p) {
        p.print(s);
        return s;
    }
};


/// Class for fast searches in point clouds following kd-tree algorithms
/// @todo test kd-tree stored in shared memory?
class PointSearch : private eckit::NonCopyable {
public:

    typedef PointSearchTree::Payload        ValueType;
    typedef PointSearchTree::Point          PointType;
    typedef PointSearchTree::PointValueType PointValueType;

public:

    PointSearch(const param::MIRParametrisation&,
                const repres::Representation&);

    PointSearch(const param::MIRParametrisation&,
                const std::vector<PointValueType>&);

public:

    /// Finds closest point to an input point
    PointValueType closestPoint(const PointType& pt) const;

    /// Finds closest N points to an input point
    void closestNPoints(const PointType& pt, size_t n, std::vector<PointValueType>& closest) const;

    /// Finds closest points within a radius
    void closestWithinRadius(const PointType& pt, double radius, std::vector<PointValueType>& closest) const;

    void statsPrint(std::ostream& o, bool fancy) const;
    void statsReset() const;

private:

    const param::MIRParametrisation& parametrisation_;
    eckit::ScopedPtr<PointSearchTree> tree_;

    void build(const repres::Representation& r);

};


class PointSearchTreeFactory {

    std::string name_;

    virtual PointSearchTree* make(const repres::Representation&,
                                  const param::MIRParametrisation&,
                                  size_t itemCount) = 0;

    virtual PointSearchTree* make(const param::MIRParametrisation&,
                                  size_t itemCount) = 0;

protected:

    PointSearchTreeFactory(const std::string&);

    virtual  ~PointSearchTreeFactory();

public:

    static PointSearchTree* build(const repres::Representation&,
                                  const param::MIRParametrisation&,
                                  size_t itemCount);

    static PointSearchTree* build(const param::MIRParametrisation&,
                                  size_t itemCount);

    static void list(std::ostream&);

};


template<class T>
class PointSearchTreeBuilder : public PointSearchTreeFactory {
    virtual PointSearchTree* make(const repres::Representation& r,
                                  const param::MIRParametrisation& param,
                                  size_t itemCount) {
        return new T(r, param, itemCount);
    }
    virtual PointSearchTree* make(const param::MIRParametrisation& param,
                                  size_t itemCount) {
        return new T(param, itemCount);
    }
public:
    PointSearchTreeBuilder(const std::string &name) : PointSearchTreeFactory(name) {}
};


}  // namespace util
}  // namespace mir


#endif

