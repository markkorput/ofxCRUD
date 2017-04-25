#!/usr/bin/env python
import time, math, random, logging

from osc_server2 import OscServer
from osc_client import OscClient

OSC_HOST = '127.0.0.1'
OSC_PORT = 8080
OSC_INCOMING_PORT = 8082

class App:
    def __init__(self, options = {}):
        self.options = options

        self.host = OSC_HOST
        self.port = OSC_PORT
        self.running = False
        self.nextIndex = 1

        self.logger = logging.getLogger(__name__)
        if 'verbose' in self.options and self.options['verbose']:
            self.logger.setLevel(logging.DEBUG)

        self.oscClient = OscClient(logger=self.logger)
        self.oscServer = OscServer(logger=self.logger)

    def setup(self):
        print('setup')
        self.oscClient.setup(OSC_HOST, OSC_PORT)
        self.oscServer.setup('', OSC_INCOMING_PORT)
        self.oscServer.messageEvent += self._onOscMessage
        self.running = True

        self.oscClient.send('/ofxCRUD/Node/create/1')
        self.oscClient.send('/ofxCRUD/Node/update/1/autoMove', ['true'])
        self.nextIndex += 1
        self.oscClient.send('/ofxCRUD/Node/create/2')
        self.oscClient.send('/ofxCRUD/Node/update/2/autoMove', ['false'])
        self.nextIndex += 1
        self.oscClient.send('/ofxCRUD/Node/create/3')
        self.oscClient.send('/ofxCRUD/Node/update/3/autoMove', ['false'])
        self.oscClient.send('/ofxCRUD/Node/update/3/leaderNodeId', ['1'])
        self.nextIndex += 1

        return True

    def update(self):
        self.oscClient.send('/ofxCRUD/Node/read/1/value/127.0.0.1/'+str(OSC_INCOMING_PORT))
        self.oscServer.update()

    def destroy(self):
        self.oscServer.destroy()

    def _onOscMessage(self, addr, data, tags = [], client_address = None):
        # self.logger.info('got msg: '+addr+" with: " + ",".join(data))
        if addr == '/ofxCRUD/Node/update/1/value':
            self.oscClient.send('/ofxCRUD/Node/update/2/value', data)

if __name__ == '__main__':
    logging.basicConfig()
    app = App({'verbose': True})
    app.setup()

    try:
        while(app.running):
            app.update()
    except KeyboardInterrupt:
        print('KeyboardInterrupt. Quitting.')

    app.destroy();
