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
        if not self.connected:
            return

        # we'll enforce a limit to the number of osc requests
        # we'll handle in a single iteration, otherwise we might
        # get stuck in processing an endless stream of data
        limit = 50
        count = 0

        # clear timed_out flag
        # self.osc_server.timed_out = False

        # # handle all pending requests then return
        # # NOTE; if you get weird bugs because self.osc_server is None,
        # # one of handled OSC messages probably triggered the destruction
        # # of this component. This should not happen until after this update
        # # loop is finished, so destructive operations should be queued for later
        # while self.osc_server.timed_out == False and count < limit:
        #     try:
        #         self.osc_server.handle_request()
        #         count += 1
        #     except Exception as exc:
        #         self.logger.error("Something went wrong while handling incoming OSC messages:")
        #         self.logger.error(exc)

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

        # try:
        #     self.osc_server = OSCServer((self.host, self.port))
        # except Exception as err:
        #     # something went wrong, cleanup
        #     self.connected = False
        #     self.osc_server = None
        #     # notify
        #     self.logger.error("{0}\nOSC Server could not start @ {1}:{2}".format(err, self.host, str(self.port)))
        #     # abort
        #     return False

        # # register time out callback
        # self.osc_server.handle_timeout = self._onTimeout
        # self.osc_server.addMsgHandler('default', self._onDefault)
        time.sleep(1)
        if not self.isAlive():
            return False

        # set internal connected flag
        self.connected = True
        # notify
        self.connectEvent(self)
        self.logger.info("OSC Server running @ {0}:{1}".format(self.host, str(self.port)))
        return True


    def _disconnect(self):
        if self.osc_server:
            self.osc_server.shutdown()
            # self.osc_server.close()
            self.connected = False
            self.osc_server = None
            self.disconnectEvent(self)
            self.logger.info('OSC Server ({0}:{1}) stopped'.format(self.host, str(self.port)))

    def _onTimeout(self):
        if self.osc_server:
            self.osc_server.timed_out = True

    def _onDefault(self, addr, tags=[], data=[], client_address=''):
        # skip touch osc touch-up events
        # if len(data) == 1 and data[0] == 0.0:
        #     return
        self.logger.debug('osc-in {0}:{1} {2} [{3}] from {4}'.format(self.host, self.port, addr,
            ", ".join(map(lambda x: str(x), data)), client_address))

        self.messageEvent(addr, data, tags, client_address)

    def _onOscMessage(self, *args, **kwargs):
        self.queue.append(args)

    # thread function
    def run(self):
        self.connected = True

        try:
            self.osc_server.serve_forever()
        except OSError as err:
            self.logger.error("Error starting OSC server:\n"+str(err))

        self.connected = False

#     def _
# def print_volume_handler(unused_addr, args, volume):
#   print("[{0}] ~ {1}".format(args[0], volume))
#
# def print_compute_handler(unused_addr, args, volume):
#   try:
#     print("[{0}] ~ {1}".format(args[0], args[1](volume)))
#   except ValueError: pass
