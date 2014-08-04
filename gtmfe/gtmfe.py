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
    def __init__(self): 
        this = _gtmfe.new_PolytopeVector()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _gtmfe.delete_PolytopeVector
    __del__ = lambda self : None;
PolytopeVector_swigregister = _gtmfe.PolytopeVector_swigregister
PolytopeVector_swigregister(PolytopeVector)

class ParameterVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ParameterVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ParameterVector, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _gtmfe.ParameterVector_a_set
    __swig_getmethods__["a"] = _gtmfe.ParameterVector_a_get
    if _newclass:a = _swig_property(_gtmfe.ParameterVector_a_get, _gtmfe.ParameterVector_a_set)
    __swig_setmethods__["b"] = _gtmfe.ParameterVector_b_set
    __swig_getmethods__["b"] = _gtmfe.ParameterVector_b_get
    if _newclass:b = _swig_property(_gtmfe.ParameterVector_b_get, _gtmfe.ParameterVector_b_set)
    __swig_setmethods__["c"] = _gtmfe.ParameterVector_c_set
    __swig_getmethods__["c"] = _gtmfe.ParameterVector_c_get
    if _newclass:c = _swig_property(_gtmfe.ParameterVector_c_get, _gtmfe.ParameterVector_c_set)
    __swig_setmethods__["d"] = _gtmfe.ParameterVector_d_set
    __swig_getmethods__["d"] = _gtmfe.ParameterVector_d_get
    if _newclass:d = _swig_property(_gtmfe.ParameterVector_d_get, _gtmfe.ParameterVector_d_set)
    def __init__(self): 
        this = _gtmfe.new_ParameterVector()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _gtmfe.delete_ParameterVector
    __del__ = lambda self : None;
ParameterVector_swigregister = _gtmfe.ParameterVector_swigregister
ParameterVector_swigregister(ParameterVector)


def mfe_main(*args):
  return _gtmfe.mfe_main(*args)
mfe_main = _gtmfe.mfe_main
# This file is compatible with both classic and new-style classes.

