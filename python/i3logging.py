
import logging
from icecube.icetray import I3Logger, I3LoggerBase, I3LogLevel

class LoggingBridge(I3LoggerBase):
	pylevels = {
		I3LogLevel.LOG_TRACE : 5,
		I3LogLevel.LOG_DEBUG : logging.DEBUG,
		I3LogLevel.LOG_INFO  : logging.INFO,
		I3LogLevel.LOG_WARN  : logging.WARNING,
		I3LogLevel.LOG_ERROR : logging.ERROR,
		I3LogLevel.LOG_FATAL : logging.CRITICAL,
	}
	i3levels = dict([(v, k) for k, v in pylevels.iteritems()])
	def getLogger(self, unit):
		if len(unit) > 0:
			name = "icecube.%s" % unit
		else:
			name = "icecube"
		return logging.getLogger(name)
	def log(self, level, unit, file, line, func, msg):
		logger = self.getLogger(unit)
		record = logging.LogRecord(logger.name, self.pylevels[level], file, line, msg, tuple(), None, None)
		logger.handle(record)
	def getLevelForUnit(self, unit):
		self.i3levels.get(self.getLogger(unit).getEffectiveLevel(), I3LogLevel.LOG_FATAL)
	def setLevelForUnit(self, unit, level):
		self.getLogger(unit).setLevel(self.pylevels[level])
	def setLevel(self, level):
		self.getLogger("".setLevel(self.pylevels[level]))

BASIC_FORMAT = "%(filename)s:%(lineno)s %(levelname)s: %(message)s"

def _setup(format=BASIC_FORMAT):
	logging.addLevelName(5, 'TRACE')
	logging.basicConfig(format=format)
	I3Logger.global_logger = LoggingBridge()

def console():
	_setup()

def rotating_files(filename, maxBytes=0, backupCount=0):
	from logging.handlers import RotatingFileHandler
	_setup()
	handler = RotatingFileHandler(filename, maxBytes=maxBytes, backupCount=backupCount)
	handler.setFormatter(logging.Formatter("[%(asctime)s] "+BASIC_FORMAT))
	logging._acquireLock()
	logging.root.handlers = list()
	logging.root.addHandler(handler)
	logging._releaseLock()

def syslog():
	from logging.handlers import SysLogHandler
	_setup()
	handler = SysLogHandler()
	handler.setFormatter(logging.Formatter("[%(asctime)s] "+BASIC_FORMAT))
	logging._acquireLock()
	logging.root.handlers = list()
	logging.root.addHandler(handler)
	logging._releaseLock()
	
