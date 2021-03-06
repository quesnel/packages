/*
 * @file vle/examples/fsa/MealyCounter1.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.extension.fsa @@endtagdepends
 */


#include <vle/extension/fsa/Mealy.hpp>

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State2 { START = 1, RUN };

class counter1 : public vf::Mealy
{
public:
    counter1(const vd::DynamicsInit& model,
             const vd::InitEventList& events) :
        vf::Mealy(model, events)
    {
        states(this) << START << RUN;

        transition(this, START, RUN) << event("in")
                                     << send(&counter1::out)
                                     << action(&counter1::start);
        transition(this, RUN, RUN) << event("in")
                                   << send(&counter1::out)
                                   << action(&counter1::run);

        initialState(START);

        value = 0;
    }

    void start(const vd::Time& /*time*/,
               const vd::ExternalEvent* /*event */)
    { value = 1; }
    void run(const vd::Time& /*time*/,
             const vd::ExternalEvent* /*event */)
    { ++value; }
    void out(const vd::Time& /*time*/,
             vd::ExternalEventList& output) const
    {
        output.emplace_back("out");
        vle::value::Map& attrs = output.back().addMap();
        attrs.addString("name","counter");
        attrs.addInt("value",value);
    }

    virtual ~counter1() { }

    virtual std::unique_ptr<vle::value::Value> observation(
        const vd::ObservationEvent& event) const override
    {
        if (event.onPort("counter")) {
            return vle::value::Integer::create(value);
        }
        return vf::Mealy::observation(event);
    }

private:
    int value;
};

DECLARE_DYNAMICS(counter1)

}}} // namespace vle examples fsa
