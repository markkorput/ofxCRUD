#!/usr/bin/env python
import time, math, random, logging
import OSC


OSC_HOST = '127.0.0.1'
OSC_PORT = 8080

class App:
    def __init__(self, options = {}):
        self.options = options
        self.client = OSC.OSCClient()
        self.host = OSC_HOST
        self.port = OSC_PORT
        self.bRunning = False
        self.nextIndex = 1

        self.logger = logging.getLogger(__name__)
        if 'verbose' in self.options and self.options['verbose']:
            self.logger.setLevel(logging.DEBUG)

    def setup(self):
        try:
            self.client.connect((OSC_HOST, OSC_PORT))
        except OSC.OSCClientError as err:
            self.logger.error("OSC connection failure: {0}".format(err))
            return False

        self.bRunning = True
        return True

    def isRunning(self):
        return self.bRunning

    def update(self):
        self.send('/ofxCRUD/Node/create/'+str(self.nextIndex))
        self.send('/ofxCRUD/Node/update/'+str(self.nextIndex)+'/autoMove', ['true'])
        # self.send('/ofxCRUD/ImageNode/update/'+str(self.nextIndex)+'/file', ['images/dance'+str(int(random.random()*10)+1)+'.jpg'])
        # self.send('/ofxCRUD/ImageNode/update/'+str(self.nextIndex)+'/pos', [",".join([str(random.random()*500), str(random.random()*300), '0'])])
        self.nextIndex += 1
        time.sleep(1.0)

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
            self.logger.error(str(err))

        self.logger.info('osc-out {0}:{1} - {2} [{3}]'.format(self.host, self.port, addr, ", ".join(map(lambda x: str(x), data))))

if __name__ == '__main__':
    logging.basicConfig()
    app = App({'verbose': True})
    app.setup()

    try:
        while(app.isRunning()):
            app.update()
    except KeyboardInterrupt:
        print('KeyboardInterrupt. Quitting.')
