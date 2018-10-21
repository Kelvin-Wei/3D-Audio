import os, sys
import wave, struct, matplotlib
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

def read_wav(name):
    wav = wave.open(name, 'r')
    params = wav.getparams()
    nframes = params[3]
    wav_str = wav.readframes(nframes)
    wav.close()
    return wav_str, params

def write_wav(name, string, params):
    wav = wave.open(name, 'w')
    wav.setparams(params)
    wav.writeframes(string)
    wav.close()

def string_to_data(string):
    return np.fromstring(string, dtype=np.short).reshape((-1,2)).T.astype(np.float32)

def data_to_string(data):
    return data.T.reshape((-1,)).astype(np.short).tostring()

def read_hrtf(angle):
    path = 'res/HRTF-full/elev0'
    left_template = 'L0eXXXa.dat'
    right_template = 'R0eXXXa.dat'
    angle_str = str(angle)
    while len(angle_str) < 3:
        angle_str = '0'+angle_str
    left_file_name = left_template.replace('XXX', angle_str)
    right_file_name = right_template.replace('XXX', angle_str)
    hrtf_file = open(os.path.join(path, left_file_name), 'rb')
    left_buf = hrtf_file.read(1024)
    hrtf_file.close()
    hrtf_file = open(os.path.join(path, right_file_name), 'rb')
    right_buf = hrtf_file.read(1024)
    hrtf_file.close()
    left_data = []
    right_data = []
    for i in range(0,1024,2):
        left_data.append(struct.unpack('>h', left_buf[i:i+2])[0])
        right_data.append(struct.unpack('>h', right_buf[i:i+2])[0])
    return np.array([left_data, right_data]).reshape((-1,))

def preload_hrtf():
    hrtf = read_hrtf(0)
    for i in range(5,360,5):
        hrtf = np.vstack((hrtf, read_hrtf(i)))
    return hrtf

def angle_to_index(angle):
    return angle/5

def filter(hrtf, wav, start, end):
    blank = end - start
    s = max(0,start - blank)
    hrtf = hrtf.reshape((2,-1))
    wav_data = wav[:,s:end]
    new_wav = np.zeros((2,end-s))
    new_wav[0,:] = signal.lfilter(hrtf[0], np.array([1.]), wav_data[0,:])
    new_wav[1,:] = signal.lfilter(hrtf[1], np.array([1.]), wav_data[1,:])
    if new_wav.shape[1] == end - start:
        return new_wav
    return new_wav[:,blank:]

# body
file_name = sys.argv[1]
new_name = file_name.replace('.wav', '_sample.wav')
wav_str, wav_params = read_wav(file_name)
wav_data = string_to_data(wav_str)
wav_max = np.max(wav_data)

angle = 135
flip_angle = 360-angle
if flip_angle == 360:
    flip_angle = 0
hrtf_data1 = read_hrtf(angle)
hrtf_data2 = read_hrtf(flip_angle)

new_wav = np.zeros((2,wav_params[3]))
new_wav_params = list(wav_params)

framerate = wav_params[2]
nframes = wav_params[3]
times = nframes/framerate

hrtf_data = preload_hrtf()

rate = 12
for i in range(times*rate):
    f_rate = framerate//rate
    start = i*f_rate
    end = min(start+f_rate, nframes)
    angle = (i*10)%360
    new_wav[:,start:end] = filter(hrtf_data[angle_to_index(angle)], wav_data, start, end)
    # if i > 0:
    #     new_wav[:,start-1] = new_wav[:,start] = np.mean(new_wav[:,start-1:start+1],axis=0)

new_wav = new_wav/np.max(new_wav)*wav_max*2.

write_wav(new_name, data_to_string(new_wav), new_wav_params)
