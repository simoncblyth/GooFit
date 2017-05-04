#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

#include "goofit/Error.h"
#include "goofit/GlobalCudaDefines.h"

class Indexable {
public:
    Indexable(std::string n, fptype val = 0) : name(n), value(val) {}
    virtual ~Indexable() {}
    
    /// Get the GooFit index
    int getIndex() const {return index;}
    /// Set the GooFit index
    void setIndex(int value) {index = value;}
    
    /// Get the index from the fitter
    int getFitterIndex() const {return fitter_index;}
    /// Set the index (should be done by the fitter)
    void setFitterIndex(int value) {fitter_index = value;}

    /// Get the name
    const std::string& getName() const {return name;}
    /// Set the name
    void setName(const std::string& val) {name = val;}
    
    /// Get the value
    fptype getValue() const {return value;}
    /// Set the value
    void setValue(fptype val) {value = val;}
    
protected:
    
    /// The variable name. Should be unique
    std::string name;
    
    /// The value of the variable
    fptype value;
    
    /// The goofit index, -1 if unset
    int index {-1};
    
    /// The fitter index, -1 if unset
    int fitter_index {-1};
};

/// Contains information about a parameter allowed
/// to vary in MINUIT, or an observable passed to a
/// data set. The index can refer either to cudaArray
/// or to an event.
class Variable : public Indexable {
public:
    friend std::ostream& operator<< (std::ostream& o, const Variable& var);
    friend std::istream& operator>> (std::istream& o, Variable& var);


    /// This is a constant varaible
    Variable(std::string n, fptype v)
      : Indexable(n, v)
      , error(0.002)
      , upperlimit(v + 0.01)
      , lowerlimit(v - 0.01)
      , fixed(true) {}
    
    /// This is an independent variable
    Variable(std::string n, fptype dn, fptype up)
    : Indexable(n)
    , upperlimit(up)
    , lowerlimit(dn) {}
    
    
    /// This is a normal variable, with value and upper/lower limits
    Variable(std::string n, fptype v, fptype dn, fptype up)
      : Indexable(n, v)
      , error(0.1*(up-dn))
      , upperlimit(up)
    , lowerlimit(dn) {}
    
    /// This is a full varaible with error scale as well
    Variable(std::string n, fptype v, fptype e, fptype dn, fptype up)
      : Indexable(n, v)
      , error(e)
      , upperlimit(up)
      , lowerlimit(dn) {}
    
    
    virtual ~Variable() = default;
    
    /// Get the error
    fptype getError() const {return error;}
    /// Set the error
    void setError(fptype val) {error = val;}
    
    /// Get the upper limit
    fptype getUpperLimit() const {return upperlimit;}
    /// Set the upper limit
    void setUpperLimit(fptype val) {upperlimit = val;}
    
    /// Get the lower limit
    fptype getLowerLimit() const {return lowerlimit;}
    /// Set the lower limit
    void setLowerLimit(fptype val) {lowerlimit = val;}
    
    /// Check to see if the value has changed this iteration (always true the first time)
    bool getChanged() const {return changed_;}
    
    /// Get and set the number of bins
    void setNumBins(size_t num) {
        if(locked_)
            throw GooFit::GeneralError("BinnedDataSet does not allow the bins to be changed after creation");
       numbins = num;
    }
    size_t getNumBins() const {return numbins;}
    
    /// Check to see if this is a constant
    bool IsFixed() const {return fixed;}
    
    /// Set the fixedness of a variable (needed for GooPdf)
    void setFixed(bool fix) {fixed = fix;}
    
    
    /// Check to see if this has been changed since last iteration
    void setChanged(bool val=true) {changed_ = val;}
    
    
    /// Get the bin size, (upper-lower) / bins
    fptype getBinSize() const {return (getUpperLimit() - getLowerLimit()) / getNumBins();}
    
    /// Currently deactivated
    void setBlind(fptype val) {blind = val;}
    

    
protected:
    
    /// The error
    fptype error;
    
    /// The upper limit
    fptype upperlimit;
    
    /// The lower limit
    fptype lowerlimit;
    
    /// A blinding value to add (disabled at the moment, TODO)
    fptype blind {0};
    
    /// The number of bins (mostly for BinnedData, or plotting help)
    size_t numbins {100};
    
    /// True if the value was unchanged since the last iteration
    bool changed_ {true};
    
    /// This "fixes" the variable (constant)
    bool fixed {false};
    
    /// You can no longer change the binning after a BinnedDataSet is created
    bool locked_ {false};
    
};

/// This is used to track event number for MPI versions.
/// A cast is done to know whether the values need to be fixed.
class CountingVariable : public Variable {
public:

    using Variable::Variable;
    virtual ~CountingVariable() = default;
};

/// This is similar to Variable, but the index points
/// to functorConstants instead of cudaArray.
class Constant : public Indexable {
public:

    Constant(std::string n, fptype val) : Indexable(n, val) {}
    virtual ~Constant() {}
};

inline std::ostream& operator<< (std::ostream& o, const Variable& var) {
    o << var.getName() << ": " << var.getValue() << " +/- " << var.getError();
    if(!var.fixed)
        o << " [" << var.getLowerLimit() << ", " << var.getUpperLimit() << "]";
    if(var.getIndex() >= 0)
        o << " GooFit index: " << var.getIndex();
    if(var.getFitterIndex() >= 0)
        o << " Fitter index: " << var.getFitterIndex();

    return o;
}

inline std::istream& operator>> (std::istream& i, Variable& var) {
    return i >> var.value;
}

/// Get the max index of a variable from a list
inline int max_index(const std::vector<Variable*> &vars) {
    const Variable* max_ind_ptr = *std::max_element(std::begin(vars),
                                              std::end(vars),
                                              [](const Variable *a, const Variable *b)
                                              {return a->getIndex() < b->getIndex();});
    return max_ind_ptr->getIndex();
}
