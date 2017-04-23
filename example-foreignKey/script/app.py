#!/usr/bin/env python
import time, math, random, logging
import OSC
from osc_server import OscServer

OSC_HOST = '127.0.0.1'
OSC_PORT = 8080
OSC_INCOMING_PORT = 8082


class App:
    def __init__(self, options = {}):
        self.options = options
        self.client = OSC.OSCClient()
        self.host = OSC_HOST
        self.port = OSC_PORT
        self.bRunning = False
        self.nextIndex = 1
        self.oscServer = OscServer()

        self.logger = logging.getLogger(__name__)
        if 'verbose' in self.options and self.options['verbose']:
            self.logger.setLevel(logging.DEBUG)

    def setup(self):
        try:
            self.client.connect((OSC_HOST, OSC_PORT))
        except OSC.OSCClientError as err:
            self.logger.error("OSC connection failure: {0}".format(err))
            return False

        self.send('/ofxCRUD/Node/create/1')
        self.send('/ofxCRUD/Node/update/1/autoMove', ['true'])
        self.nextIndex += 1
        # self.send('/ofxCRUD/ImageNode/update/'+str(self.nextIndex)+'/file', ['images/dance'+str(int(random.random()*10)+1)+'.jpg'])
        # self.send('/ofxCRUD/ImageNode/update/'+str(self.nextIndex)+'/pos', [",".join([str(random.random()*500), str(random.random()*300), '0'])])
        self.send('/ofxCRUD/Node/create/2')
        self.send('/ofxCRUD/Node/update/2/autoMove', ['false'])
        self.nextIndex += 1

        self.oscServer.setup('', OSC_INCOMING_PORT)
        self.oscServer.messageEvent += self._onOscMessage
        self.bRunning = True
        return True

    def isRunning(self):
        return self.bRunning

    def update(self):
        time.sleep(1.0)
        self.send('/ofxCRUD/Node/read/1/value/127.0.0.1/'+str(OSC_INCOMING_PORT))
        self.oscServer.update()

    def send(self, addr, data=[]):
        msg = OSC.OSCMessage()
        msg.setAddress(addr)

        for item in data:
            msg.append(item)

        try:
            self.client.send(msg)
        except OSC.OSCClientError as err:
            pass
        except AttributeError as err:
            self.logger.error('[osc-out {0}:{1}] error:'.format(self.host, self.port))
            #self.logger.error(str(err))

        self.logger.info('osc-out {0}:{1} - {2} [{3}]'.format(self.host, self.port, addr, ", ".join(map(lambda x: str(x), data))))

    def _onOscMessage(self, addr, tags, data, client_address):
        self.logger.info('got msg: '+addr+" with: " + ",".join(data))
        if addr == '/ofxCRUD/Node/update/1/value':
            self.send('/ofxCRUD/Node/update/2/value', data)

if __name__ == '__main__':
    logging.basicConfig()
    app = App({'verbose': True})
    app.setup()

    try:
        while(app.isRunning()):
            app.update()
    except KeyboardInterrupt:
        print('KeyboardInterrupt. Quitting.')
