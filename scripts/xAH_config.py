import ROOT
# load this for the MSG::level values. See https://its.cern.ch/jira/browse/ATLASG-270
ROOT.asg.ToolStore()

import logging
logger = logging.getLogger("xAH_config")
logger.setLevel(10) # we use info

# for generating names if needed
from xAH_nameGenerator import xAH_nameGenerator

class xAH_config(object):
  def __init__(self):
    self._algorithms = []
    self._log        = []

  def setalg(self, className, options):
    # check first argument
    if not isinstance(className, str):
      raise TypeError("className must be a string")

    if not isinstance(options, dict):
      raise TypeError("Must pass in a dictionary of options")

    # if m_name not set, randomly generate it
    algName = options.get("m_name", None)
    if algName is None:
      algName = str(xAH_nameGenerator())
      logger.warning("Setting missing m_name={0:s} for instance of {1:s}".format(algName, className))
      options['m_name'] = algName
    if not isinstance(algName, str) and not isinstance(algName, unicode):
      raise TypeError("'m_name' must be a string for instance of {0:s}".format(className))

    # handle deprecation of m_debug, m_verbose
    if 'm_debug' in options:
      logger.warning("m_debug is being deprecated. See https://github.com/UCATLAS/xAODAnaHelpers/pull/882 . Set m_msgLevel='debug'")
    if 'm_verbose' in options:
      logger.warning("m_verbose is being deprecated. See https://github.com/UCATLAS/xAODAnaHelpers/pull/882 . Set m_msgLevel='verbose'.")

    # handle msgLevels, can be string or integer
    msgLevel = options.get("m_msgLevel", "info")
    if not isinstance(msgLevel, str) and not isinstance(msgLevel, int):
      raise TypeError("m_msgLevel must be a string or integer for instance of {0:s}".format(className))
    if isinstance(msgLevel, str):
      if not hasattr(ROOT.MSG, msgLevel.upper()):
        raise ValueError("m_msgLevel must be a valid MSG::level: {0:s}".format(msgLevel))
      msgLevel = getattr(ROOT.MSG, msgLevel.upper())
    options['m_msgLevel'] = msgLevel

    #
    # Construct the given constructor
    #    (complicated b/c we have to deal nesting of namespaces)
    #
    alg = reduce(lambda x,y: getattr(x, y, None), className.split('.'), ROOT)
    if alg is None:
      raise AttributeError(className)

    #
    # Construct an instance of the alg and set its attributes
    #
    alg_obj = alg()
    alg_obj.SetName(algName)
    alg_obj.setMsgLevel(msgLevel)
    self._log.append((alg,algName))
    for k,v in options.iteritems():
      # only crash on algorithm configurations that aren't m_msgLevel and m_name (xAH specific)
      if not hasattr(alg_obj, k) and k not in ['m_msgLevel', 'm_name']:
        raise AttributeError(k)
      self._log.append((alg, k, v))
      try:
        setattr(alg_obj, k, v)
      except:
        logger.error("There was a problem setting {0:s} to {1} for {2:s}::{3:s}".format(k, v, className, algName))
        raise

    #print

    #
    # Add the constructed algo to the list of algorithms to run
    #
    self._algorithms.append(alg_obj)
