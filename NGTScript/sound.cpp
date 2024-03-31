//NGTAUDIO
#define NOMINMAX
#include "dlibrary.h"
#include "sound.h"
#include "ngt.h"
#include <thread>
#include "phonon.h""
IPLContextSettings contextSettings{};
IPLContext context = nullptr;
IPLHRTF hrtf = nullptr;
IPLAudioSettings audioSettings{};
IPLerror phonon_init() {
    contextSettings.version = STEAMAUDIO_VERSION;
    IPLerror errorCode = iplContextCreate(&contextSettings, &context);
    audioSettings.samplingRate = 44100;
    audioSettings.frameSize = 256; // the size of audio buffers we intend to process

    IPLHRTFSettings hrtfSettings{};
    hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

    errorCode=iplHRTFCreate(context, &audioSettings, &hrtfSettings, &hrtf);
    return errorCode;
}
typedef struct {
    IPLBinauralEffectSettings effectSettings{};
    IPLBinauralEffect effect;
}phonon_sound;
IPLerror phonon_effect_init(phonon_sound* sound_source = nullptr) {
    sound_source->effectSettings.hrtf = hrtf;
    return iplBinauralEffectCreate(context, &audioSettings, &sound_source->effectSettings, &sound_source->effect);
}
void phonon_process(HDSP handle, BASSDWORD channel, void* buffer, BASSDWORD length, phonon_sound* audio_source) {
    IPLAudioBuffer inBuffer;
    IPLAudioBuffer outBuffer;
    inBuffer.numChannels = 2;
    inBuffer.data =reinterpret_cast<float**>( buffer);
    inBuffer.numSamples = length;
    IPLBinauralEffectParams params{};
    params.direction = IPLVector3{ 1.0f, 1.0f, 1.0f };
    params.hrtf = hrtf;
    params.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
    params.spatialBlend = 1.0f;
    params.peakDelays = nullptr;
    iplBinauralEffectApply(audio_source->effect, &params, &inBuffer, &outBuffer);

}
//BASS
//Typedefs
typedef uint8_t BASSBYTE;
typedef uint16_t BASSWORD;
typedef uint32_t BASSDWORD;
typedef uint64_t BASSQWORD;
typedef BASSDWORD HMUSIC;		// MOD music handle
typedef BASSDWORD HSAMPLE;		// sample handle
typedef BASSDWORD HCHANNEL;		// sample playback handle
typedef BASSDWORD HSTREAM;		// sample stream handle
typedef BASSDWORD HRECORD;		// recording handle
typedef BASSDWORD HSYNC;		// synchronizer handle
typedef BASSDWORD HDSP;			// DSP handle
typedef BASSDWORD HFX;			// effect handle
typedef BASSDWORD HPLUGIN;		// plugin handle
//Bass constants
const int BASS_OK = 0; // all is OK
const int BASS_ERROR_MEM = 1; // memory error
const int BASS_ERROR_FILEOPEN = 2; // can't open the file
const int BASS_ERROR_DRIVER = 3; // can't find a free/valid driver
const int BASS_ERROR_BUFLOST = 4; // the sample buffer was lost
const int BASS_ERROR_HANDLE = 5; // invalid handle
const int BASS_ERROR_FORMAT = 6; // unsupported sample format
const int BASS_ERROR_POSITION = 7; // invalid position
const int BASS_ERROR_INIT = 8; // BASS_Init has not been successfully called
const int BASS_ERROR_START = 9; // BASS_Start has not been successfully called
const int BASS_ERROR_SSL = 10; // SSL/HTTPS support isn't available
const int BASS_ERROR_ALREADY = 14; // already initialized/paused/whatever
const int BASS_ERROR_NOCHAN = 18; // can't get a free channel
const int BASS_ERROR_ILLTYPE = 19; // an illegal type was specified
const int BASS_ERROR_ILLPARAM = 20; // an illegal parameter was specified
const int BASS_ERROR_NO3D = 21; // no 3D support
const int BASS_ERROR_NOEAX = 22; // no EAX support
const int BASS_ERROR_DEVICE = 23; // illegal device number
const int BASS_ERROR_NOPLAY = 24; // not playing
const int BASS_ERROR_FREQ = 25; // illegal sample rate
const int BASS_ERROR_NOTFILE = 27; // the stream is not a file stream
const int BASS_ERROR_NOHW = 29; // no hardware voices available
const int BASS_ERROR_EMPTY = 31; // the MOD music has no sequence data
const int BASS_ERROR_NONET = 32; // no internet connection could be opened
const int BASS_ERROR_CREATE = 33; // couldn't create the file
const int BASS_ERROR_NOFX = 34; // effects are not available
const int BASS_ERROR_NOTAVAIL = 37; // requested data/action is not available
const int BASS_ERROR_DECODE = 38; // the channel is/isn't a "decoding channel"
const int BASS_ERROR_DX = 39; // a sufficient DirectX version is not installed
const int BASS_ERROR_TIMEOUT = 40; // connection timedout
const int BASS_ERROR_FILEFORM = 41; // unsupported file format
const int BASS_ERROR_SPEAKER = 42; // unavailable speaker
const int BASS_ERROR_VERSION = 43; // invalid BASS version (used by add-ons)
const int BASS_ERROR_CODEC = 44; // codec is not available/supported
const int BASS_ERROR_ENDED = 45; // the channel/file has ended
const int BASS_ERROR_BUSY = 46; // the device is busy
const int BASS_ERROR_UNKNOWN = -1; // some other mystery problem
const int BASS_CONFIG_BUFFER = 0;
const int BASS_CONFIG_UPDATEPERIOD = 1;
const int BASS_CONFIG_GVOL_SAMPLE = 4;
const int BASS_CONFIG_GVOL_STREAM = 5;
const int BASS_CONFIG_GVOL_MUSIC = 6;
const int BASS_CONFIG_CURVE_VOL = 7;
const int BASS_CONFIG_CURVE_PAN = 8;
const int BASS_CONFIG_FLOATDSP = 9;
const int BASS_CONFIG_3DALGORITHM = 10;
const int BASS_CONFIG_NET_TIMEOUT = 11;
const int BASS_CONFIG_NET_BUFFER = 12;
const int BASS_CONFIG_PAUSE_NOPLAY = 13;
const int BASS_CONFIG_NET_PREBUF = 15;
const int BASS_CONFIG_NET_PASSIVE = 18;
const int BASS_CONFIG_REC_BUFFER = 19;
const int BASS_CONFIG_NET_PLAYLIST = 21;
const int BASS_CONFIG_MUSIC_VIRTUAL = 22;
const int BASS_CONFIG_VERIFY = 23;
const int BASS_CONFIG_UPDATETHREADS = 24;
const int BASS_CONFIG_DEV_BUFFER = 27;
const int BASS_CONFIG_VISTA_TRUEPOS = 30;
const int BASS_CONFIG_IOS_MIXAUDIO = 34;
const int BASS_CONFIG_DEV_DEFAULT = 36;
const int BASS_CONFIG_NET_READTIMEOUT = 37;
const int BASS_CONFIG_VISTA_SPEAKERS = 38;
const int BASS_CONFIG_IOS_SPEAKER = 39;
const int BASS_CONFIG_MF_DISABLE = 40;
const int BASS_CONFIG_HANDLES = 41;
const int BASS_CONFIG_UNICODE = 42;
const int BASS_CONFIG_SRC = 43;
const int BASS_CONFIG_SRC_SAMPLE = 44;
const int BASS_CONFIG_ASYNCFILE_BUFFER = 45;
const int BASS_CONFIG_OGG_PRESCAN = 47;
const int BASS_CONFIG_MF_VIDEO = 48;
const int BASS_CONFIG_AIRPLAY = 49;
const int BASS_CONFIG_DEV_NONSTOP = 50;
const int BASS_CONFIG_IOS_NOCATEGORY = 51;
const int BASS_CONFIG_VERIFY_NET = 52;
const int BASS_CONFIG_DEV_PERIOD = 53;
const int BASS_CONFIG_FLOAT = 54;
const int BASS_CONFIG_NET_SEEK = 56;
const int BASS_CONFIG_AM_DISABLE = 58;
const int BASS_CONFIG_NET_PLAYLIST_DEPTH = 59;
const int BASS_CONFIG_NET_PREBUF_WAIT = 60;
const int BASS_CONFIG_NET_AGENT = 16;
const int BASS_CONFIG_NET_PROXY = 17;
const int BASS_CONFIG_IOS_NOTIFY = 46;
const int BASS_DEVICE_8BITS = 1; // 8 bit
const int BASS_DEVICE_MONO = 2; // mono
const int BASS_DEVICE_3D = 4; // enable 3D functionality
const int BASS_DEVICE_16BITS = 8; // limit output to 16 bit
const int BASS_DEVICE_LATENCY = 0x100; // calculate device latency (BASS_INFO struct)
const int BASS_DEVICE_CPSPEAKERS = 0x400; // detect speakers via Windows control panel
const int BASS_DEVICE_SPEAKERS = 0x800; // force enabling of speaker assignment
const int BASS_DEVICE_NOSPEAKER = 0x1000; // ignore speaker arrangement
const int BASS_DEVICE_DMIX = 0x2000; // use ALSA "dmix" plugin
const int BASS_DEVICE_FREQ = 0x4000; // set device sample rate
const int BASS_DEVICE_STEREO = 0x8000; // limit output to stereo
const int BASS_DEVICE_HOG = 0x10000; // hog/exclusive mode
const int BASS_DEVICE_AUDIOTRACK = 0x20000; // use AudioTrack output
const int BASS_DEVICE_DSOUND = 0x40000; // use DirectSound output
const int BASS_OBJECT_DS = 1; // IDirectSound
const int BASS_OBJECT_DS3DL = 2; // IDirectSound3DListener
const int BASS_DEVICE_ENABLED = 1;
const int BASS_DEVICE_DEFAULT = 2;
const int BASS_DEVICE_INIT = 4;
const int BASS_DEVICE_LOOPBACK = 8;
const int BASS_DEVICE_TYPE_MASK = 0x000000;
const int BASS_DEVICE_TYPE_NETWORK = 0x01000000;
const int BASS_DEVICE_TYPE_SPEAKERS = 0x02000000;
const int BASS_DEVICE_TYPE_LINE = 0x03000000;
const int BASS_DEVICE_TYPE_HEADPHONES = 0x04000000;
const int BASS_DEVICE_TYPE_MICROPHONE = 0x05000000;
const int BASS_DEVICE_TYPE_HEADSET = 0x06000000;
const int BASS_DEVICE_TYPE_HANDSET = 0x07000000;
const int BASS_DEVICE_TYPE_DIGITAL = 0x08000000;
const int BASS_DEVICE_TYPE_SPDIF = 0x09000000;
const int BASS_DEVICE_TYPE_HDMI = 0x0a000000;
const int BASS_DEVICE_TYPE_DISPLAYPORT = 0x40000000;
const int BASS_DEVICES_AIRPLAY = 0x1000000;
const int DSCAPS_CONTINUOUSRATE = 0x00000010; // supports all sample rates between min/maxrate
const int DSCAPS_EMULDRIVER = 0x00000020; // device does NOT have hardware DirectSound support
const int DSCAPS_CERTIFIED = 0x00000040; // device driver has been certified by Microsoft
const int DSCAPS_SECONDARYMONO = 0x00000100; // mono
const int DSCAPS_SECONDARYSTEREO = 0x00000200; // stereo
const int DSCAPS_SECONDARY8BIT = 0x00000400; // 8 bit
const int DSCAPS_SECONDARY16BIT = 0x00000800; // 16 bit
const int DSCCAPS_EMULDRIVER = DSCAPS_EMULDRIVER; // device does NOT have hardware DirectSound recording support
const int DSCCAPS_CERTIFIED = DSCAPS_CERTIFIED; // device driver has been certified by Microsoft
const int BASS_SAMPLE_8BITS = 1; // 8 bit
const int BASS_SAMPLE_FLOAT = 256; // 32 bit floating-point
const int BASS_SAMPLE_MONO = 2; // mono
const int BASS_SAMPLE_LOOP = 4; // looped
const int BASS_SAMPLE_3D = 8; // 3D functionality
const int BASS_SAMPLE_SOFTWARE = 16; // not using hardware mixing
const int BASS_SAMPLE_MUTEMAX = 32; // mute at max distance (3D only)
const int BASS_SAMPLE_VAM = 64; // DX7 voice allocation & management
const int BASS_SAMPLE_FX = 128; // old implementation of DX8 effects
const int BASS_SAMPLE_OVER_VOL = 0x10000; // override lowest volume
const int BASS_SAMPLE_OVER_POS = 0x20000; // override longest playing
const int BASS_SAMPLE_OVER_DIST = 0x30000; // override furthest from listener (3D only)
const int BASS_STREAM_PRESCAN = 0x20000; // enable pin-point seeking/length (MP3/MP2/MP1)
const int BASS_STREAM_AUTOFREE = 0x40000; // automatically free the stream when it stop/ends
const int BASS_STREAM_RESTRATE = 0x80000; // restrict the download rate of internet file streams
const int BASS_STREAM_BLOCK = 0x100000; // download/play internet file stream in small blocks
const int BASS_STREAM_DECODE = 0x200000; // don't play the stream, only decode (BASS_ChannelGetData)
const int BASS_STREAM_STATUS = 0x800000; // give server status info (HTTP/ICY tags) in DOWNLOADPROC
const int BASS_MP3_IGNOREDELAY = 0x200; // ignore LAME/Xing/VBRI/iTunes delay & padding info
const int BASS_MP3_SETPOS = BASS_STREAM_PRESCAN;
const int BASS_MUSIC_FLOAT = BASS_SAMPLE_FLOAT;
const int BASS_MUSIC_MONO = BASS_SAMPLE_MONO;
const int BASS_MUSIC_LOOP = BASS_SAMPLE_LOOP;
const int BASS_MUSIC_3D = BASS_SAMPLE_3D;
const int BASS_MUSIC_FX = BASS_SAMPLE_FX;
const int BASS_MUSIC_AUTOFREE = BASS_STREAM_AUTOFREE;
const int BASS_MUSIC_DECODE = BASS_STREAM_DECODE;
const int BASS_MUSIC_PRESCAN = BASS_STREAM_PRESCAN; // calculate playback length
const int BASS_MUSIC_CALCLEN = BASS_MUSIC_PRESCAN;
const int BASS_MUSIC_RAMP = 0x200; // normal ramping
const int BASS_MUSIC_RAMPS = 0x400; // sensitive ramping
const int BASS_MUSIC_SURROUND = 0x800; // surround sound
const int BASS_MUSIC_SURROUND2 = 0x1000; // surround sound (mode 2)
const int BASS_MUSIC_FT2PAN = 0x2000; // apply FastTracker 2 panning to XM files
const int BASS_MUSIC_FT2MOD = 0x2000; // play .MOD as FastTracker 2 does
const int BASS_MUSIC_PT1MOD = 0x4000; // play .MOD as ProTracker 1 does
const int BASS_MUSIC_NONINTER = 0x10000; // non-interpolated sample mixing
const int BASS_MUSIC_SINCINTER = 0x800000; // sinc interpolated sample mixing
const int BASS_MUSIC_POSRESET = 0x8000; // stop all notes when moving position
const int BASS_MUSIC_POSRESETEX = 0x400000; // stop all notes and reset bmp/etc when moving position
const int BASS_MUSIC_STOPBACK = 0x80000; // stop the music on a backwards jump effect
const int BASS_MUSIC_NOSAMPLE = 0x100000; // don't load the samples
const int BASS_SPEAKER_FRONT = 0x1000000; // front speakers
const int BASS_SPEAKER_REAR = 0x2000000; // rear/side speakers
const int BASS_SPEAKER_CENLFE = 0x3000000; // center & LFE speakers (5.1)
const int BASS_SPEAKER_REAR2 = 0x4000000; // rear center speakers (7.1)
const int BASS_SPEAKER_LEFT = 0x10000000; // modifier: left
const int BASS_SPEAKER_RIGHT = 0x20000000; // modifier: right
const int BASS_SPEAKER_FRONTLEFT = BASS_SPEAKER_FRONT | BASS_SPEAKER_LEFT;
const int BASS_SPEAKER_FRONTRIGHT = BASS_SPEAKER_FRONT | BASS_SPEAKER_RIGHT;
const int BASS_SPEAKER_REARLEFT = BASS_SPEAKER_REAR | BASS_SPEAKER_LEFT;
const int BASS_SPEAKER_REARRIGHT = BASS_SPEAKER_REAR | BASS_SPEAKER_RIGHT;
const int BASS_SPEAKER_CENTER = BASS_SPEAKER_CENLFE | BASS_SPEAKER_LEFT;
const int BASS_SPEAKER_LFE = BASS_SPEAKER_CENLFE | BASS_SPEAKER_RIGHT;
const int BASS_SPEAKER_REAR2LEFT = BASS_SPEAKER_REAR2 | BASS_SPEAKER_LEFT;;;
const int BASS_SPEAKER_REAR2RIGHT = BASS_SPEAKER_REAR2 | BASS_SPEAKER_RIGHT;
const int BASS_ASYNCFILE = 0x40000000;
const int BASS_RECORD_PAUSE = 0x8000; // start recording paused
const int BASS_RECORD_ECHOCANCEL = 0x2000;
const int BASS_RECORD_AGC = 0x4000;
const int BASS_VAM_HARDWARE = 1;
const int BASS_VAM_SOFTWARE = 2;
const int BASS_VAM_TERM_TIME = 4;
const int BASS_VAM_TERM_DIST = 8;
const int BASS_VAM_TERM_PRIO = 16;
const int BASS_ORIGRES_FLOAT = 0x10000;
const int BASS_CTYPE_SAMPLE = 1;
const int BASS_CTYPE_RECORD = 2;
const int BASS_CTYPE_STREAM = 0x10000;
const int BASS_CTYPE_STREAM_OGG = 0x10002;
const int BASS_CTYPE_STREAM_MP1 = 0x10003;
const int BASS_CTYPE_STREAM_MP2 = 0x10004;
const int BASS_CTYPE_STREAM_MP3 = 0x10005;
const int BASS_CTYPE_STREAM_AIFF = 0x10006;
const int BASS_CTYPE_STREAM_CA = 0x10007;
const int BASS_CTYPE_STREAM_MF = 0x10008;
const int BASS_CTYPE_STREAM_AM = 0x10009;
const int BASS_CTYPE_STREAM_DUMMY = 0x18000;
const int BASS_CTYPE_STREAM_DEVICE = 0x18001;
const int BASS_CTYPE_STREAM_WAV = 0x40000; // WAVE flag, LOWORD=codec
const int BASS_CTYPE_STREAM_WAV_PCM = 0x50001;
const int BASS_CTYPE_STREAM_WAV_FLOAT = 0x50003;
const int BASS_CTYPE_MUSIC_MOD = 0x20000;
const int BASS_CTYPE_MUSIC_MTM = 0x20001;
const int BASS_CTYPE_MUSIC_S3M = 0x20002;
const int BASS_CTYPE_MUSIC_XM = 0x20003;
const int BASS_CTYPE_MUSIC_IT = 0x20004;
const int BASS_CTYPE_MUSIC_MO3 = 0x00100; // MO3 flag
const int BASS_3DMODE_NORMAL = 0; // normal 3D processing
const int BASS_3DMODE_RELATIVE = 1; // position is relative to the listener
const int BASS_3DMODE_OFF = 2; // no 3D processing
const int BASS_3DALG_DEFAULT = 0;
const int BASS_3DALG_OFF = 1;
const int BASS_3DALG_FULL = 2;
const int BASS_3DALG_LIGHT = 3;
const int BASS_ATTRIB_FREQ = 1;
const int BASS_ATTRIB_VOL = 2;
const int BASS_ATTRIB_PAN = 3;
const int BASS_ATTRIB_EAXMIX = 4;
const int BASS_ATTRIB_NOBUFFER = 5;
const int BASS_ATTRIB_VBR = 6;
const int BASS_ATTRIB_CPU = 7;
const int BASS_ATTRIB_SRC = 8;
const int BASS_ATTRIB_NET_RESUME = 9;
const int BASS_ATTRIB_SCANINFO = 10;
const int BASS_ATTRIB_NORAMP = 11;
const int BASS_ATTRIB_BITRATE = 12;
const int BASS_ATTRIB_BUFFER = 13;
const int BASS_FX_FREESOURCE = 0x10000;
const int BASS_ATTRIB_MUSIC_AMPLIFY = 0x100;
const int BASS_ATTRIB_MUSIC_PANSEP = 0x101;
const int BASS_ATTRIB_MUSIC_PSCALER = 0x102;
const int BASS_ATTRIB_MUSIC_BPM = 0x103;
const int BASS_ATTRIB_MUSIC_SPEED = 0x104;
const int BASS_ATTRIB_MUSIC_VOL_GLOBAL = 0x105;
const int BASS_ATTRIB_MUSIC_ACTIVE = 0x106;
const int BASS_ATTRIB_MUSIC_VOL_CHAN = 0x200;
const int BASS_ATTRIB_MUSIC_VOL_INST = 0x300;
const int BASS_SLIDE_LOG = 0x1000000;
const int BASS_DATA_AVAILABLE = 0; // query how much data is buffered
const int BASS_DATA_NOREMOVE = 0x10000000; // flag: don't remove data from recording buffer
const int BASS_DATA_FIXED = 0x20000000; // flag: return 8.24 fixed-point data
const int BASS_DATA_FLOAT = 0x40000000; // flag: return floating-point sample data
const int BASS_DATA_FFT_INDIVIDUAL = 0x10; // FFT flag: FFT for each channel, else all combined
const int BASS_DATA_FFT_NOWINDOW = 0x20; // FFT flag: no Hanning window
const int BASS_DATA_FFT_REMOVEDC = 0x40; // FFT flag: pre-remove DC bias
const int BASS_DATA_FFT_COMPLEX = 0x80; // FFT flag: return complex data
const int BASS_DATA_FFT_NYQUIST = 0x100; // FFT flag: return extra Nyquist value
const int BASS_LEVEL_MONO = 1; // get mono level
const int BASS_LEVEL_STEREO = 2; // get stereo level
const int BASS_LEVEL_RMS = 4; // get RMS levels
const int BASS_LEVEL_VOLPAN = 8; // apply VOL/PAN attributes to the levels
const int BASS_LEVEL_NOREMOVE = 16; // don't remove data from recording buffer
const int BASS_INPUT_ON = 0x20000;
const int BASS_INPUT_OFF = 0x10000;
const int BASS_FX_DX8_CHORUS = 0;
const int BASS_FX_DX8_COMPRESSOR = 1;
const int BASS_FX_DX8_DISTORTION = 2;
const int BASS_FX_DX8_ECHO = 3;
const int BASS_FX_DX8_FLANGER = 4;
const int BASS_FX_DX8_GARGLE = 5;
const int BASS_FX_DX8_I3DL2REVERB = 6;
const int BASS_FX_DX8_PARAMEQ = 7;
const int BASS_FX_DX8_REVERB = 8;
const int BASS_FX_VOLUME = 9;
const int BASS_ACTIVE_STOPPED = 0;
const int BASS_ACTIVE_PLAYING = 1;
const int BASS_ACTIVE_STALLED = 2;
const int BASS_ACTIVE_PAUSED = 3;
const int BASS_DX8_PHASE_NEG_180 = 0;
const int BASS_DX8_PHASE_NEG_90 = 1;
const int BASS_DX8_PHASE_ZERO = 2;
const int BASS_DX8_PHASE_90 = 3;
const int BASS_DX8_PHASE_180 = 4;
const int BASS_TAG_ID3 = 0;// ID3v1 tags : TAG_ID3 structure
const int BASS_TAG_ID3V2 = 1;// ID3v2 tags : variable length block
const int BASS_TAG_OGG = 2;// OGG comments : series of null-terminated UTF-8 strings
const int BASS_TAG_HTTP = 3;// HTTP headers : series of null-terminated ANSI strings
const int BASS_TAG_ICY = 4;// ICY headers : series of null-terminated ANSI strings
const int BASS_TAG_META = 5;// ICY metadata : ANSI string
const int BASS_TAG_APE = 6;// APE tags : series of null-terminated UTF-8 strings
const int BASS_TAG_MP4 = 7;// MP4/iTunes metadata : series of null-terminated UTF-8 strings
const int BASS_TAG_WMA = 8;// WMA tags : series of null-terminated UTF-8 strings
const int BASS_TAG_VENDOR = 9;// OGG encoder : UTF-8 string
const int BASS_TAG_LYRICS3 = 10;// Lyric3v2 tag : ASCII string
const int BASS_TAG_CA_CODEC = 11;// CoreAudio codec info : TAG_CA_CODEC structure
const int BASS_TAG_MF = 13;// Media Foundation tags : series of null-terminated UTF-8 strings
const int BASS_TAG_WAVEFORMAT = 14;// WAVE format : WAVEFORMATEEX structure
const int BASS_TAG_AM_MIME = 15;// Android Media MIME type : ASCII string
const int BASS_TAG_AM_NAME = 16;// Android Media codec name : ASCII string
const int BASS_TAG_RIFF_INFO = 0x100;// RIFF "INFO" tags : series of null-terminated ANSI strings
const int BASS_TAG_RIFF_BEXT = 0x101;// RIFF/BWF "bext" tags : TAG_BEXT structure
const int BASS_TAG_RIFF_CART = 0x102;// RIFF/BWF "cart" tags : TAG_CART structure
const int BASS_TAG_RIFF_DISP = 0x103;// RIFF "DISP" text tag : ANSI string
const int BASS_TAG_RIFF_CUE = 0x104;// RIFF "cue " chunk : TAG_CUE structure
const int BASS_TAG_RIFF_SMPL = 0x105;// RIFF "smpl" chunk : TAG_SMPL structure
const int BASS_TAG_APE_BINARY = 0x1000;// + index #, binary APE tag : TAG_APE_BINARY structure
const int BASS_TAG_MUSIC_NAME = 0x10000;// MOD music name : ANSI string
const int BASS_TAG_MUSIC_MESSAGE = 0x10001;// MOD message : ANSI string
const int BASS_TAG_MUSIC_ORDERS = 0x10002;// MOD order list : BYTE array of pattern numbers
const int BASS_TAG_MUSIC_AUTH = 0x10003;// MOD author : UTF-8 string
const int BASS_TAG_MUSIC_INST = 0x10100;// + instrument #, MOD instrument name : ANSI string
const int BASS_TAG_MUSIC_SAMPLE = 0x10300;// + sample #, MOD sample name : ANSI string
const int BASSVERSION = 0x204; 	// API version
const std::string BASSVERSIONTEXT = "2.4";
typedef struct {
    float fInGain;
    float fReverbMix;
    float fReverbTime;
    float fHighFreqRTRatio;
} BASS_DX8_REVERB;
dlibrary bass;
void soundsystem_init() {
    bass.load(L"bass.dll");
    lfunc BASS_Init = bass.get("BASS_Init");
    BASS_Init(-1, 44100, 0, 0, NULL);
}
void soundsystem_free() {
    lfunc BASS_Free = bass.get("BASS_Free");
    BASS_Free();
    bass.unload();
}
    std::string sound_path;
void set_sound_storage(const std::string& path) {
    sound_path = path;
}
std::string get_sound_storage() {
    return sound_path;
}
void set_master_volume(float volume) {

    if (volume > 0 or volume < -100)return;
}
float get_master_volume() {
    return 0;
}
void mixer_start() {
    lfunc BASS_Start = bass.get("BASS_Start");
    BASS_Start();
}
void mixer_stop() {
    lfunc BASS_Pause = bass.get("BASS_Pause");
    BASS_Pause();

}
bool mixer_play_sound(const std::string& filename) {
    std::string result;
    if (sound_path != "") {
        result = sound_path + "/" + filename.c_str();
    }
    else {
        result = filename;
    }
    return false;
}
void mixer_reinit() {
    soundsystem_free();
    soundsystem_init();
}
bool sound_global_hrtf = false;
bool inited = false;
class  mixer {
public:
};
class sound {
public:
    bool is_3d_;
    bool playing = false, paused = false, active = false;
    HSTREAM handle_;
    HFX reverb;
    sound(const std::string  &filename="", bool set3d=false) {
        if (!inited) {
            soundsystem_init();
            inited = true;
        }
        if (filename != "")
            this->load(filename);
    }
    ~sound() {
        if (this->active)this->close();
    }

    bool load(const std::string& filename) {
        BASSDWORD flags=0;
        std::string result;
        if (sound_path != "") {
            result = sound_path + "/" + filename.c_str();
        }
        else {
            result = filename;
        }
        if (active) {
            this->close();
            active = false;
        }
        lfunc BASS_StreamCreateFile = bass.get("BASS_StreamCreateFile");
        handle_=reinterpret_cast<HSTREAM>(BASS_StreamCreateFile(false, result.c_str(), 0, 0, flags));
        if (handle_ != 0)
            active = true;
        return handle_ == 0;
    }
    bool load_from_memory(const std::string& data, bool set3d) {
        BASSDWORD flags = 0;

        if (active) {
            this->close();
            active = false;
        }
        lfunc BASS_StreamCreateFile = bass.get("BASS_StreamCreateFile");
        handle_ = reinterpret_cast<HSTREAM>(BASS_StreamCreateFile(true, data.c_str(), 0, strlen(data.c_str()), flags));
        if (handle_ != 0)
            active = true;
        return handle_ == 0;

    }
    bool stream(const std::string& filename, bool set3d) {
        std::string result;
        if (sound_path != "") {
            result = sound_path + "/" + filename.c_str();
        }
        else {
            result = filename;
        }
        if (active) {
            this->close();
            active = false;
        }
        return true;
    }
    bool load_url(const std::string& url, bool set3d) {

        if (active) {
            this->close();
            active = false;
        }
        lfunc BASS_StreamCreateURL= bass.get("BASS_StreamCreateURL");
        handle_ = reinterpret_cast<HSTREAM>(BASS_StreamCreateURL(url.c_str(), 0, 0, NULL, 0));
        if (handle_ != 0)
            active = true;
        return handle_ == 0;
    }
    std::string push_memory() {
        return NULL;
    }
    void set_faid_time(float volume_beg, float volume_end, float time) {
    }
    bool play(bool restart=true) {
        if (!active)return false;
        lfunc BASS_ChannelFlags = bass.get("BASS_ChannelFlags");
        BASS_ChannelFlags(handle_, BASS_SAMPLE_LOOP, 0);
        lfunc BASS_ChannelPlay = bass.get("BASS_ChannelPlay");
        return BASS_ChannelPlay(handle_, restart);
        }
    bool play_looped(bool restart=true) {
        if (!active)return false;

        lfunc BASS_ChannelFlags = bass.get("BASS_ChannelFlags");
        BASS_ChannelFlags(handle_, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
        lfunc BASS_ChannelPlay = bass.get("BASS_ChannelPlay");
        return BASS_ChannelPlay(handle_, restart);
    }
    bool pause() {
        if (!active)return false;
        lfunc BASS_ChannelPause = bass.get("BASS_ChannelPause");
        return BASS_ChannelPause(handle_);
    }
    bool play_wait(bool restart=true) {
        this->play(restart);
        while (true) {
            update_game_window();
            delay(5);
            bool ac = this->is_playing();
            if (ac == false) {
                break;
            }
        }
        return true;
    }

    bool stop() {
        if (!active)return false;
        lfunc BASS_ChannelStop= bass.get("BASS_ChannelStop");
        return BASS_ChannelStop(handle_);
    }    
    bool close() {
        if (!active)return false;
        active = false;
        lfunc BASS_StreamFree = bass.get("BASS_StreamFree");
        return BASS_StreamFree(handle_);
    }
    void set_fx(const std::string& fx, int pr = 0) {
        if (!active)return;
        lfunc BASS_ChannelSetFX = bass.get("BASS_ChannelSetFX");
        if (fx == "reverb") {
            reverb = reinterpret_cast<HFX>(BASS_ChannelSetFX(handle_, BASS_FX_DX8_REVERB, pr));
        }
    }
    void delete_fx(const std::string& fx) {
        if (!active)return;
    }
    void set_reverb_parameters(float dry, float wet, float room_size, float damping, float mode) {
        if (!active)return;
        lfunc BASS_FXSetParameters = bass.get("BASS_FXSetParameters");
        BASS_DX8_REVERB params;
        params.fInGain = dry;
        params.fReverbMix = wet+100;
        params.fReverbTime = room_size;
        params.fHighFreqRTRatio = damping;
        BASS_FXSetParameters(reverb, params);
    }
    void set_delay_parameters(float dry, float wet, float dcay) {
        if (!active)return;
    }
    void set_position(float listener_x, float listener_y, float listener_z, float source_x, float source_y, float source_z, float theta=0, float pan_step=1, float volume_step=1, float behind_pitch_decrease=0, float start_pan=0, float start_volume=0, float start_pitch=0) {
        if (!active) return;
        float delta_x = 0;
        float delta_y = 0;
        float delta_z = 0;
        float final_pan = start_pan;
        float final_volume = start_volume;
        float final_pitch = start_pitch;
        float rotational_source_x = source_x;
        float rotational_source_y = source_y;
        if (theta > 0.0)
        {
            rotational_source_x = (cos(theta) * (source_x - listener_x)) - (sin(theta) * (source_y - listener_y)) + listener_x;
            rotational_source_y = (sin(theta) * (source_x - listener_x)) + (cos(theta) * (source_y - listener_y)) + listener_y;
            source_x = rotational_source_x;
            source_y = rotational_source_y;
        }
        if (source_x < listener_x)
        {
            delta_x = listener_x - source_x;
            final_pan -= (delta_x * pan_step)+70;
            final_volume -= (delta_x * volume_step);
        }
        if (source_x > listener_x)
        {
            delta_x = source_x - listener_x;
            final_pan += (delta_x * pan_step)+70;
            final_volume -= (delta_x * volume_step);
        }
        if (source_y < listener_y)
        {
            final_pitch -= abs(behind_pitch_decrease);
            delta_y = listener_y - source_y;
            final_volume -= (delta_y * volume_step);
        }
        if (source_y > listener_y)
        {
            delta_y = source_y - listener_y;
            final_volume -= (delta_y * volume_step);
        }
        if (source_z < listener_z)
        {
            final_pitch -= abs(behind_pitch_decrease);
            delta_z = listener_z - source_z;
            final_volume -= (delta_z * volume_step);
        }
        if (source_z > listener_z)
        {
            delta_z = source_z - listener_z;
            final_volume -= (delta_z * volume_step);
        }
        if (final_pan < -100)
        {
            final_pan = -100;
        }
        if (final_pan > 100)
        {
            final_pan = 100;
        }
        if (final_volume < -100)
        {
            final_volume = -100;
        }
            this->set_pan ( final_pan);
            this->set_volume ( final_volume);
            this->set_pitch ( final_pitch);
    }

        void set_position(const ngtvector* listener=nullptr, const ngtvector* source=nullptr) {
        if (!active)return;
        this->set_position(listener->x, listener->y, listener->z, source->x, source->y, source->z);
        }
    void set_hrtf(bool hrtf) {
        if (!active)return;
    }
    void set_volume_step(float volume_step) {
        if (!active)return;
    }
    void set_pan_step(float pan_step) {
        if (!active)return;
}
    void set_pitch_step(float pitch_step) {
        if (!active)return;
}
    bool seek(float new_position) {
        if (!active)return false;
        if (new_position > this->get_length())
            return false;
        return true;
    }
    void set_looping(bool looping) {
        if (!active)return;
    }
    bool get_looping()const {
        if (!active)return false;
    }
    float get_pan() const {
        if (!active)return -17435;

        float pan;
        lfunc func = bass.get("BASS_ChannelGetAttribute");
        //Using function signature to do this
        typedef bool(*func_sig)(HSTREAM, BASSDWORD, float*);
        func_sig BASS_ChannelGetAttribute = reinterpret_cast<func_sig>(func);
        BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_PAN, &pan);

        return pan * 100;
    }

    void set_pan(float pan) {
        if (!active)return;
        lfunc func = bass.get("BASS_ChannelSetAttribute");
        //Using function signature to do this
        typedef bool(*func_sig)(HSTREAM, BASSDWORD, float);
        func_sig BASS_ChannelSetAttribute = reinterpret_cast<func_sig>(func);
        BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_PAN, pan/ 100);
    }

    float get_volume() const {
        if (!active)return -17435;

        float volume ;
        lfunc func= bass.get("BASS_ChannelGetAttribute");
//Using function signature to do this
        typedef bool(*func_sig)(HSTREAM, BASSDWORD, float*);
        func_sig BASS_ChannelGetAttribute = reinterpret_cast<func_sig>(func);
        BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_VOL, &volume);

        return 20 * log10f(volume);
    }
            void set_volume(float volume) {
        if (!active)return;
        lfunc func = bass.get("BASS_ChannelSetAttribute");
        //Using function signature to do this
        typedef bool(*func_sig)(HSTREAM, BASSDWORD, float);
        func_sig BASS_ChannelSetAttribute = reinterpret_cast<func_sig>(func);
        BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_VOL, powf(10, volume / 20.0f));

            }
        float get_pitch() const {
        if (!active)return -17435;
        float pitch = 0;
        lfunc BASS_ChannelGetAttribute = bass.get("BASS_ChannelGetAttribute");
        BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_FREQ, pitch);

        return pitch * 100;
    }

        void set_pitch(float pitch) {
            if (!active)return;
            lfunc BASS_ChannelSetAttribute = bass.get("BASS_ChannelSetAttribute");
            BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_FREQ, pitch/ 100);

        }
        void set_speed(float speed) {
}
        bool is_active() const {
        return active;
    }

    bool is_playing() const {
        if (!active)return false;
        lfunc BASS_ChannelIsActive = bass.get("BASS_ChannelIsActive");
        BASSDWORD state =reinterpret_cast<BASSDWORD> (BASS_ChannelIsActive(handle_));
        return reinterpret_cast<BASSDWORD>( BASS_ChannelIsActive(handle_)) == BASS_ACTIVE_PLAYING;
    }
        
    bool is_paused() const {
        if (!active)return false;
    }

    float get_position()  {
        if (!active)return -17435;
        float position=0;
        return position;
    }

    float get_length()  {
        if (!active)return-17435;
        float length = 0;
        return static_cast<float>(length/100);
    }

    void set_length(float length = 0.0) {
        if (!active)return;
        if (length > this->get_length())            return;
    }
    float get_sample_rate() const {
        float rate = 0;
        return rate;
    }
};
void set_sound_global_hrtf(bool hrtf) {
    sound_global_hrtf = hrtf;
}
bool get_sound_global_hrtf() {
    return sound_global_hrtf;
}
sound* fsound(const std::string& filename, bool set3d) { return new sound(filename, set3d); }
mixer* fmixer() { return new mixer; }
void register_sound(asIScriptEngine* engine) {
    engine->RegisterFuncdef("void sound_end_callback(string=\"\")");
    engine->RegisterGlobalFunction("void set_sound_storage(const string &in)property", asFUNCTION(set_sound_storage), asCALL_CDECL);

    engine->RegisterGlobalFunction("string get_sound_storage()property", asFUNCTION(get_sound_storage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_master_volume(float)property", asFUNCTION(set_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("float get_master_volume()property", asFUNCTION(get_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_start()", asFUNCTION(mixer_start), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_stop()", asFUNCTION(mixer_stop), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool mixer_play_sound(const string &in)", asFUNCTION(mixer_play_sound), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_reinit()", asFUNCTION(mixer_reinit), asCALL_CDECL);
    engine->RegisterObjectType("mixer", sizeof(mixer), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("mixer", asBEHAVE_FACTORY, "mixer @m()", asFUNCTION(fmixer), asCALL_CDECL);
    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_FACTORY, "sound@ s(const string &in=\"\", bool=false)", asFUNCTION(fsound), asCALL_CDECL);
    engine->RegisterObjectMethod("sound", "bool load(const string &in, bool=false, sound_end_callback@=null)const", asMETHOD(sound, load), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool load_from_memory(const string &in, bool=false)const", asMETHOD(sound, load_from_memory), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stream(const string &in, bool=false)const", asMETHOD(sound, stream), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool load_url(const string &in, bool=false)const", asMETHOD(sound, load_url), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "string push_memory()const", asMETHOD(sound, push_memory), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_faid_time(float, float, float)const", asMETHOD(sound, set_faid_time), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "bool play()const", asMETHOD(sound, play), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_looped()const", asMETHOD(sound, play_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool pause()const", asMETHOD(sound, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_wait()const", asMETHOD(sound, play_wait), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stop()const", asMETHOD(sound, stop), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool close()const", asMETHOD(sound, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_fx(const string &in, int=0)const", asMETHOD(sound, set_fx), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void delete_fx(const string &in, int=0)const", asMETHOD(sound, delete_fx), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_reverb_parameters(float, float, float, float, float)const", asMETHOD(sound, set_reverb_parameters), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_delay_parameters(float, float, float)const", asMETHOD(sound, set_delay_parameters), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_position(float, float, float, float, float, float, float=0, float=1  , float=1, float=0, float=0, float=0, float=0)const", asMETHODPR(sound, set_position, (float, float, float, float, float, float, float, float, float, float, float, float, float), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_position(const vector@=null, const vector@=null)const", asMETHODPR(sound, set_position, (const ngtvector*, const ngtvector*), void), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_hrtf(bool=true)const property", asMETHOD(sound, set_hrtf), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume_step(float)const property", asMETHOD(sound, set_volume_step), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan_step(float)const property", asMETHOD(sound, set_pan_step), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch_step(float)const property", asMETHOD(sound, set_pitch_step), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool seek(float)const", asMETHOD(sound, seek), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_looping() const property", asMETHOD(sound, get_looping), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_looping(bool)const property", asMETHOD(sound, set_looping), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "float get_pan() const property", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(float)const property", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_volume() const property", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(float)const property", asMETHOD(sound, set_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_pitch() const property", asMETHOD(sound, get_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch(float)const property", asMETHOD(sound, set_pitch), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "float get_speed() const property", asMETHOD(sound, get_speed), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_speed(float)const property", asMETHOD(sound, set_speed), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "bool get_active() const property", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_playing() const property", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_paused() const property", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_position() const property", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_length() const property", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_length(float=0.0) const property", asMETHOD(sound, set_length), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "float get_sample_rate() const property", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);
    engine->RegisterGlobalFunction("void set_sound_global_hrtf(bool)property", asFUNCTION(set_sound_global_hrtf), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool get_sound_global_hrtf()property", asFUNCTION(get_sound_global_hrtf), asCALL_CDECL);
//    engine->RegisterGlobalProperty("mixer@ sound_default_mixer", (void*)&sound_default_mixer);
}