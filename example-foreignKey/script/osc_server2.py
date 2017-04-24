import logging, threading, time
from evento import Event

from pythonosc import dispatcher
from pythonosc import osc_server

DEFAULT_PORT = 8081
DEFAULT_HOST = ''

class OscServer(threading.Thread):
    def __init__(self, logger=None):
        threading.Thread.__init__(self)

        # attributes
        self.osc_server = None
        self.connected = False
        self.running = False
        self.host = DEFAULT_HOST
        self.port = DEFAULT_PORT

        self.logger = logger
        if self.logger == None:
            self.logger = logging.getLogger(__name__)

        # events
        self.connectEvent = Event()
        self.disconnectEvent = Event()
        self.messageEvent = Event()

        self.queue = []

    def __del__(self):
        self.destroy()

    def setup(self, host, port):
        self.host = host
        self.port = port
        self._connect()

    def destroy(self):
        if self.connected:
            self._disconnect()

    def update(self):
        q = self.queue
        self.queue = []

        for item in q:
            if len(item) > 1:
                self.messageEvent(item[0], item[1:])
            else:
                self.messageEvent(item[0], [])

    def _connect(self):
        if self.connected:
            self.logger.warning('already connected')
            return False

        disp = dispatcher.Dispatcher()
        disp.map("/ofxCRUD/Node/update/1/value", self._onOscMessage)
        #disp.map("/volume", print_volume_handler, "Volume")
        #disp.map("/logvolume", print_compute_handler, "Log volume", math.log)

        self.osc_server = osc_server.ThreadingOSCUDPServer(
            (self.host, self.port), disp)

        self.logger.info("Starting OSC server on {}".format(self.osc_server.server_address))
        self.start()

        # set internal connected flag
        self.connected = True
        # notify
        self.connectEvent(self)
        self.logger.info("OSC Server running @ {0}:{1}".format(self.host, str(self.port)))
        return True

    def _disconnect(self):
        if self.osc_server:
            self.osc_server.shutdown()
            # del self.osc_server

        if self.isAlive():
            self.kill()

        self.connected = False
        self.osc_server = None
        self.disconnectEvent(self)
        self.logger.info('OSC Server ({0}:{1}) stopped'.format(self.host, str(self.port)))

    def _onOscMessage(self, *args, **kwargs):
        # print('got: ', args[0])

        for item in self.queue:
            if item[0] == args[0]:
                self.queue.remove(item)

        self.queue.append(args)

    # thread function
    def run(self):
        self.connected = True

        try:
            self.osc_server.serve_forever()
        except OSError as err:
            self.logger.error("Error starting OSC server:\n"+str(err))

        self.connected = False
