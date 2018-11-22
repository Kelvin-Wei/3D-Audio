import os, sys
import wave, struct
import argparse
import numpy as np
from scipy import signal

parser = argparse.ArgumentParser()
parser.add_argument("--input","-i")
parser.add_argument("--output","-o",type=str,default="3d_output.wav")
parser.add_argument("--rate","-r",type=int,default=12)
parser.add_argument("--level","-l",type=int,choices=range(-40, 100, 10),default=0)
parser.add_argument("--clockwise","-c",action="store_true")
parser.set_defaults(clockwise=False)

class Audio3D():

    def __init__(self):
        self.__load_hrtf()

    def __str2dat(self, string):
        return np.fromstring(string, dtype=np.short).reshape((-1,2)).T.astype(np.float32)

    def __dat2str(self, data):
        return data.T.reshape((-1,)).astype(np.short).tostring()

    def __angle2id(self, angle):
        return angle / 5

    def __load_hrtf(self):
        self.__hrtf_dict = {}
        level_list = range(-40, 100, 10)
        for lv in level_list:
            level_name = "level%d" % lv
            self.__hrtf_dict[level_name] = np.load("res/level%d.npy" % lv)

    def __load_audio(self, audio_file):
        if not audio_file.endswith(".wav"):
            assert("No *.wav file is found.")
        wav_file = wave.open(audio_file, 'r')
        wav_params = wav_file.getparams()
        wav_nframes = wav_params[3]
        wav_raw = wav_file.readframes(wav_nframes)
        wav_file.close()
        return self.__str2dat(wav_raw), wav_params

    def __save_audio(self, outp_file, params, data):
        wav_file = wave.open(outp_file, 'w')
        params = list(params)
        wav_file.setparams(params)
        wav_file.writeframes(self.__dat2str(data))
        wav_file.close()

    def __norm(self, data):
        data_scale = np.max(np.abs(data))
        return data / data_scale, data_scale

    def __filter(self, wav_data, hrtf_data, start, end):
        blank = end - start
        st = max(0, start - blank)
        hrtf_data = hrtf_data.reshape((2,-1))
        wav_slide = wav_data[:,st:end]
        outp_slide = np.zeros((2,end-st))
        outp_slide[0,:] = signal.lfilter(hrtf_data[0], np.array([1.]), wav_slide[0,:])
        outp_slide[1,:] = signal.lfilter(hrtf_data[1], np.array([1.]), wav_slide[1,:])
        if outp_slide.shape[1] != end - start:
            outp_slide = outp_slide[:,blank:]
        return outp_slide

    def __run(self, wav_data, hrtf_data, duration, nframes, frame_per_rate, rate, direction):
        outp_track = np.zeros((2, nframes))
        angle_id = 0
        boundary = hrtf_data.shape[0]
        for i in range(duration * rate):
            start = i * frame_per_rate
            end = min(start + frame_per_rate, nframes)
            angle_id += direction
            angle_id += boundary
            angle_id %= boundary
            outp_track[:,start:end] = self.__filter(wav_data, hrtf_data[angle_id], start, end)
        return outp_track

    def perform(self, audio_file, rate, level, direction, outp_file):
        track, track_param = self.__load_audio(audio_file)
        track, track_scale = self.__norm(track)
        duration = track_param[3] / track_param[2]
        frame_per_rate = track_param[2] / rate
        outp_track = self.__run(track, self.__hrtf_dict["level%d" % level], duration, track_param[3], frame_per_rate, rate, direction)
        outp_track = self.__norm(outp_track)[0] * track_scale
        self.__save_audio(outp_file, track_param, outp_track)

if __name__ == "__main__":
    args = parser.parse_args()
    audio3d = Audio3D()
    audio3d.perform(args.input,
                    args.rate,
                    args.level,
                    1 if args.clockwise else -1,
                    args.output)