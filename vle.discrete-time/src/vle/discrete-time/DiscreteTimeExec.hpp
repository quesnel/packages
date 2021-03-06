/*
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2013 INRA http://www.inra.fr
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


#ifndef VLE_DISCRETE_TIME_EXEC_HPP
#define VLE_DISCRETE_TIME_EXEC_HPP 1

#include <vle/devs/Executive.hpp>
#include <vle/discrete-time/details/DiscreteTimeGen.hpp>
#include <vle/discrete-time/details/ComputeInterface.hpp>

namespace vle {
namespace discrete_time {

struct Pimpl;

/**
 * @brief Discrete time for a dynamic
 */
class  DiscreteTimeExec : public vle::devs::Executive,
                         public TemporalValuesProvider,
                         public ComputeInterface
{
public:

    DiscreteTimeExec(const vle::devs::ExecutiveInit& init,
            const vle::devs::InitEventList&  events);

    virtual ~DiscreteTimeExec();

    vle::devs::Dynamics* toDynamics();

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    /////////////////////////////
    //User API Overwrite options
    /////////////////////////////

    void time_step(double val) ;
    void init_value(const std::string& v, const vle::value::Value& val);
    void dim(const std::string& v, unsigned int val);
    void history_size(const std::string& v, unsigned int val);
    void sync(const std::string& v, unsigned int val);
    void output_nil(const std::string& v, bool val);
    void output_period(const std::string& v, unsigned int val);
    void allow_update(const std::string& v, bool val);
    void error_no_sync(const std::string& v, bool val);
    void bags_to_eat(unsigned int val);
    void global_output_nils(bool val);


    /////////////////////////////
    //User API miscelenaous
    /////////////////////////////

    unsigned int dim(const Vect&) const;
    bool firstCompute() const;
    DEVS_Options& getOptions();

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    //////////////////////
    //DEVS implementation
    //////////////////////

    virtual vle::devs::Time init(vle::devs::Time time) override;

    virtual vle::devs::Time timeAdvance() const override;

    virtual void internalTransition(vle::devs::Time time) override;

    virtual void externalTransition(
        const vle::devs::ExternalEventList& event,
        vle::devs::Time time) override;

    virtual void confluentTransitions(
        vle::devs::Time internal,
        const vle::devs::ExternalEventList& extEventlist) override;

    virtual void output(vle::devs::Time /* time */,
        vle::devs::ExternalEventList& /* output */) const override;

    virtual std::unique_ptr<vle::value::Value> observation(
    const vle::devs::ObservationEvent& /* event */) const override;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    ////////////////////
    //Internal
    ////////////////////

    /**
     * @brief Process method used for DEVS state entrance
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processIn(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Process method used for DEVS state exit
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processOut(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Update guards for internal transition
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void updateGuards(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Handles the list of external event
     * @param t, the current time
     * @param ext, the list of external event
     */
    virtual void handleExtEvt(const vle::devs::Time& t,
            const vle::devs::ExternalEventList& ext) override;

    /**
     * @brief Outputs one state variable
     * @param model, the atomic model for which output occurs
     * @param time, the current time
     * @param output, the list of outputs external event to fill
     */
    virtual void outputVar(const vle::vpz::AtomicModel& model,
                           const vle::devs::Time& time,
                           vle::devs::ExternalEventList& output) override;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    ////////////////////
    //Internal
    ////////////////////
    /**
     * @brief Handles one external even
     * @param t, the current time
     * @param port, the name of the port
     * @param attr, a map of attributes
     */
    void handleExtVar(const vle::devs::Time& t,
            const std::string& port, const vle::value::Map& attrs);
    void outputVar(const vle::devs::Time& time,
            vle::devs::ExternalEventList& output) const;
    void updateGuardAllSynchronized(const vle::devs::Time& t);
    void updateGuardHasSync(const vle::devs::Time& t);
    void varOnSyncError(std::string& v);
    bool isSync(const std::string& var_name, unsigned int currTimeStep) const;
    void initializeFromInitEventList(const vle::devs::InitEventList&  events);

    Pimpl* mpimpl;
};

}} // namespaces

#endif
