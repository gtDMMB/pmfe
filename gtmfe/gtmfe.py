# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.12
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_gtmfe', [dirname(__file__)])
        except ImportError:
            import _gtmfe
            return _gtmfe
        if fp is not None:
            try:
                _mod = imp.load_module('_gtmfe', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _gtmfe = swig_import_helper()
    del swig_import_helper
else:
    import _gtmfe
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class SwigPyIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SwigPyIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SwigPyIterator, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _gtmfe.delete_SwigPyIterator
    __del__ = lambda self : None;
    def value(self): return _gtmfe.SwigPyIterator_value(self)
    def incr(self, n=1): return _gtmfe.SwigPyIterator_incr(self, n)
    def decr(self, n=1): return _gtmfe.SwigPyIterator_decr(self, n)
    def distance(self, *args): return _gtmfe.SwigPyIterator_distance(self, *args)
    def equal(self, *args): return _gtmfe.SwigPyIterator_equal(self, *args)
    def copy(self): return _gtmfe.SwigPyIterator_copy(self)
    def next(self): return _gtmfe.SwigPyIterator_next(self)
    def __next__(self): return _gtmfe.SwigPyIterator___next__(self)
    def previous(self): return _gtmfe.SwigPyIterator_previous(self)
    def advance(self, *args): return _gtmfe.SwigPyIterator_advance(self, *args)
    def __eq__(self, *args): return _gtmfe.SwigPyIterator___eq__(self, *args)
    def __ne__(self, *args): return _gtmfe.SwigPyIterator___ne__(self, *args)
    def __iadd__(self, *args): return _gtmfe.SwigPyIterator___iadd__(self, *args)
    def __isub__(self, *args): return _gtmfe.SwigPyIterator___isub__(self, *args)
    def __add__(self, *args): return _gtmfe.SwigPyIterator___add__(self, *args)
    def __sub__(self, *args): return _gtmfe.SwigPyIterator___sub__(self, *args)
    def __iter__(self): return self
SwigPyIterator_swigregister = _gtmfe.SwigPyIterator_swigregister
SwigPyIterator_swigregister(SwigPyIterator)

class ParameterVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ParameterVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ParameterVector, name)
    __repr__ = _swig_repr
    __swig_setmethods__["multiloop_penalty"] = _gtmfe.ParameterVector_multiloop_penalty_set
    __swig_getmethods__["multiloop_penalty"] = _gtmfe.ParameterVector_multiloop_penalty_get
    if _newclass:multiloop_penalty = _swig_property(_gtmfe.ParameterVector_multiloop_penalty_get, _gtmfe.ParameterVector_multiloop_penalty_set)
    __swig_setmethods__["unpaired_penalty"] = _gtmfe.ParameterVector_unpaired_penalty_set
    __swig_getmethods__["unpaired_penalty"] = _gtmfe.ParameterVector_unpaired_penalty_get
    if _newclass:unpaired_penalty = _swig_property(_gtmfe.ParameterVector_unpaired_penalty_get, _gtmfe.ParameterVector_unpaired_penalty_set)
    __swig_setmethods__["branch_penalty"] = _gtmfe.ParameterVector_branch_penalty_set
    __swig_getmethods__["branch_penalty"] = _gtmfe.ParameterVector_branch_penalty_get
    if _newclass:branch_penalty = _swig_property(_gtmfe.ParameterVector_branch_penalty_get, _gtmfe.ParameterVector_branch_penalty_set)
    __swig_setmethods__["dummy_scaling"] = _gtmfe.ParameterVector_dummy_scaling_set
    __swig_getmethods__["dummy_scaling"] = _gtmfe.ParameterVector_dummy_scaling_get
    if _newclass:dummy_scaling = _swig_property(_gtmfe.ParameterVector_dummy_scaling_get, _gtmfe.ParameterVector_dummy_scaling_set)
    def __init__(self, *args): 
        this = _gtmfe.new_ParameterVector(*args)
        try: self.this.append(this)
        except: self.this = this
    def set_from_pairs(self, *args): return _gtmfe.ParameterVector_set_from_pairs(self, *args)
    def get_pairs(self): return _gtmfe.ParameterVector_get_pairs(self)
    def set_from_fractions(self, multiloop_fraction, unpaired_fraction, branch_fraction, dummy_fraction):
        self.set_from_pairs( pairll(multiloop_fraction.numerator, multiloop_fraction.denominator),
                             pairll(unpaired_fraction.numerator, unpaired_fraction.denominator),
                             pairll(branch_fraction.numerator, branch_fraction.denominator),
                             pairll(dummy_fraction.numerator, dummy_fraction.denominator),
                             )

    def get_python_numbers(self):
        from fractions import Fraction
        pairs = self.get_pairs()
        result = {"multiloop penalty": Fraction(pairs[0][0], pairs[0][1]),
                  "unpaired penalty": Fraction(pairs[1][0], pairs[1][1]),
                  "branch penalty": Fraction(pairs[2][0], pairs[2][1]),
                  "dummy scaling": Fraction(pairs[3][0], pairs[3][1]),
        }
        return result

    __swig_destroy__ = _gtmfe.delete_ParameterVector
    __del__ = lambda self : None;
ParameterVector_swigregister = _gtmfe.ParameterVector_swigregister
ParameterVector_swigregister(ParameterVector)

class PolytopeVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PolytopeVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PolytopeVector, name)
    __repr__ = _swig_repr
    __swig_setmethods__["multiloops"] = _gtmfe.PolytopeVector_multiloops_set
    __swig_getmethods__["multiloops"] = _gtmfe.PolytopeVector_multiloops_get
    if _newclass:multiloops = _swig_property(_gtmfe.PolytopeVector_multiloops_get, _gtmfe.PolytopeVector_multiloops_set)
    __swig_setmethods__["branches"] = _gtmfe.PolytopeVector_branches_set
    __swig_getmethods__["branches"] = _gtmfe.PolytopeVector_branches_get
    if _newclass:branches = _swig_property(_gtmfe.PolytopeVector_branches_get, _gtmfe.PolytopeVector_branches_set)
    __swig_setmethods__["unpaired"] = _gtmfe.PolytopeVector_unpaired_set
    __swig_getmethods__["unpaired"] = _gtmfe.PolytopeVector_unpaired_get
    if _newclass:unpaired = _swig_property(_gtmfe.PolytopeVector_unpaired_get, _gtmfe.PolytopeVector_unpaired_set)
    __swig_setmethods__["w"] = _gtmfe.PolytopeVector_w_set
    __swig_getmethods__["w"] = _gtmfe.PolytopeVector_w_get
    if _newclass:w = _swig_property(_gtmfe.PolytopeVector_w_get, _gtmfe.PolytopeVector_w_set)
    __swig_setmethods__["energy"] = _gtmfe.PolytopeVector_energy_set
    __swig_getmethods__["energy"] = _gtmfe.PolytopeVector_energy_get
    if _newclass:energy = _swig_property(_gtmfe.PolytopeVector_energy_get, _gtmfe.PolytopeVector_energy_set)
    __swig_setmethods__["params"] = _gtmfe.PolytopeVector_params_set
    __swig_getmethods__["params"] = _gtmfe.PolytopeVector_params_get
    if _newclass:params = _swig_property(_gtmfe.PolytopeVector_params_get, _gtmfe.PolytopeVector_params_set)
    def get_pairs(self): return _gtmfe.PolytopeVector_get_pairs(self)
    def get_python_numbers(self):
        from fractions import Fraction
        pairs = self.get_pairs()
        result = {"multiloops" : pairs[0][0],
                  "unpaired": pairs[1][0],
                  "branches": pairs[2][0],
                  "w": Fraction(pairs[3][0], pairs[3][1]),
                  "energy": Fraction(pairs[4][0], pairs[4][1]),
        }
                  
        return result
        
    def __init__(self): 
        this = _gtmfe.new_PolytopeVector()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _gtmfe.delete_PolytopeVector
    __del__ = lambda self : None;
PolytopeVector_swigregister = _gtmfe.PolytopeVector_swigregister
PolytopeVector_swigregister(PolytopeVector)

class pairll(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, pairll, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, pairll, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _gtmfe.new_pairll(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_setmethods__["first"] = _gtmfe.pairll_first_set
    __swig_getmethods__["first"] = _gtmfe.pairll_first_get
    if _newclass:first = _swig_property(_gtmfe.pairll_first_get, _gtmfe.pairll_first_set)
    __swig_setmethods__["second"] = _gtmfe.pairll_second_set
    __swig_getmethods__["second"] = _gtmfe.pairll_second_get
    if _newclass:second = _swig_property(_gtmfe.pairll_second_get, _gtmfe.pairll_second_set)
    def __len__(self): return 2
    def __repr__(self): return str((self.first, self.second))
    def __getitem__(self, index): 
      if not (index % 2): 
        return self.first
      else:
        return self.second
    def __setitem__(self, index, val):
      if not (index % 2): 
        self.first = val
      else:
        self.second = val
    __swig_destroy__ = _gtmfe.delete_pairll
    __del__ = lambda self : None;
pairll_swigregister = _gtmfe.pairll_swigregister
pairll_swigregister(pairll)

class vecll(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vecll, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vecll, name)
    __repr__ = _swig_repr
    def iterator(self): return _gtmfe.vecll_iterator(self)
    def __iter__(self): return self.iterator()
    def __nonzero__(self): return _gtmfe.vecll___nonzero__(self)
    def __bool__(self): return _gtmfe.vecll___bool__(self)
    def __len__(self): return _gtmfe.vecll___len__(self)
    def pop(self): return _gtmfe.vecll_pop(self)
    def __getslice__(self, *args): return _gtmfe.vecll___getslice__(self, *args)
    def __setslice__(self, *args): return _gtmfe.vecll___setslice__(self, *args)
    def __delslice__(self, *args): return _gtmfe.vecll___delslice__(self, *args)
    def __delitem__(self, *args): return _gtmfe.vecll___delitem__(self, *args)
    def __getitem__(self, *args): return _gtmfe.vecll___getitem__(self, *args)
    def __setitem__(self, *args): return _gtmfe.vecll___setitem__(self, *args)
    def append(self, *args): return _gtmfe.vecll_append(self, *args)
    def empty(self): return _gtmfe.vecll_empty(self)
    def size(self): return _gtmfe.vecll_size(self)
    def clear(self): return _gtmfe.vecll_clear(self)
    def swap(self, *args): return _gtmfe.vecll_swap(self, *args)
    def get_allocator(self): return _gtmfe.vecll_get_allocator(self)
    def begin(self): return _gtmfe.vecll_begin(self)
    def end(self): return _gtmfe.vecll_end(self)
    def rbegin(self): return _gtmfe.vecll_rbegin(self)
    def rend(self): return _gtmfe.vecll_rend(self)
    def pop_back(self): return _gtmfe.vecll_pop_back(self)
    def erase(self, *args): return _gtmfe.vecll_erase(self, *args)
    def __init__(self, *args): 
        this = _gtmfe.new_vecll(*args)
        try: self.this.append(this)
        except: self.this = this
    def push_back(self, *args): return _gtmfe.vecll_push_back(self, *args)
    def front(self): return _gtmfe.vecll_front(self)
    def back(self): return _gtmfe.vecll_back(self)
    def assign(self, *args): return _gtmfe.vecll_assign(self, *args)
    def resize(self, *args): return _gtmfe.vecll_resize(self, *args)
    def insert(self, *args): return _gtmfe.vecll_insert(self, *args)
    def reserve(self, *args): return _gtmfe.vecll_reserve(self, *args)
    def capacity(self): return _gtmfe.vecll_capacity(self)
    __swig_destroy__ = _gtmfe.delete_vecll
    __del__ = lambda self : None;
vecll_swigregister = _gtmfe.vecll_swigregister
vecll_swigregister(vecll)


def mfe_main(*args):
  return _gtmfe.mfe_main(*args)
mfe_main = _gtmfe.mfe_main
# This file is compatible with both classic and new-style classes.


