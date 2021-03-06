/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2014 INRA http://www.inra.fr
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


#ifndef VLE_DISCRETE_TIME_TEMPORAL_VALUES_HPP
#define VLE_DISCRETE_TIME_TEMPORAL_VALUES_HPP 1

#include <deque>
#include <vector>
#include <iostream>
#include <map>
#include <vle/devs/Time.hpp>
#include <vle/devs/InitEventList.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>

namespace vle {
namespace discrete_time {


enum VAR_TYPE {MONO, MULTI, VALUE_VLE};
enum SNAPSHOT_ID {SNAP1, SNAP2};

class TemporalValuesProvider;


struct VarUpdate
{
    VarUpdate(const vle::devs::Time& t, double val);
    vle::devs::Time timeOfUpdate;
    double value;
};

struct VectUpdate
{
    VectUpdate(const devs::Time& t, unsigned int dim);
    VectUpdate(const devs::Time& t, unsigned int dim, bool complet);
    VectUpdate(const devs::Time& t, const value::Tuple&);
    VectUpdate(const devs::Time& t, const value::Table&, unsigned int col);

    virtual ~VectUpdate();

    void assign(const vle::value::Tuple&);

    vle::devs::Time timeOfUpdate;
    std::vector<double> value;
    bool complete;
};

struct VarValueUpdate
{
    VarValueUpdate(const VarValueUpdate& v);
    VarValueUpdate(const vle::devs::Time& t,
            std::unique_ptr<vle::value::Value> val);
    virtual ~VarValueUpdate();

    vle::devs::Time timeOfUpdate;
    std::unique_ptr<vle::value::Value> value;
};

struct VarMono;
struct VarMulti;
struct VarValue;

struct VarInterface
{
    bool error_no_sync;
    bool allow_update;
    bool history_size_given;
    unsigned int history_size;
    std::unique_ptr<vle::value::Value> init_value;
    TemporalValuesProvider* tvp;

    VarInterface(TemporalValuesProvider* eq);
    virtual ~VarInterface();

    bool isVarMono() const;
    bool isVarMulti() const;
    bool isVarValue() const;
    const VarMono& toVarMono() const;
    const VarMulti& toVarMulti() const;
    const VarValue& toVarValue() const;
    VarMono& toVarMono();
    VarMulti& toVarMulti();
    VarValue& toVarValue();

    void initHistoryVar(const std::string& varName, const vle::devs::Time& t);

    virtual void update(const vle::devs::Time& t,
            const vle::value::Value& val) = 0;
    virtual VAR_TYPE getType() const = 0;
    virtual vle::devs::Time lastUpdateTime() const = 0;



};

struct VarMono : public VarInterface
{

    static double getDefaultInit();

    typedef std::deque<VarUpdate*> History;
    typedef std::map<SNAPSHOT_ID, double> Snapshot;

    History history;
    Snapshot* snapshot;

    VarMono(TemporalValuesProvider* eq);
    virtual ~VarMono();

    VAR_TYPE getType() const;
    double getVal(const vle::devs::Time& t, double delay) const;
    void update(const vle::devs::Time& t, double val);
    void update(const vle::devs::Time& t, const vle::value::Value& val);
    vle::devs::Time lastUpdateTime() const;
    double lastVal(const vle::devs::Time& beg, const vle::devs::Time& end);

    void addSnapshot(SNAPSHOT_ID idSnap, double val);
    bool hasSnapshot(SNAPSHOT_ID idSnap);
    double getSnapshot(SNAPSHOT_ID idSnap);
    void clearSnapshot();

};

struct VarMulti : public VarInterface
{
    typedef std::deque<VectUpdate*> History;
    typedef std::map<SNAPSHOT_ID, std::vector<double> > Snapshot;

    History history;
    Snapshot* snapshot;
    unsigned int dim;

    VarMulti(TemporalValuesProvider* eq, unsigned int dimension);
    virtual ~VarMulti();

    VAR_TYPE getType() const;
    double getVal(unsigned int i, const vle::devs::Time& t, double delay) const;
    const std::vector<double>& getVal(const vle::devs::Time& t,
            double delay) const;
    void update(const vle::devs::Time& /*t*/, const vle::value::Value& /*val*/);
    void update(const vle::devs::Time& /*t*/, unsigned int dim, double val);
    vle::devs::Time lastUpdateTime() const;

    void addSnapshot(SNAPSHOT_ID idSnap, const std::vector<double>& val);
    bool hasSnapshot(SNAPSHOT_ID idSnap);
    const std::vector<double>& getSnapshot(SNAPSHOT_ID idSnap);
    void clearSnapshot();
};


struct VarValue : public VarInterface
{
    typedef std::deque<VarValueUpdate*> History;
    typedef std::map<SNAPSHOT_ID, std::unique_ptr<vle::value::Value>> Snapshot;

    History history;
    Snapshot* snapshot;

    VarValue(TemporalValuesProvider* eq);
    virtual ~VarValue();

    VAR_TYPE getType() const;
    const vle::value::Value&  getVal(const vle::devs::Time& t,
            double delay) const;
    void update(const vle::devs::Time& /*t*/,
            const vle::value::Value& /*val*/);
    vle::devs::Time lastUpdateTime() const;
    const vle::value::Value& lastVal(const vle::devs::Time& beg,
            const vle::devs::Time& end);

    void addSnapshot(SNAPSHOT_ID idSnap, const vle::value::Value& val);
    bool hasSnapshot(SNAPSHOT_ID idSnap);
    const vle::value::Value& getSnapshot(SNAPSHOT_ID idSnap);
    void clearSnapshot();

};

typedef std::map < std::string, VarInterface*> Variables;

////////////////////
//User interface
////////////////////

class Var
{
public:
    std::string name;
    VarMono* itVar;

    Var();
    //Var(VarMono* v);
    virtual ~Var();

    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap, const devs::Time& currentTime);
    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap);

    void init_value(double v);
    void init_history(const vle::devs::Time& t);



    void history_size(unsigned int s);


    double operator()(double delay) const;
    double operator()() const;
    double lastVal(double delayBeg, double delayEnd) const;
    void operator=(double v);
};

struct Vect_i
{
    VarMulti* itVar;
    unsigned int dim;

    Vect_i(VarMulti* itv, unsigned int d);

    double operator()(double delay);
    double operator()();
    void operator=(double val);
};

class Vect
{
public:
    std::string name;
    VarMulti* itVar;

    Vect();
    virtual ~Vect();

    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap, const vle::devs::Time& currentTime);
    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap);

    void history_size(unsigned int s);
    void dim(unsigned int s);
    unsigned int dim() const;

    Vect_i operator[](unsigned int i);
};


class ValueVle
{
public:

    std::string name;
    VarValue* itVar;

    ValueVle();
    virtual ~ValueVle();

    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap, const vle::devs::Time& currentTime);
    void init(TemporalValuesProvider* tvpin, const std::string& varName,
            const devs::InitEventList& initMap);

    void history_size(unsigned int s);

    const vle::value::Value& operator()(double delay) const;
    vle::value::Value& operator()();
    const vle::value::Value& lastVal(double delayBeg, double delayEnd) const;
    void operator=(const vle::value::Value& val);
};

class TemporalValuesProvider
{
public:
     TemporalValuesProvider();
     TemporalValuesProvider(const std::string& model_name,
             const vle::devs::InitEventList& initMap);
     virtual ~TemporalValuesProvider();

     void initHistory(const vle::devs::Time& t);
     void snapshot(SNAPSHOT_ID idSnap);

     unsigned int dim(const Vect& v) const;
     const std::string& get_model_name() const;
     Variables& getVariables();
     const Variables& getVariables() const;
     const vle::devs::Time& getCurrentTime() const;
     double getDelta() const;

     void setCurrentTime(const vle::devs::Time& t);
     std::string getVarName(VarInterface* v);

private:
     std::string model_name;
     Variables variables;
     vle::devs::Time current_time;
     double delta;

};

inline std::ostream&
operator<<(std::ostream& o, const VarMono::History& hs)
{
    VarMono::History::const_iterator itb = hs.begin();
    VarMono::History::const_iterator ite = hs.end();
    for (;itb != ite; itb++) {
        o <<" [" << (*itb)->timeOfUpdate <<" : " << (*itb)->value <<"], ";
    }
    return o;
}

inline std::ostream&
operator<<(std::ostream& o, const VarMulti::History& hs)
{
    VarMulti::History::const_iterator itb = hs.begin();
    VarMulti::History::const_iterator ite = hs.end();
    for (;itb != ite; itb++) {
        o <<" [" << (*itb)->timeOfUpdate <<" : " ;
        const std::vector<double>& vu = (*itb)->value;
        for (unsigned int i =0; i < vu.size(); i++) {
            o << vu[i] << " ";
        }
    }
    o << "], ";
    return o;
}


inline std::ostream&
operator<<(std::ostream& o, const VarValue::History& hs)
{
    VarValue::History::const_iterator itb = hs.begin();
    VarValue::History::const_iterator ite = hs.end();
    for (;itb != ite; itb++) {
        o <<" [" << (*itb)->timeOfUpdate <<" : " << *((*itb)->value) <<"], ";
    }
    return o;
}

inline std::ostream&
operator<<(std::ostream& o, const VarInterface& v)
{
    switch (v.getType()) {
    case MONO : {
        const VarMono& mono = static_cast<const VarMono&>(v);
        o << mono.history ;
        break;
    } case MULTI : {
        const VarMulti& multi = static_cast<const VarMulti&>(v);
        o << " :: " ;
        for (unsigned int i =0 ; i < multi.dim ; i++) {
            //TODO
        }
        break;
    } case VALUE_VLE: {
        break;
    }}
    return o;
}

}} // namespaces

#endif
