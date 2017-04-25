import logging

from pythonosc import osc_message_builder
from pythonosc import udp_client




class OscClient:
    def __init__(self, logger=None):
        self.client = None # OSC.OSCClient())

        self.logger = logger
        if self.logger == None:
            self.logger = logging.getLogger(__name__)

    def setup(self, host, port):
        self.host = host
        self.port = port
        # try:
        #     self.client.connect((host, port))
        # except OSC.OSCClientError as err:
        #     self.logger.error("OSC connection failure: {0}".format(err))
        #     return False
        self.client = udp_client.SimpleUDPClient(host, port)
        self.logger.info("OSC client connected to {0}:{1}".format(host, port))
        return True

    def send(self, addr, data=[]):
        # msg = OSC.OSCMessage()
        # msg.setAddress(addr)
        #
        # for item in data:
        #     msg.append(item)
        #
        # try:
        #     self.client.send(msg)
        # except OSC.OSCClientError as err:
        #     pass
        # except AttributeError as err:
        #     self.logger.error('[osc-out {0}:{1}] error:'.format(self.host, self.port))
        #     #self.logger.error(str(err))
        self.client.send_message(addr, data)
        # self.logger.info('osc-out {0}:{1} - {2} [{3}]'.format(self.host, self.port, addr, ", ".join(map(lambda x: str(x), data))))
