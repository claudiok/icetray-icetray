
import logging
from icecube.icetray import I3Logger, I3LoggerBase, I3LogLevel

class LoggingBridge(I3LoggerBase):
	levels = {
		I3LogLevel.LOG_TRACE : 5,
		I3LogLevel.LOG_DEBUG : logging.DEBUG,
		I3LogLevel.LOG_INFO  : logging.INFO,
		I3LogLevel.LOG_WARN  : logging.WARNING,
		I3LogLevel.LOG_ERROR : logging.ERROR,
		I3LogLevel.LOG_FATAL : logging.CRITICAL,
	}
	def log(self, level, unit, file, line, func, msg):
		if len(unit) > 0:
			name = "icecube.%s" % unit
		else:
			name = "icecube"
		logger = logging.getLogger(name)
		record = logging.LogRecord(logger.name, self.levels[level], file, line, msg, tuple(), None, None)
		logger.handle(record)

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
	
