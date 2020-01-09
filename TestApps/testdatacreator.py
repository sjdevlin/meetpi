import pyaudio
import math
import struct
import wave
import os
#
Threshold = .7 # audio level to start recording

SHORT_NORMALIZE = (1.0/32768.0)
chunk = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
swidth = 2

CLIP_LENGTH = 2

f_name_directory = '/Users/Dev/Documents/MeetPi/Data'

class Recorder:

    @staticmethod
    def rms(frame):
        count = len(frame) / swidth
        format = "%dh" % (count)
        shorts = struct.unpack(format, frame)

        sum_squares = 0.0
        for sample in shorts:
            n = sample * SHORT_NORMALIZE
            sum_squares += n * n
            rms = math.pow(sum_squares / count, 0.5)

            return rms * 1000

    def __init__(self):
        self.p = pyaudio.PyAudio()
        self.stream = self.p.open(format=FORMAT,
                                  channels=CHANNELS,
                                  rate=RATE,
                                  input=True,
                                  output=True,
                                  frames_per_buffer=chunk)


    def write(self, recording):
        WAVE_OUTPUT_FILENAME = "test_"
        n_files = len(os.listdir(f_name_directory))
        filename = os.path.join(f_name_directory, WAVE_OUTPUT_FILENAME + '{}.wav'.format(n_files))
        wf = wave.open(filename, 'wb')
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(self.p.get_sample_size(FORMAT))
        wf.setframerate(RATE)
        wf.writeframes(recording)
        wf.close()
        print('Written to file: {}'.format(filename))



    def listen(self):
        print ('starting...')
        recording_underway = False
           
        audiobuffer = []
        while True:
            data = self.stream.read(chunk)
            rms_val = self.rms(data)
            audiobuffer.append(data)
            if len(audiobuffer)>50000:
                audiobuffer=[]

            if not recording_underway and rms_val > Threshold:
                recording_underway  = True
                time_since_last_audio = 0
                print('Start new audio')
            if recording_underway:
                if rms_val < Threshold:
                    time_since_last_audio += 1
                else:         
                    time_since_last_audio = 0
                
            if recording_underway and time_since_last_audio > 50:
                recording_underway = False
                cursor = len(audiobuffer)
                self.write(b''.join(audiobuffer))
                audiobuffer=[]
                
a = Recorder()
#object_type = input('input type of object e.g. bottle or can: ')
a.listen()
