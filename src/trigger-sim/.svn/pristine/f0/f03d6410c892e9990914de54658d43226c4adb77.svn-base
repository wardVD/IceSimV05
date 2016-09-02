from icecube import dataclasses
from icecube.dataclasses import I3TriggerHierarchy
from icecube.dataclasses import I3Trigger
from icecube.dataclasses import TriggerKey

def find_triggers(th, key):
    return_list = list()
    for t in th:
        if t.key == key:
            return_list.append(t)
    return return_list

def n_triggers(th, sourceID = None, typeID = None, configID = None):
    if sourceID == None and \
            typeID == None and \
            configID == None : return 0
    return_value = 0
    for t in th:
        if ( sourceID == None or t.key.source == sourceID ) and \
        ( typeID == None or t.key.type == typeID ) and \
        ( configID == None or t.key.config_id == configID ) :
            return_value += 1
    return return_value      

def trigger_lengths(th, sourceID = None, typeID = None, configID = None):
    return_list = list() 
    if sourceID == None and \
            typeID == None and \
            configID == None : return return_list
    for t in th:
        if ( sourceID == None or t.key.source == sourceID ) and \
        ( typeID == None or t.key.type == typeID ) and \
        ( configID == None or t.key.config_id == configID ) :
            return_list.append( t.length )
    return return_list

from icecube import dataclasses as dc
def generate_trigger_key(key_str):
    """This function generates a TriggerKey from the string output.

    This function takes a string of the form "[<source>:<type>:<config_id>]"
    and is useful for generating a TriggerKey after printing one from a GCD file.

    Example :
    In [1]: from icecube import dataclasses
    In [2]: from icecube import dataio
    In [3]: from icecube.sim_services.sim_utils.gcd_utils import get_triggerstatus
    In [4]: f = dataio.I3File(<path_to_gcd_file>)
    In [5]: ts = get_triggerstatus(f)
    In [6]: for key in ts.keys() : print key
    [IN_ICE:SIMPLE_MULTIPLICITY:1006]
    [IN_ICE:SIMPLE_MULTIPLICITY:1011]
    [IN_ICE:SLOW_PARTICLE:24002]
    [IN_ICE:STRING:1007]
    [IN_ICE:VOLUME:21001]
    [ICE_TOP:SIMPLE_MULTIPLICITY:102]
    In [7]: from icecube.trigger_sim.trigger_hierarchy_recipes import generate_trigger_key
    In [8]: key = generate_trigger_key('[IN_ICE:SIMPLE_MULTIPLICITY:1006]')
    In [9]: trigger_config = ts[key]
    In [10]: for setting in trigger_config.trigger_settings : print setting
    (threshold, 8)
    (timeWindow, 5000)
    """
    if not key_str.startswith('[') or \
      not key_str.endswith(']') or \
      key_str.count(':') != 2 :
        print ("ERROR : Malformed string '%s'" % key_str)
        print ("Expecting a string of the form '[<source>:<type>:<config_id>]'")
        return None

    s = key_str.strip('[').strip(']').split(':')
    source = getattr(dataclasses.I3Trigger,s[0])
    ttype = getattr(dataclasses.I3Trigger,s[1])
    config_id = int(s[2])

    return dataclasses.TriggerKey(source, ttype, config_id)    



trigger_config_IDs = dict()
trigger_config_IDs["2014"] = dict()
trigger_config_IDs["2014"]["SMT8"] = 1006
trigger_config_IDs["2014"]["SMT3"] = 1011
trigger_config_IDs["2014"]["SMT6"] = 102
trigger_config_IDs["2014"]["SLOP"] = 24002
trigger_config_IDs["2014"]["STRING"] = 1007
trigger_config_IDs["2014"]["VOLUME"] = 21001

trigger_config_IDs["2013"] = dict()
trigger_config_IDs["2013"]["SMT8"] = 1006
trigger_config_IDs["2013"]["SMT3"] = 1011
trigger_config_IDs["2013"]["SMT6"] = 102
trigger_config_IDs["2013"]["SLOP"] = 24002
trigger_config_IDs["2013"]["STRING"] = 1007
trigger_config_IDs["2013"]["VOLUME"] = 21001

trigger_config_IDs["2012"] = dict()
trigger_config_IDs["2012"]["SMT8"] = 1006
trigger_config_IDs["2012"]["SMT3"] = 1011
trigger_config_IDs["2012"]["SMT6"] = 102
trigger_config_IDs["2012"]["SLOP"] = 24002
trigger_config_IDs["2012"]["STRING"] = 1007
trigger_config_IDs["2012"]["VOLUME"] = 21001

trigger_config_IDs["2011"] = dict()
trigger_config_IDs["2011"]["SMT8"] = 1006
trigger_config_IDs["2011"]["SMT3"] = 1011
trigger_config_IDs["2011"]["SMT6"] = 102
trigger_config_IDs["2011"]["SLOP"] = 22005
trigger_config_IDs["2011"]["STRING"] = 1007
trigger_config_IDs["2011"]["VOLUME"] = 21001

trigger_config_IDs["IC79"] = dict()
trigger_config_IDs["IC79"]["SMT8"] = 1006
trigger_config_IDs["IC79"]["SMT3"] = 1010
trigger_config_IDs["IC79"]["SMT6"] = 102
trigger_config_IDs["IC79"]["STRING"] = 1007

trigger_config_IDs["IC59"] = dict()
trigger_config_IDs["IC59"]["SMT8"] = 1006
trigger_config_IDs["IC59"]["SMT3"] = 1008
trigger_config_IDs["IC59"]["SMT6"] = 102
trigger_config_IDs["IC59"]["ULEE_57"] = 117
trigger_config_IDs["IC59"]["ULEE_62"] = 118
trigger_config_IDs["IC59"]["ULEE_63"] = 119
trigger_config_IDs["IC59"]["ULEE_64"] = 120
trigger_config_IDs["IC59"]["ULEE_19"] = 1000
trigger_config_IDs["IC59"]["ULEE_28"] = 1001
trigger_config_IDs["IC59"]["ULEE_38"] = 1002
trigger_config_IDs["IC59"]["ULEE_47"] = 1003
trigger_config_IDs["IC59"]["ULEE_48"] = 1004
trigger_config_IDs["IC59"]["ULEE_56"] = 1005
trigger_config_IDs["IC59"]["STRING"] = 1007

trigger_config_IDs["IC40"] = dict()
trigger_config_IDs["IC40"]["SMT8"] = 103
trigger_config_IDs["IC40"]["SMT6"] = 102


